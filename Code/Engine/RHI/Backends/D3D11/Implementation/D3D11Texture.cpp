#include <RHI/Backends/D3D11/D3D11Texture.h>
#include <RHI/Backends/D3D11/D3D11TextureView.h>
#include <d3d11_1.h>

ezString D3D11Texture::GetName() const
{
  return Name;
}

void D3D11Texture::SetName(const ezString& name)
{
  Name = name;
  if (DeviceTexture != nullptr)
    DeviceTexture->SetPrivateData(WKPDID_D3DDebugObjectName, name.GetCharacterCount(), name.GetData());
}

bool D3D11Texture::IsDisposed() const
{
  return DeviceTexture == nullptr;
}

ezEnum<RHIPixelFormat> D3D11Texture::GetFormat() const
{
  return Format;
}

ezUInt32 D3D11Texture::GetWidth() const
{
  return Width;
}

ezUInt32 D3D11Texture::GetHeight() const
{
  return Height;
}

ezUInt32 D3D11Texture::GetDepth() const
{
  return Depth;
}

ezUInt32 D3D11Texture::GetMipLevels() const
{
  return MipLevels;
}

ezUInt32 D3D11Texture::GetArrayLayers() const
{
  return ArrayLayers;
}

ezBitflags<RHITextureUsage> D3D11Texture::GetUsage() const
{
  return TextureUsage;
}

ezEnum<RHITextureType> D3D11Texture::GetType() const
{
  return TextureType;
}

ezEnum<RHITextureSampleCount> D3D11Texture::GetSampleCount() const
{
  return SampleCount;
}

ID3D11Resource* D3D11Texture::GetDeviceTexture()
{
  return DeviceTexture;
}

DXGI_FORMAT D3D11Texture::GetDxgiFormat() const
{
  return DxgiFormat;
}

DXGI_FORMAT D3D11Texture::GetTypelessDxgiFormatt() const
{
  return TypelessDxgiFormat;
}

D3D11Texture::D3D11Texture(ID3D11Device* device, const RHITextureDescription& description)
{
  Device = device;

  Width = description.Width;
  Height = description.Height;
  Depth = description.Depth;
  MipLevels = description.MipLevels;
  ArrayLayers = description.ArrayLayers;
  Format = description.Format;
  TextureUsage = description.Usage;
  TextureType = description.Type;
  SampleCount = description.SampleCount;

  DxgiFormat = D3D11Formats::ToDxgiFormat(description.Format, (description.Usage & RHITextureUsage::DepthStencil) == RHITextureUsage::DepthStencil);
  TypelessDxgiFormat = D3D11Formats::GetTypelessFormat(DxgiFormat);

  ezUInt32 cpuFlags = 0;      //D3D11_CPU_ACCESS_FLAG
  ezUInt32 resourceUsage = 0; //D3D11_USAGE
  ezUInt32 bindFlags = 0;     //D3D11_BIND_FLAG
  ezUInt32 optionFlags = 0;   //D3D11_RESOURCE_MISC_FLAG

  if ((description.Usage & RHITextureUsage::RenderTarget) == RHITextureUsage::RenderTarget)
  {
    bindFlags |= D3D11_BIND_RENDER_TARGET;
  }
  if ((description.Usage & RHITextureUsage::DepthStencil) == RHITextureUsage::DepthStencil)
  {
    bindFlags |= D3D11_BIND_DEPTH_STENCIL;
  }
  if ((description.Usage & RHITextureUsage::Sampled) == RHITextureUsage::Sampled)
  {
    bindFlags |= D3D11_BIND_SHADER_RESOURCE;
  }
  if ((description.Usage & RHITextureUsage::Storage) == RHITextureUsage::Storage)
  {
    bindFlags |= D3D11_BIND_UNORDERED_ACCESS;
  }
  if ((description.Usage & RHITextureUsage::Staging) == RHITextureUsage::Staging)
  {
    cpuFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    resourceUsage = D3D11_USAGE_STAGING;
  }

  if ((description.Usage & RHITextureUsage::GenerateMipmaps) != 0)
  {
    bindFlags |= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    optionFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
  }

  ezUInt32 arraySize = description.ArrayLayers;
  if ((description.Usage & RHITextureUsage::Cubemap) == RHITextureUsage::Cubemap)
  {
    optionFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    arraySize *= 6;
  }

  ezUInt32 roundedWidth = description.Width;
  ezUInt32 roundedHeight = description.Height;
  if (FormatHelpers::IsCompressedFormat(description.Format))
  {
    roundedWidth = ((roundedWidth + 3) / 4) * 4;
    roundedHeight = ((roundedHeight + 3) / 4) * 4;
  }

  if (TextureType == RHITextureType::Texture1D)
  {
    D3D11_TEXTURE1D_DESC desc1D;

    desc1D.Width = roundedWidth;
    desc1D.MipLevels = description.MipLevels;
    desc1D.ArraySize = arraySize;
    desc1D.Format = TypelessDxgiFormat;
    desc1D.BindFlags = bindFlags;
    desc1D.CPUAccessFlags = cpuFlags;
    desc1D.Usage = (D3D11_USAGE)resourceUsage;
    desc1D.MiscFlags = optionFlags;

    HRESULT hr = device->CreateTexture1D(&desc1D, nullptr, reinterpret_cast<ID3D11Texture1D**>(&DeviceTexture));
  }
  else if (TextureType == RHITextureType::Texture2D)
  {
    D3D11_TEXTURE2D_DESC desc2D;

    desc2D.Width = roundedWidth;
    desc2D.Height = roundedHeight;
    desc2D.MipLevels = description.MipLevels;
    desc2D.ArraySize = arraySize;
    desc2D.Format = TypelessDxgiFormat;
    desc2D.BindFlags = bindFlags;
    desc2D.CPUAccessFlags = cpuFlags;
    desc2D.Usage = (D3D11_USAGE)resourceUsage;

    desc2D.SampleDesc.Count = FormatHelpers::GetSampleCount(SampleCount);
    desc2D.SampleDesc.Quality = 0;

    desc2D.MiscFlags = optionFlags;

    HRESULT hr = device->CreateTexture2D(&desc2D, nullptr, reinterpret_cast<ID3D11Texture2D**>(&DeviceTexture));
  }
  else
  {
    EZ_ASSERT_ALWAYS(TextureType == RHITextureType::Texture3D, "TextureType must be RHITextureType::Texture3D");

    D3D11_TEXTURE3D_DESC desc3D;

    desc3D.Width = roundedWidth;
    desc3D.Height = roundedHeight;
    desc3D.Depth = description.Depth;
    desc3D.MipLevels = description.MipLevels;
    desc3D.Format = TypelessDxgiFormat;
    desc3D.BindFlags = bindFlags;
    desc3D.CPUAccessFlags = cpuFlags;
    desc3D.Usage = (D3D11_USAGE)resourceUsage;
    desc3D.MiscFlags = optionFlags;

    HRESULT hr = device->CreateTexture3D(&desc3D, nullptr, reinterpret_cast<ID3D11Texture3D**>(&DeviceTexture));
  }
}

D3D11Texture::D3D11Texture(ID3D11Texture2D* existingTexture, ezEnum<RHITextureType> type, ezEnum<RHIPixelFormat> format)
{
  existingTexture->GetDevice(&Device);

  D3D11_TEXTURE2D_DESC desc;
  existingTexture->GetDesc(&desc);

  DeviceTexture = existingTexture;

  Width = desc.Width;
  Height = desc.Height;
  Depth = 1;
  MipLevels = desc.MipLevels;
  ArrayLayers = desc.ArraySize;
  Format = format;
  SampleCount = FormatHelpers::GetSampleCount(desc.SampleDesc.Count);
  TextureType = type;

  TextureUsage = D3D11Formats::GetRHIUsage((D3D11_BIND_FLAG)desc.BindFlags, (D3D11_CPU_ACCESS_FLAG)desc.CPUAccessFlags, (D3D11_RESOURCE_MISC_FLAG)desc.MiscFlags);

  DxgiFormat = D3D11Formats::ToDxgiFormat(format, (TextureUsage & RHITextureUsage::DepthStencil) == RHITextureUsage::DepthStencil);
  TypelessDxgiFormat = D3D11Formats::GetTypelessFormat(DxgiFormat);
}

RHITextureView* D3D11Texture::CreateFullTextureView(RHIGraphicsDevice* graphicsDevice)
{
  const RHITextureViewDescription& desc(this);

  D3D11GraphicsDevice* d3d11GD = Util::AssertSubtype<RHIGraphicsDevice, D3D11GraphicsDevice>(graphicsDevice);

  //return new D3D11TextureView(d3d11GD, desc);
  return EZ_DEFAULT_NEW(D3D11TextureView, d3d11GD, desc);
}

void D3D11Texture::DisposeCore()
{
  if (DeviceTexture != nullptr)
  {
    //DeviceTexture->Release();
  }
}


EZ_STATICLINK_FILE(RHI, RHI_Backends_D3D11_Implementation_D3D11Texture);

