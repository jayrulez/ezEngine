#include <RHIDX11/RHIDX11PCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics/Platform/Win/HResultUtils.h>
#include <RHIDX11/Device/GraphicsDeviceDX11.h>
#include <RHIDX11/Device/CommandListDX11.h>
#include <RHIDX11/Device/DX11Utils.h>

// These will let the driver select the dedicated GPU in favour of the integrated one:
extern "C"
{
  _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
  _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

using namespace Microsoft::WRL;

using namespace DX11_Internal;


// Engine functions
ezRHIGraphicsDeviceDX11::ezRHIGraphicsDeviceDX11(bool debuglayer)
{
  DEBUGDEVICE = debuglayer;

  HRESULT hr = E_FAIL;

  uint32_t createDeviceFlags = 0;

  if (debuglayer)
  {
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  }

  D3D_DRIVER_TYPE driverTypes[] =
    {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_WARP,
      D3D_DRIVER_TYPE_REFERENCE,
    };
  uint32_t numDriverTypes = EZ_ARRAY_SIZE(driverTypes);

  D3D_FEATURE_LEVEL featureLevels[] =
    {
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
    };
  uint32_t numFeatureLevels = EZ_ARRAY_SIZE(featureLevels);

  for (uint32_t driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
  {
    driverType = driverTypes[driverTypeIndex];
    hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &device, &featureLevel, &immediateContext);

    if (SUCCEEDED(hr))
      break;
  }
  if (FAILED(hr))
  {
    EZ_REPORT_FAILURE("Failed to create the graphics device! ERROR: {0}", ezHRESULTtoString(hr) /*ez TODO: std::hex*/);
  }

  ComPtr<IDXGIDevice2> pDXGIDevice;
  hr = device.As(&pDXGIDevice);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");

  // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
  // ensures that the application will only render after each VSync, minimizing power consumption.
  hr = pDXGIDevice->SetMaximumFrameLatency(1);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");

  ComPtr<IDXGIAdapter> DXGIAdapter;
  hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&DXGIAdapter);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");

  hr = DXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&DXGIFactory);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");

  if (debuglayer)
  {
    ID3D11Debug* d3dDebug = nullptr;
    if (SUCCEEDED(device->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug)))
    {
      ID3D11InfoQueue* d3dInfoQueue = nullptr;
      if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
      {
        d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
        d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);

        D3D11_MESSAGE_ID hide[] =
          {
            D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
            // Add more message IDs here as needed
          };

        D3D11_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = _countof(hide);
        filter.DenyList.pIDList = hide;
        d3dInfoQueue->AddStorageFilterEntries(&filter);
        d3dInfoQueue->Release();
      }
      d3dDebug->Release();
    }
  }

  D3D11_QUERY_DESC queryDesc = {};
  queryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
  for (auto& x : disjointQueries)
  {
    hr = device->CreateQuery(&queryDesc, &x);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");
  }

  D3D_FEATURE_LEVEL aquiredFeatureLevel = device->GetFeatureLevel();
  if (aquiredFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
  {
    capabilities |= GRAPHICSDEVICE_CAPABILITY_TESSELLATION;
  }

  //D3D11_FEATURE_DATA_D3D11_OPTIONS features_0;
  //hr = device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS, &features_0, sizeof(features_0));

  //D3D11_FEATURE_DATA_D3D11_OPTIONS1 features_1;
  //hr = device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &features_1, sizeof(features_1));

  D3D11_FEATURE_DATA_D3D11_OPTIONS2 features_2;
  hr = device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS2, &features_2, sizeof(features_2));
  if (features_2.ConservativeRasterizationTier >= D3D11_CONSERVATIVE_RASTERIZATION_TIER_1)
  {
    capabilities |= GRAPHICSDEVICE_CAPABILITY_CONSERVATIVE_RASTERIZATION;
  }
  if (features_2.ROVsSupported == TRUE)
  {
    capabilities |= GRAPHICSDEVICE_CAPABILITY_RASTERIZER_ORDERED_VIEWS;
  }

  if (features_2.TypedUAVLoadAdditionalFormats)
  {
    // More info about UAV format load support: https://docs.microsoft.com/en-us/windows/win32/direct3d12/typed-unordered-access-view-loads
    capabilities |= GRAPHICSDEVICE_CAPABILITY_UAV_LOAD_FORMAT_COMMON;

    D3D11_FEATURE_DATA_FORMAT_SUPPORT2 FormatSupport = {};
    FormatSupport.InFormat = DXGI_FORMAT_R11G11B10_FLOAT;
    hr = device->CheckFeatureSupport(D3D11_FEATURE_FORMAT_SUPPORT2, &FormatSupport, sizeof(FormatSupport));
    if (SUCCEEDED(hr) && (FormatSupport.OutFormatSupport2 & D3D11_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
    {
      capabilities |= GRAPHICSDEVICE_CAPABILITY_UAV_LOAD_FORMAT_R11G11B10_FLOAT;
    }
  }

  D3D11_FEATURE_DATA_D3D11_OPTIONS3 features_3;
  hr = device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS3, &features_3, sizeof(features_3));
  if (features_3.VPAndRTArrayIndexFromAnyShaderFeedingRasterizer == TRUE)
  {
    capabilities |= GRAPHICSDEVICE_CAPABILITY_RENDERTARGET_AND_VIEWPORT_ARRAYINDEX_WITHOUT_GS;
  }

  CreateBackBufferResources();

  emptyresource = EZ_DEFAULT_NEW(EmptyResourceHandle);

  ezLog::Info("Created ezRHIGraphicsDeviceDX11");
}

ezRHIGraphicsDeviceDX11::~ezRHIGraphicsDeviceDX11()
{
  SubmitCommandLists();
  /*
  for (auto& commandList : m_CommandLists)
  {
    commandList->Destroy(this);
    EZ_DEFAULT_DELETE(commandList);
  }
  m_CommandLists.Clear();
  */
}

void ezRHIGraphicsDeviceDX11::CreateBackBufferResources()
{
  //HRESULT hr;

  //hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
  //if (FAILED(hr)) {
  //	wiHelper::messageBox("BackBuffer creation Failed!", "Error!");
  //	wiPlatform::Exit();
  //}

  //hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
  //if (FAILED(hr)) {
  //	wiHelper::messageBox("Main Rendertarget creation Failed!", "Error!");
  //	wiPlatform::Exit();
  //}
}

bool ezRHIGraphicsDeviceDX11::CreateSwapChain(const SwapChainDesc* pDesc, SwapChain* swapChain) const
{
  //auto internal_state = static_cast<SwapChain_DX11*>(swapChain->internal_state.Borrow());
  auto internal_state = swapChain->internal_state.Downcast<SwapChain_DX11>();
  if (swapChain->internal_state == nullptr)
  {
    internal_state = EZ_DEFAULT_NEW(SwapChain_DX11);
  }
  swapChain->internal_state = internal_state;
  swapChain->desc = *pDesc;
  HRESULT hr;

  if (internal_state->swapChain == nullptr)
  {
    // Create swapchain:
    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = pDesc->m_pWindow->GetClientAreaSize().width;
    sd.Height = pDesc->m_pWindow->GetClientAreaSize().height;
    sd.Format = _ConvertFormat(pDesc->format);
    sd.Stereo = false;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = pDesc->buffercount;
    sd.Flags = 0;
    sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS_UWP)
    sd.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;

    hr = DXGIFactory->CreateSwapChainForCoreWindow(
      device.Get(),
      pDesc->m_pWindow->GetNativeWindowHandle(),
      &sd,
      nullptr,
      &internal_state->swapChain);
#else
    sd.Scaling = DXGI_SCALING_STRETCH;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
    fullscreenDesc.RefreshRate.Numerator = 60;
    fullscreenDesc.RefreshRate.Denominator = 1;
    fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // needs to be unspecified for correct fullscreen scaling!
    fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    fullscreenDesc.Windowed = !pDesc->fullscreen;
    hr = DXGIFactory->CreateSwapChainForHwnd(
      device.Get(),
      (HWND)pDesc->m_pWindow->GetNativeWindowHandle(),
      &sd,
      &fullscreenDesc,
      nullptr,
      &internal_state->swapChain);
#endif

    if (FAILED(hr))
    {
      return false;
    }
  }
  else
  {
    // Resize swapchain:
    internal_state->backBuffer.Reset();
    internal_state->renderTargetView.Reset();

    hr = internal_state->swapChain->ResizeBuffers(
      pDesc->buffercount,
      pDesc->m_pWindow->GetClientAreaSize().width,
      pDesc->m_pWindow->GetClientAreaSize().height,
      _ConvertFormat(pDesc->format),
      0);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");
  }

  hr = internal_state->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &internal_state->backBuffer);
  if (FAILED(hr))
  {
    return false;
  }

  hr = device->CreateRenderTargetView(internal_state->backBuffer.Get(), nullptr, &internal_state->renderTargetView);
  if (FAILED(hr))
  {
    return false;
  }

  return true;
}
bool ezRHIGraphicsDeviceDX11::CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer) const
{
  auto internal_state = EZ_DEFAULT_NEW(Resource_DX11);
  pBuffer->internal_state = internal_state;
  pBuffer->type = GPUResource::GPU_RESOURCE_TYPE::BUFFER;

  D3D11_BUFFER_DESC desc;
  desc.ByteWidth = pDesc->ByteWidth;
  desc.Usage = _ConvertUsage(pDesc->Usage);
  desc.BindFlags = _ParseBindFlags(pDesc->BindFlags);
  desc.CPUAccessFlags = _ParseCPUAccessFlags(pDesc->CPUAccessFlags);
  desc.MiscFlags = _ParseResourceMiscFlags(pDesc->MiscFlags);
  desc.StructureByteStride = pDesc->StructureByteStride;

  D3D11_SUBRESOURCE_DATA data;
  if (pInitialData != nullptr)
  {
    data = _ConvertSubresourceData(*pInitialData);
  }

  pBuffer->desc = *pDesc;
  HRESULT hr = device->CreateBuffer(&desc, pInitialData == nullptr ? nullptr : &data, (ID3D11Buffer**)internal_state->resource.ReleaseAndGetAddressOf());
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "GPUBuffer creation failed!");

  if (SUCCEEDED(hr))
  {
    // Create resource views if needed
    if (pDesc->BindFlags & BIND_SHADER_RESOURCE)
    {
      CreateSubresource(pBuffer, SRV, 0);
    }
    if (pDesc->BindFlags & BIND_UNORDERED_ACCESS)
    {
      CreateSubresource(pBuffer, UAV, 0);
    }
  }

  return SUCCEEDED(hr);
}
bool ezRHIGraphicsDeviceDX11::CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, Texture* pTexture) const
{
  auto internal_state = EZ_DEFAULT_NEW(Texture_DX11);
  pTexture->internal_state = internal_state;
  pTexture->type = GPUResource::GPU_RESOURCE_TYPE::TEXTURE;

  pTexture->desc = *pDesc;

  ezDynamicArray<D3D11_SUBRESOURCE_DATA> data;
  if (pInitialData != nullptr)
  {
    uint32_t dataCount = pDesc->ArraySize * ezMath::Max(1u, pDesc->MipLevels);
    data.SetCount(dataCount);
    for (uint32_t slice = 0; slice < dataCount; ++slice)
    {
      data[slice] = _ConvertSubresourceData(pInitialData[slice]);
    }
  }

  HRESULT hr = S_OK;

  switch (pTexture->desc.type)
  {
    case TextureDesc::TEXTURE_1D:
    {
      D3D11_TEXTURE1D_DESC desc = _ConvertTextureDesc1D(&pTexture->desc);
      hr = device->CreateTexture1D(&desc, data.GetData(), (ID3D11Texture1D**)internal_state->resource.ReleaseAndGetAddressOf());
    }
    break;
    case TextureDesc::TEXTURE_2D:
    {
      D3D11_TEXTURE2D_DESC desc = _ConvertTextureDesc2D(&pTexture->desc);
      hr = device->CreateTexture2D(&desc, data.GetData(), (ID3D11Texture2D**)internal_state->resource.ReleaseAndGetAddressOf());
    }
    break;
    case TextureDesc::TEXTURE_3D:
    {
      D3D11_TEXTURE3D_DESC desc = _ConvertTextureDesc3D(&pTexture->desc);
      hr = device->CreateTexture3D(&desc, data.GetData(), (ID3D11Texture3D**)internal_state->resource.ReleaseAndGetAddressOf());
    }
    break;
    default:
      EZ_ASSERT_ALWAYS(false, "");
      break;
  }

  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");
  if (FAILED(hr))
    return SUCCEEDED(hr);

  if (pTexture->desc.MipLevels == 0)
  {
    pTexture->desc.MipLevels = (uint32_t)log2(ezMath::Max(pTexture->desc.Width, pTexture->desc.Height)) + 1;
  }

  if (pTexture->desc.BindFlags & BIND_RENDER_TARGET)
  {
    CreateSubresource(pTexture, RTV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_DEPTH_STENCIL)
  {
    CreateSubresource(pTexture, DSV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_SHADER_RESOURCE)
  {
    CreateSubresource(pTexture, SRV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_UNORDERED_ACCESS)
  {
    CreateSubresource(pTexture, UAV, 0, -1, 0, -1);
  }

  return SUCCEEDED(hr);
}
bool ezRHIGraphicsDeviceDX11::CreateShader(SHADERSTAGE stage, const void* pShaderBytecode, ezUInt64 BytecodeLength, Shader* pShader) const
{
  pShader->stage = stage;

  HRESULT hr = E_FAIL;

  switch (stage)
  {
    case SHADERSTAGE::VS:
    {
      auto internal_state = EZ_DEFAULT_NEW(VertexShader_DX11);
      pShader->internal_state = internal_state;
      internal_state->shadercode.SetCount((ezUInt32)BytecodeLength);// todo: 64bits
      std::memcpy(internal_state->shadercode.GetData(), pShaderBytecode, BytecodeLength);
      hr = device->CreateVertexShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
    case SHADERSTAGE::HS:
    {
      auto internal_state = EZ_DEFAULT_NEW(HullShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreateHullShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
    case SHADERSTAGE::DS:
    {
      auto internal_state = EZ_DEFAULT_NEW(DomainShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreateDomainShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
    case SHADERSTAGE::GS:
    {
      auto internal_state = EZ_DEFAULT_NEW(GeometryShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreateGeometryShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
    case SHADERSTAGE::PS:
    {
      auto internal_state = EZ_DEFAULT_NEW(PixelShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreatePixelShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
    case SHADERSTAGE::CS:
    {
      auto internal_state = EZ_DEFAULT_NEW(ComputeShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreateComputeShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
  }

  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");

  return SUCCEEDED(hr);
}
bool ezRHIGraphicsDeviceDX11::CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState) const
{
  auto internal_state = EZ_DEFAULT_NEW(Sampler_DX11);
  pSamplerState->internal_state = internal_state;

  D3D11_SAMPLER_DESC desc;
  desc.Filter = _ConvertFilter(pSamplerDesc->Filter);
  desc.AddressU = _ConvertTextureAddressMode(pSamplerDesc->AddressU);
  desc.AddressV = _ConvertTextureAddressMode(pSamplerDesc->AddressV);
  desc.AddressW = _ConvertTextureAddressMode(pSamplerDesc->AddressW);
  desc.MipLODBias = pSamplerDesc->MipLODBias;
  desc.MaxAnisotropy = pSamplerDesc->MaxAnisotropy;
  desc.ComparisonFunc = _ConvertComparisonFunc(pSamplerDesc->ComparisonFunc);
  desc.BorderColor[0] = pSamplerDesc->BorderColor[0];
  desc.BorderColor[1] = pSamplerDesc->BorderColor[1];
  desc.BorderColor[2] = pSamplerDesc->BorderColor[2];
  desc.BorderColor[3] = pSamplerDesc->BorderColor[3];
  desc.MinLOD = pSamplerDesc->MinLOD;
  desc.MaxLOD = pSamplerDesc->MaxLOD;

  pSamplerState->desc = *pSamplerDesc;
  HRESULT hr = device->CreateSamplerState(&desc, &internal_state->resource);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");

  return SUCCEEDED(hr);
}
bool ezRHIGraphicsDeviceDX11::CreateQueryHeap(const GPUQueryHeapDesc* pDesc, GPUQueryHeap* pQueryHeap) const
{
  auto internal_state = EZ_DEFAULT_NEW(QueryHeap_DX11);
  pQueryHeap->internal_state = internal_state;

  pQueryHeap->desc = *pDesc;

  D3D11_QUERY_DESC desc;
  desc.MiscFlags = 0;
  desc.Query = D3D11_QUERY_EVENT;
  switch (pDesc->type)
  {
    default:
    case GPU_QUERY_TYPE_TIMESTAMP:
      desc.Query = D3D11_QUERY_TIMESTAMP;
      break;
    case GPU_QUERY_TYPE_OCCLUSION:
      desc.Query = D3D11_QUERY_OCCLUSION;
      break;
    case GPU_QUERY_TYPE_OCCLUSION_BINARY:
      desc.Query = D3D11_QUERY_OCCLUSION_PREDICATE;
      break;
  }

  internal_state->resources.SetCount(pDesc->queryCount);
  for (uint32_t i = 0; i < pDesc->queryCount; ++i)
  {
    HRESULT hr = device->CreateQuery(&desc, &internal_state->resources[i]);
    if (!SUCCEEDED(hr))
    {
      return false;
    }
  }

  return true;
}
bool ezRHIGraphicsDeviceDX11::CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso) const
{
  auto internal_state = EZ_DEFAULT_NEW(PipelineState_DX11);
  pso->internal_state = internal_state;

  pso->desc = *pDesc;

  HRESULT hr;



  if (pDesc->il != nullptr)
  {
    ezDynamicArray<D3D11_INPUT_ELEMENT_DESC> desc;
    desc.SetCount(pDesc->il->elements.GetCount());
    for (ezUInt32 i = 0; i < desc.GetCount(); ++i)
    {
      desc[i].SemanticName = pDesc->il->elements[i].SemanticName.GetData();
      desc[i].SemanticIndex = pDesc->il->elements[i].SemanticIndex;
      desc[i].Format = _ConvertFormat(pDesc->il->elements[i].Format);
      desc[i].InputSlot = pDesc->il->elements[i].InputSlot;
      desc[i].AlignedByteOffset = pDesc->il->elements[i].AlignedByteOffset;
      if (desc[i].AlignedByteOffset == InputLayout::APPEND_ALIGNED_ELEMENT)
        desc[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
      desc[i].InputSlotClass = _ConvertInputClassification(pDesc->il->elements[i].InputSlotClass);
      desc[i].InstanceDataStepRate = 0;
      if (desc[i].InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA)
      {
        desc[i].InstanceDataStepRate = 1u;
      }
    }

    EZ_ASSERT_ALWAYS(pDesc->vs != nullptr, "");
    auto vs_internal = static_cast<VertexShader_DX11*>(pDesc->vs->internal_state.Borrow());
    hr = device->CreateInputLayout(desc.GetData(), (UINT)desc.GetCount(), vs_internal->shadercode.GetData(), vs_internal->shadercode.GetCount(), &internal_state->il);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");
  }



  if (pDesc->bs != nullptr)
  {
    D3D11_BLEND_DESC desc;
    desc.AlphaToCoverageEnable = pDesc->bs->AlphaToCoverageEnable;
    desc.IndependentBlendEnable = pDesc->bs->IndependentBlendEnable;
    for (int i = 0; i < 8; ++i)
    {
      desc.RenderTarget[i].BlendEnable = pDesc->bs->RenderTarget[i].BlendEnable;
      desc.RenderTarget[i].SrcBlend = _ConvertBlend(pDesc->bs->RenderTarget[i].SrcBlend);
      desc.RenderTarget[i].DestBlend = _ConvertBlend(pDesc->bs->RenderTarget[i].DestBlend);
      desc.RenderTarget[i].BlendOp = _ConvertBlendOp(pDesc->bs->RenderTarget[i].BlendOp);
      desc.RenderTarget[i].SrcBlendAlpha = _ConvertBlend(pDesc->bs->RenderTarget[i].SrcBlendAlpha);
      desc.RenderTarget[i].DestBlendAlpha = _ConvertBlend(pDesc->bs->RenderTarget[i].DestBlendAlpha);
      desc.RenderTarget[i].BlendOpAlpha = _ConvertBlendOp(pDesc->bs->RenderTarget[i].BlendOpAlpha);
      desc.RenderTarget[i].RenderTargetWriteMask = (ezUInt8)_ParseColorWriteMask(pDesc->bs->RenderTarget[i].RenderTargetWriteMask);
    }

    hr = device->CreateBlendState(&desc, &internal_state->bs);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");
  }


  if (pDesc->dss != nullptr)
  {
    D3D11_DEPTH_STENCIL_DESC desc;
    desc.DepthEnable = pDesc->dss->DepthEnable;
    desc.DepthWriteMask = _ConvertDepthWriteMask(pDesc->dss->DepthWriteMask);
    desc.DepthFunc = _ConvertComparisonFunc(pDesc->dss->DepthFunc);
    desc.StencilEnable = pDesc->dss->StencilEnable;
    desc.StencilReadMask = pDesc->dss->StencilReadMask;
    desc.StencilWriteMask = pDesc->dss->StencilWriteMask;
    desc.FrontFace.StencilDepthFailOp = _ConvertStencilOp(pDesc->dss->FrontFace.StencilDepthFailOp);
    desc.FrontFace.StencilFailOp = _ConvertStencilOp(pDesc->dss->FrontFace.StencilFailOp);
    desc.FrontFace.StencilFunc = _ConvertComparisonFunc(pDesc->dss->FrontFace.StencilFunc);
    desc.FrontFace.StencilPassOp = _ConvertStencilOp(pDesc->dss->FrontFace.StencilPassOp);
    desc.BackFace.StencilDepthFailOp = _ConvertStencilOp(pDesc->dss->BackFace.StencilDepthFailOp);
    desc.BackFace.StencilFailOp = _ConvertStencilOp(pDesc->dss->BackFace.StencilFailOp);
    desc.BackFace.StencilFunc = _ConvertComparisonFunc(pDesc->dss->BackFace.StencilFunc);
    desc.BackFace.StencilPassOp = _ConvertStencilOp(pDesc->dss->BackFace.StencilPassOp);

    hr = device->CreateDepthStencilState(&desc, &internal_state->dss);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");
  }


  if (pDesc->rs != nullptr)
  {
    D3D11_RASTERIZER_DESC desc;
    desc.FillMode = _ConvertFillMode(pDesc->rs->FillMode);
    desc.CullMode = _ConvertCullMode(pDesc->rs->CullMode);
    desc.FrontCounterClockwise = pDesc->rs->FrontCounterClockwise;
    desc.DepthBias = pDesc->rs->DepthBias;
    desc.DepthBiasClamp = pDesc->rs->DepthBiasClamp;
    desc.SlopeScaledDepthBias = pDesc->rs->SlopeScaledDepthBias;
    desc.DepthClipEnable = pDesc->rs->DepthClipEnable;
    desc.ScissorEnable = true;
    desc.MultisampleEnable = pDesc->rs->MultisampleEnable;
    desc.AntialiasedLineEnable = pDesc->rs->AntialiasedLineEnable;


    if (CheckCapability(GRAPHICSDEVICE_CAPABILITY_CONSERVATIVE_RASTERIZATION) && pDesc->rs->ConservativeRasterizationEnable == TRUE)
    {
      ComPtr<ID3D11Device3> device3;
      if (SUCCEEDED(device.As(&device3)))
      {
        D3D11_RASTERIZER_DESC2 desc2;
        desc2.FillMode = desc.FillMode;
        desc2.CullMode = desc.CullMode;
        desc2.FrontCounterClockwise = desc.FrontCounterClockwise;
        desc2.DepthBias = desc.DepthBias;
        desc2.DepthBiasClamp = desc.DepthBiasClamp;
        desc2.SlopeScaledDepthBias = desc.SlopeScaledDepthBias;
        desc2.DepthClipEnable = desc.DepthClipEnable;
        desc2.ScissorEnable = desc.ScissorEnable;
        desc2.MultisampleEnable = desc.MultisampleEnable;
        desc2.AntialiasedLineEnable = desc.AntialiasedLineEnable;
        desc2.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_ON;
        desc2.ForcedSampleCount = pDesc->rs->ForcedSampleCount;

        ComPtr<ID3D11RasterizerState2> rasterizer2;
        hr = device3->CreateRasterizerState2(&desc2, &rasterizer2);
        EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");

        internal_state->rs = rasterizer2;
      }
    }
    else if (pDesc->rs->ForcedSampleCount > 0)
    {
      ComPtr<ID3D11Device1> device1;
      if (SUCCEEDED(device.As(&device1)))
      {
        D3D11_RASTERIZER_DESC1 desc1;
        desc1.FillMode = desc.FillMode;
        desc1.CullMode = desc.CullMode;
        desc1.FrontCounterClockwise = desc.FrontCounterClockwise;
        desc1.DepthBias = desc.DepthBias;
        desc1.DepthBiasClamp = desc.DepthBiasClamp;
        desc1.SlopeScaledDepthBias = desc.SlopeScaledDepthBias;
        desc1.DepthClipEnable = desc.DepthClipEnable;
        desc1.ScissorEnable = desc.ScissorEnable;
        desc1.MultisampleEnable = desc.MultisampleEnable;
        desc1.AntialiasedLineEnable = desc.AntialiasedLineEnable;
        desc1.ForcedSampleCount = pDesc->rs->ForcedSampleCount;

        ComPtr<ID3D11RasterizerState1> rasterizer1;
        hr = device1->CreateRasterizerState1(&desc1, &rasterizer1);
        EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");

        internal_state->rs = rasterizer1;
      }
    }

    hr = device->CreateRasterizerState(&desc, &internal_state->rs);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");
  }

  return true;
}
bool ezRHIGraphicsDeviceDX11::CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass) const
{
  renderpass->internal_state = emptyresource;

  renderpass->desc = *pDesc;

  return true;
}

int ezRHIGraphicsDeviceDX11::CreateSubresource(Texture* texture, SUBRESOURCE_TYPE type, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount) const
{
  auto internal_state = to_internal(texture);

  switch (type)
  {
    case SUBRESOURCE_TYPE::SRV:
    {
      D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case FORMAT_R16_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R16_UNORM;
          break;
        case FORMAT_R32_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
          break;
        case FORMAT_R24G8_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          break;
        case FORMAT_R32G8X24_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
          break;
        default:
          srv_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
          srv_desc.Texture1DArray.FirstArraySlice = firstSlice;
          srv_desc.Texture1DArray.ArraySize = sliceCount;
          srv_desc.Texture1DArray.MostDetailedMip = firstMip;
          srv_desc.Texture1DArray.MipLevels = mipCount;
        }
        else
        {
          srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
          srv_desc.Texture1D.MostDetailedMip = firstMip;
          srv_desc.Texture1D.MipLevels = mipCount;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          if (texture->desc.MiscFlags & RESOURCE_MISC_TEXTURECUBE)
          {
            if (texture->desc.ArraySize > 6)
            {
              srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
              srv_desc.TextureCubeArray.First2DArrayFace = firstSlice;
              srv_desc.TextureCubeArray.NumCubes = ezMath::Min(texture->desc.ArraySize, sliceCount) / 6;
              srv_desc.TextureCubeArray.MostDetailedMip = firstMip;
              srv_desc.TextureCubeArray.MipLevels = mipCount;
            }
            else
            {
              srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
              srv_desc.TextureCube.MostDetailedMip = firstMip;
              srv_desc.TextureCube.MipLevels = mipCount;
            }
          }
          else
          {
            if (texture->desc.SampleCount > 1)
            {
              srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
              srv_desc.Texture2DMSArray.FirstArraySlice = firstSlice;
              srv_desc.Texture2DMSArray.ArraySize = sliceCount;
            }
            else
            {
              srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
              srv_desc.Texture2DArray.FirstArraySlice = firstSlice;
              srv_desc.Texture2DArray.ArraySize = sliceCount;
              srv_desc.Texture2DArray.MostDetailedMip = firstMip;
              srv_desc.Texture2DArray.MipLevels = mipCount;
            }
          }
        }
        else
        {
          if (texture->desc.SampleCount > 1)
          {
            srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
          }
          else
          {
            srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srv_desc.Texture2D.MostDetailedMip = firstMip;
            srv_desc.Texture2D.MipLevels = mipCount;
          }
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_3D)
      {
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        srv_desc.Texture3D.MostDetailedMip = firstMip;
        srv_desc.Texture3D.MipLevels = mipCount;
      }

      ComPtr<ID3D11ShaderResourceView> srv;
      HRESULT hr = device->CreateShaderResourceView(internal_state->resource.Get(), &srv_desc, &srv);
      if (SUCCEEDED(hr))
      {
        if (!internal_state->srv)
        {
          internal_state->srv = srv;
          return -1;
        }
        internal_state->subresources_srv.PushBack(srv);
        return int(internal_state->subresources_srv.GetCount() - 1);
      }
      else
      {
        EZ_ASSERT_ALWAYS(false, "");
      }
    }
    break;
    case SUBRESOURCE_TYPE::UAV:
    {
      D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case FORMAT_R16_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R16_UNORM;
          break;
        case FORMAT_R32_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R32_FLOAT;
          break;
        case FORMAT_R24G8_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          break;
        case FORMAT_R32G8X24_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
          break;
        default:
          uav_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
          uav_desc.Texture1DArray.FirstArraySlice = firstSlice;
          uav_desc.Texture1DArray.ArraySize = sliceCount;
          uav_desc.Texture1DArray.MipSlice = firstMip;
        }
        else
        {
          uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
          uav_desc.Texture1D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
          uav_desc.Texture2DArray.FirstArraySlice = firstSlice;
          uav_desc.Texture2DArray.ArraySize = sliceCount;
          uav_desc.Texture2DArray.MipSlice = firstMip;
        }
        else
        {
          uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
          uav_desc.Texture2D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_3D)
      {
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
        uav_desc.Texture3D.MipSlice = firstMip;
        uav_desc.Texture3D.FirstWSlice = 0;
        uav_desc.Texture3D.WSize = -1;
      }

      ComPtr<ID3D11UnorderedAccessView> uav;
      HRESULT hr = device->CreateUnorderedAccessView(internal_state->resource.Get(), &uav_desc, &uav);
      if (SUCCEEDED(hr))
      {
        if (!internal_state->uav)
        {
          internal_state->uav = uav;
          return -1;
        }
        internal_state->subresources_uav.PushBack(uav);
        return int(internal_state->subresources_uav.GetCount() - 1);
      }
      else
      {
        EZ_ASSERT_ALWAYS(false, "");
      }
    }
    break;
    case SUBRESOURCE_TYPE::RTV:
    {
      D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case FORMAT_R16_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R16_UNORM;
          break;
        case FORMAT_R32_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R32_FLOAT;
          break;
        case FORMAT_R24G8_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          break;
        case FORMAT_R32G8X24_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
          break;
        default:
          rtv_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
          rtv_desc.Texture1DArray.FirstArraySlice = firstSlice;
          rtv_desc.Texture1DArray.ArraySize = sliceCount;
          rtv_desc.Texture1DArray.MipSlice = firstMip;
        }
        else
        {
          rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
          rtv_desc.Texture1D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          if (texture->desc.SampleCount > 1)
          {
            rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
            rtv_desc.Texture2DMSArray.FirstArraySlice = firstSlice;
            rtv_desc.Texture2DMSArray.ArraySize = sliceCount;
          }
          else
          {
            rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtv_desc.Texture2DArray.FirstArraySlice = firstSlice;
            rtv_desc.Texture2DArray.ArraySize = sliceCount;
            rtv_desc.Texture2DArray.MipSlice = firstMip;
          }
        }
        else
        {
          if (texture->desc.SampleCount > 1)
          {
            rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
          }
          else
          {
            rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            rtv_desc.Texture2D.MipSlice = firstMip;
          }
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_3D)
      {
        rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
        rtv_desc.Texture3D.MipSlice = firstMip;
        rtv_desc.Texture3D.FirstWSlice = 0;
        rtv_desc.Texture3D.WSize = -1;
      }

      ComPtr<ID3D11RenderTargetView> rtv;
      HRESULT hr = device->CreateRenderTargetView(internal_state->resource.Get(), &rtv_desc, &rtv);
      if (SUCCEEDED(hr))
      {
        if (!internal_state->rtv)
        {
          internal_state->rtv = rtv;
          return -1;
        }
        internal_state->subresources_rtv.PushBack(rtv);
        return int(internal_state->subresources_rtv.GetCount() - 1);
      }
      else
      {
        EZ_ASSERT_ALWAYS(false, "");
      }
    }
    break;
    case SUBRESOURCE_TYPE::DSV:
    {
      D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case FORMAT_R16_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D16_UNORM;
          break;
        case FORMAT_R32_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
          break;
        case FORMAT_R24G8_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
          break;
        case FORMAT_R32G8X24_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
          break;
        default:
          dsv_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
          dsv_desc.Texture1DArray.FirstArraySlice = firstSlice;
          dsv_desc.Texture1DArray.ArraySize = sliceCount;
          dsv_desc.Texture1DArray.MipSlice = firstMip;
        }
        else
        {
          dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
          dsv_desc.Texture1D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          if (texture->desc.SampleCount > 1)
          {
            dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
            dsv_desc.Texture2DMSArray.FirstArraySlice = firstSlice;
            dsv_desc.Texture2DMSArray.ArraySize = sliceCount;
          }
          else
          {
            dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            dsv_desc.Texture2DArray.FirstArraySlice = firstSlice;
            dsv_desc.Texture2DArray.ArraySize = sliceCount;
            dsv_desc.Texture2DArray.MipSlice = firstMip;
          }
        }
        else
        {
          if (texture->desc.SampleCount > 1)
          {
            dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
          }
          else
          {
            dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            dsv_desc.Texture2D.MipSlice = firstMip;
          }
        }
      }

      ComPtr<ID3D11DepthStencilView> dsv;
      HRESULT hr = device->CreateDepthStencilView(internal_state->resource.Get(), &dsv_desc, &dsv);
      if (SUCCEEDED(hr))
      {
        if (!internal_state->dsv)
        {
          internal_state->dsv = dsv;
          return -1;
        }
        internal_state->subresources_dsv.PushBack(dsv);
        return int(internal_state->subresources_dsv.GetCount() - 1);
      }
      else
      {
        EZ_ASSERT_ALWAYS(false, "");
      }
    }
    break;
    default:
      break;
  }
  return -1;
}
int ezRHIGraphicsDeviceDX11::CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, uint64_t offset, uint64_t size) const
{
  auto internal_state = to_internal(buffer);
  const GPUBufferDesc& desc = buffer->GetDesc();
  HRESULT hr = E_FAIL;

  switch (type)
  {
    case SUBRESOURCE_TYPE::SRV:
    {
      D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

      if (desc.MiscFlags & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
      {
        // This is a Raw Buffer
        srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        srv_desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
        srv_desc.BufferEx.FirstElement = (UINT)offset / sizeof(uint32_t);
        srv_desc.BufferEx.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / sizeof(uint32_t);
      }
      else if (desc.MiscFlags & RESOURCE_MISC_BUFFER_STRUCTURED)
      {
        // This is a Structured Buffer
        srv_desc.Format = DXGI_FORMAT_UNKNOWN;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        srv_desc.BufferEx.FirstElement = (UINT)offset / desc.StructureByteStride;
        srv_desc.BufferEx.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / desc.StructureByteStride;
      }
      else
      {
        // This is a Typed Buffer
        uint32_t stride = GetFormatStride(desc.Format);
        srv_desc.Format = _ConvertFormat(desc.Format);
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srv_desc.Buffer.FirstElement = (UINT)offset / stride;
        srv_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / stride;
      }

      ComPtr<ID3D11ShaderResourceView> srv;
      hr = device->CreateShaderResourceView(internal_state->resource.Get(), &srv_desc, &srv);

      if (SUCCEEDED(hr))
      {
        if (internal_state->srv == nullptr)
        {
          internal_state->srv = srv;
          return -1;
        }
        else
        {
          internal_state->subresources_srv.PushBack(srv);
          return int(internal_state->subresources_srv.GetCount() - 1);
        }
      }
      else
      {
        EZ_ASSERT_ALWAYS(false, "");
      }
    }
    break;
    case SUBRESOURCE_TYPE::UAV:
    {
      D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
      uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

      if (desc.MiscFlags & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
      {
        // This is a Raw Buffer
        uav_desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
        uav_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        uav_desc.Buffer.FirstElement = (UINT)offset / sizeof(uint32_t);
        uav_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / sizeof(uint32_t);
      }
      else if (desc.MiscFlags & RESOURCE_MISC_BUFFER_STRUCTURED)
      {
        // This is a Structured Buffer
        uav_desc.Format = DXGI_FORMAT_UNKNOWN; // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
        uav_desc.Buffer.FirstElement = (UINT)offset / desc.StructureByteStride;
        uav_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / desc.StructureByteStride;
      }
      else
      {
        // This is a Typed Buffer
        uint32_t stride = GetFormatStride(desc.Format);
        uav_desc.Format = _ConvertFormat(desc.Format);
        uav_desc.Buffer.FirstElement = (UINT)offset / stride;
        uav_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / stride;
      }

      ComPtr<ID3D11UnorderedAccessView> uav;
      hr = device->CreateUnorderedAccessView(internal_state->resource.Get(), &uav_desc, &uav);

      if (SUCCEEDED(hr))
      {
        if (internal_state->uav == nullptr)
        {
          internal_state->uav = uav;
          return -1;
        }
        else
        {
          internal_state->subresources_uav.PushBack(uav);
          return int(internal_state->subresources_uav.GetCount() - 1);
        }
      }
      else
      {
        EZ_ASSERT_ALWAYS(false, "");
      }
    }
    break;
    default:
      EZ_ASSERT_ALWAYS(false, "");
      break;
  }
  return -1;
}

void ezRHIGraphicsDeviceDX11::Map(const GPUResource* resource, Mapping* mapping) const
{
  auto internal_state = to_internal(resource);

  D3D11_MAPPED_SUBRESOURCE map_result = {};
  D3D11_MAP map_type = D3D11_MAP_READ_WRITE;
  if (mapping->_flags & Mapping::FLAG_READ)
  {
    if (mapping->_flags & Mapping::FLAG_WRITE)
    {
      map_type = D3D11_MAP_READ_WRITE;
    }
    else
    {
      map_type = D3D11_MAP_READ;
    }
  }
  else if (mapping->_flags & Mapping::FLAG_WRITE)
  {
    map_type = D3D11_MAP_WRITE_NO_OVERWRITE;
  }
  HRESULT hr = immediateContext->Map(internal_state->resource.Get(), 0, map_type, D3D11_MAP_FLAG_DO_NOT_WAIT, &map_result);
  if (SUCCEEDED(hr))
  {
    mapping->data = map_result.pData;
    mapping->rowpitch = map_result.RowPitch;
  }
  else
  {
    EZ_ASSERT_ALWAYS(false, "");
    mapping->data = nullptr;
    mapping->rowpitch = 0;
  }
}
void ezRHIGraphicsDeviceDX11::Unmap(const GPUResource* resource) const
{
  auto internal_state = to_internal(resource);
  immediateContext->Unmap(internal_state->resource.Get(), 0);
}
void ezRHIGraphicsDeviceDX11::QueryRead(const GPUQueryHeap* heap, uint32_t index, uint32_t count, uint64_t* results) const
{
  if (count == 0)
    return;

  auto internal_state = to_internal(heap);

  const uint32_t _flags = D3D11_ASYNC_GETDATA_DONOTFLUSH;

  HRESULT hr = S_OK;

  EZ_ASSERT_ALWAYS(index + count <= internal_state->resources.GetCount(), "");
  for (uint32_t i = 0; i < count; ++i)
  {
    ID3D11Query* QUERY = internal_state->resources[index + i].Get();

    switch (heap->desc.type)
    {
      case GPU_QUERY_TYPE_TIMESTAMP:
        hr = immediateContext->GetData(QUERY, &results[i], sizeof(uint64_t), _flags);
        break;
      case GPU_QUERY_TYPE_OCCLUSION:
        hr = immediateContext->GetData(QUERY, &results[i], sizeof(uint64_t), _flags);
        break;
      case GPU_QUERY_TYPE_OCCLUSION_BINARY:
      {
        BOOL passed = FALSE;
        hr = immediateContext->GetData(QUERY, &passed, sizeof(BOOL), _flags);
        results[i] = (uint64_t)passed;
        break;
      }
    }
  }
}

void ezRHIGraphicsDeviceDX11::SetCommonSampler(const StaticSampler* sam)
{
  common_samplers.PushBack(*sam);
}

void ezRHIGraphicsDeviceDX11::SetName(GPUResource* pResource, const char* name)
{
  auto internal_state = to_internal(pResource);
  internal_state->resource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
}

ezRHICommandList* ezRHIGraphicsDeviceDX11::BeginCommandList(QUEUE_TYPE queue)
{
  ezRHICommandListDX11* commandList = EZ_DEFAULT_NEW(ezRHICommandListDX11);
  commandList->Initialize(this);

  m_CommandLists.PushBack(commandList);

  return commandList;
}

void ezRHIGraphicsDeviceDX11::SubmitCommandLists()
{
  const int disjoint_write = FRAMECOUNT % EZ_ARRAY_SIZE(disjointQueries);
  const int disjoint_read = (FRAMECOUNT + 1) % EZ_ARRAY_SIZE(disjointQueries);
  immediateContext->Begin(disjointQueries[disjoint_write].Get());

  // Execute deferred command lists:

  for (const auto& commandList : m_CommandLists)
  {
    commandList->Submit(this);
  }

  immediateContext->ClearState();

  immediateContext->End(disjointQueries[disjoint_write].Get());
  D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint;
  HRESULT hr = immediateContext->GetData(
    disjointQueries[disjoint_read].Get(),
    &disjoint,
    sizeof(disjoint),
    D3D11_ASYNC_GETDATA_DONOTFLUSH);
  if (disjoint.Disjoint == FALSE && hr == S_OK)
  {
    TIMESTAMP_FREQUENCY = disjoint.Frequency;
  }

  FRAMECOUNT++;

  
  for (auto& commandList : m_CommandLists)
  {
    commandList->Destroy(this);
    EZ_DEFAULT_DELETE(commandList);
  }
  m_CommandLists.Clear();
}

void ezRHIGraphicsDeviceDX11::WaitForGPU() const
{
  immediateContext->Flush();


  D3D11_QUERY_DESC desc;
  desc.MiscFlags = 0;
  desc.Query = D3D11_QUERY_EVENT;

  ComPtr<ID3D11Query> query;
  HRESULT hr = device->CreateQuery(&desc, &query);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");
  immediateContext->End(query.Get());
  BOOL result;
  while (immediateContext->GetData(query.Get(), &result, sizeof(result), 0) == S_FALSE)
    ;
  EZ_ASSERT_ALWAYS(result == TRUE, "");
}


Texture ezRHIGraphicsDeviceDX11::GetBackBuffer(const SwapChain* swapchain) const
{
  auto swapchain_internal = to_internal(swapchain);

  auto internal_state = EZ_DEFAULT_NEW(Texture_DX11);
  internal_state->resource = swapchain_internal->backBuffer;

  Texture result;
  result.internal_state = internal_state;
  result.type = GPUResource::GPU_RESOURCE_TYPE::TEXTURE;

  D3D11_TEXTURE2D_DESC desc;
  swapchain_internal->backBuffer->GetDesc(&desc);
  result.desc = _ConvertTextureDesc_Inv(&desc);

  return result;
}
