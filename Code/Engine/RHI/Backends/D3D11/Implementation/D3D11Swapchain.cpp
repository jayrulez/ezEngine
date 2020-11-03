#include <RHI/Backends/D3D11/D3D11Swapchain.h>

ezString D3D11Swapchain::GetName() const
{
  ezUInt32 size = 1024 - 1;
  char* pName = new char[size + 1];

  DXGISwapChain->GetPrivateData(WKPDID_D3DDebugObjectName, &size, pName);

  pName[size] = '\0';

  ezString name(pName);

  return name;
}

void D3D11Swapchain::SetName(const ezString& name)
{
  Name = name;

  DXGISwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, Name.GetCharacterCount(), name.GetData());
}

bool D3D11Swapchain::IsDisposed() const
{
  return Disposed;
}

void D3D11Swapchain::Dispose()
{
  if (!Disposed)
  {
    if (DepthTexture != nullptr)
      DepthTexture->Dispose();

    if (Framebuffer != nullptr)
      Framebuffer->Dispose();

    if (DXGISwapChain != nullptr)
    {
      DXGISwapChain->Release();
      DXGISwapChain = nullptr;
    }

    Disposed = true;
  }
}

RHIFramebuffer* D3D11Swapchain::GetFramebuffer() const
{
  return Framebuffer;
}

void D3D11Swapchain::Resize(ezUInt32 width, ezUInt32 height)
{
  {
    ezLock lock(ReferencedCLsMutex);
    for (D3D11CommandList* cl : ReferencedCLs)
    {
      cl->Reset();
    }
    ReferencedCLs.Clear();
  }

  bool resizeBuffers = false;

  if (Framebuffer != nullptr)
  {
    resizeBuffers = true;
    if (DepthTexture != nullptr)
    {
      DepthTexture->Dispose();
      DepthTexture = nullptr;
    }

    Framebuffer->Dispose();
  }

  ezUInt32 actualWidth = (ezUInt32)(width * PixelScale);
  ezUInt32 actualHeight = (ezUInt32)(height * PixelScale);
  if (resizeBuffers)
  {
    HRESULT hr = DXGISwapChain->ResizeBuffers(2, actualWidth, actualHeight, ColorFormat, /*DXGI_SWAP_CHAIN_FLAG */ 0);
  }

  {
    ID3D11Texture2D* backBufferTexture = nullptr;
    HRESULT hr = DXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));

    if (DepthFormat.has_value())
    {
      RHITextureDescription depthDesc(
        actualWidth,                   //width
        actualHeight,                  //height
        1,                             // depth
        1,                             // mipLevels
        1,                             // arrayLayers
        DepthFormat.value(),           // Format
        RHITextureUsage::DepthStencil, //usage
        RHITextureType::Texture2D      // type
      );

      DepthTexture = new D3D11Texture(GraphicsDevice->GetDevice(), depthDesc);
    }

    D3D11Texture* backBufferRHITexture = new D3D11Texture(
      backBufferTexture,
      RHITextureType::Texture2D,
      D3D11Formats::ToRHIFormat(ColorFormat));

    ezDynamicArray<RHITexture*> colorTargets;
    colorTargets.PushBack(backBufferRHITexture);

    RHIFramebufferDescription desc(DepthTexture, colorTargets);
    Framebuffer = new D3D11Framebuffer(GraphicsDevice->GetDevice(), desc);
    Framebuffer->SetSwapchain(this);

    //backBufferTexture->Release(); // TODO: confirm that this should be done, check veldrid and maybe message mellinoe
  }
}

bool D3D11Swapchain::GetSyncToVerticalBlank() const
{
  return VSync;
}

void D3D11Swapchain::SetSyncToVerticalBlank(bool value)
{
  VSync = value;
  SyncInterval = D3D11Util::GetSyncInterval(value);
}

IDXGISwapChain* D3D11Swapchain::GetIDXGISwapChain() const
{
  return DXGISwapChain;
}

ezUInt32 D3D11Swapchain::GetSyncInterval() const
{
  return SyncInterval;
}

void D3D11Swapchain::AddCommandListReference(D3D11CommandList* cl)
{
  ezLock lock(ReferencedCLsMutex);
  ReferencedCLs.Insert(cl);
}

void D3D11Swapchain::RemoveCommandListReference(D3D11CommandList* cl)
{
  ezLock lock(ReferencedCLsMutex);
  ReferencedCLs.Remove(cl);
}

D3D11Swapchain::D3D11Swapchain(D3D11GraphicsDevice* graphicsDevice, const RHISwapchainDescription& description)
{
  GraphicsDevice = graphicsDevice;
  DepthFormat = description.DepthFormat;
  VSync = description.SyncToVerticalBlank;

  ColorFormat = description.ColorSrgb
                  ? DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
                  : DXGI_FORMAT_B8G8R8A8_UNORM;

  RHIWin32SwapchainSource* win32Source = dynamic_cast<RHIWin32SwapchainSource*>(description.Source);

  if (win32Source)
  {
    DXGI_SWAP_CHAIN_DESC dxgiSCDesc;

    dxgiSCDesc.BufferCount = 2,
    dxgiSCDesc.Windowed = true,
    dxgiSCDesc.BufferDesc.Width = description.Width;
    dxgiSCDesc.BufferDesc.Height = description.Height;
    dxgiSCDesc.BufferDesc.Format = ColorFormat;
    dxgiSCDesc.OutputWindow = (HWND)win32Source->GetHwnd();
    dxgiSCDesc.SampleDesc = {
      1, // Count
      0  // Quality
    };
    dxgiSCDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    dxgiSCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;


    {
      IDXGIFactory* dxgiFactory = nullptr;
      HRESULT hr = GraphicsDevice->GetAdapter()->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));

      hr = dxgiFactory->CreateSwapChain(GraphicsDevice->GetDevice(), &dxgiSCDesc, &DXGISwapChain);
      dxgiFactory->MakeWindowAssociation((HWND)win32Source->GetHwnd(), DXGI_MWA_NO_ALT_ENTER);

      dxgiFactory->Release();
    }

    return;
  }


  //RHIUWPSwapchainSource* uwpSource = dynamic_cast<RHIUWPSwapchainSource*>(description.Source);
  //if (uwpSource)
  //{
  //  PixelScale = uwpSource->GetLogicalDpi() / 96.0f;

  //  // Properties of the swap chain
  //  DXGI_SWAP_CHAIN_DESC1 swapChainDescription;
  //  swapChainDescription.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
  //  swapChainDescription.BufferCount = 2;
  //  swapChainDescription.Format = ColorFormat;
  //  swapChainDescription.Height = (ezUInt32)(description.Height * PixelScale);
  //  swapChainDescription.Width = (ezUInt32)(description.Width * PixelScale);
  //  swapChainDescription.SampleDesc = {
  //    1, // Count
  //    0  // Quality
  //  };
  //  swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  //  swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

  //  {
  //    // Get the Vortice.DXGI factory automatically created when initializing the Direct3D device.
  //    IDXGIFactory2* dxgiFactory = nullptr;
  //    HRESULT hr = GraphicsDevice->GetAdapter()->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory));

  //    {
  //      IDXGISwapChain1* swapChain1 = nullptr;
  //      HRESULT hr = dxgiFactory->CreateSwapChainForComposition(GraphicsDevice->GetDevice(), &swapChainDescription, nullptr, &swapChain1);
  //      // Create the swap chain and get the highest version available.

  //      //HRESULT hr = swapChain1->QueryInterface(__uuidof(IDXGISwapChain2), reinterpret_cast<void**>(&DXGISwapChain)); // TODO
  //      HRESULT hr = swapChain1->QueryInterface(__uuidof(IDXGISwapChain1), reinterpret_cast<void**>(&DXGISwapChain));
  //    }
  //  }

  //  IUnknown* unknown = nullptr;
  //  CoCreateInstance(__uuidof(IUnknown), )
  //  ComObject co = new ComObject(uwpSource->GetSwapChainPanelNative());

  //  ISwapChainPanelNative swapchainPanelNative = co.QueryInterfaceOrNull<ISwapChainPanelNative>();
  //  if (swapchainPanelNative != null)
  //  {
  //    swapchainPanelNative.SetSwapChain(_dxgiSwapChain);
  //  }
  //  else
  //  {
  //    ISwapChainBackgroundPanelNative bgPanelNative = co.QueryInterfaceOrNull<ISwapChainBackgroundPanelNative>();
  //    if (bgPanelNative != null)
  //    {
  //      bgPanelNative.SetSwapChain(_dxgiSwapChain);
  //    }
  //  }
  //}

  Resize(description.Width, description.Height);
}


EZ_STATICLINK_FILE(RHI, RHI_Backends_D3D11_Implementation_D3D11Swapchain);

