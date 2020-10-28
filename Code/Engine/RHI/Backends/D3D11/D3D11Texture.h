#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Utils.h>

#include <RHI/Descriptors/TextureDescription.h>
#include <RHI/Descriptors/TextureViewDescription.h>

#include <RHI/Backends/D3D11/D3D11Utils.h>
#include <RHI/Resources/Texture.h>

#include <d3d11_1.h>

class D3D11GraphicsDevice;

class D3D11Texture : public RHITexture
{
private:
  ID3D11Device* Device;
  ID3D11Resource* DeviceTexture = nullptr;
  DXGI_FORMAT DxgiFormat = DXGI_FORMAT_UNKNOWN;
  DXGI_FORMAT TypelessDxgiFormat = DXGI_FORMAT_UNKNOWN;

  ezUInt32 Width = 0;
  ezUInt32 Height = 0;
  ezUInt32 Depth = 0;
  ezUInt32 MipLevels = 0;
  ezUInt32 ArrayLayers = 0;
  ezEnum<RHIPixelFormat> Format = RHIPixelFormat::Default;
  ezBitflags<RHITextureUsage> TextureUsage;
  ezEnum<RHITextureType> TextureType = RHITextureType::Default;
  ezEnum<RHITextureSampleCount> SampleCount = RHITextureSampleCount::Default;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override { return Name; }

  virtual void SetName(const ezString& name) override
  {
    Name = name;
    if (DeviceTexture != nullptr)
      DeviceTexture->SetPrivateData(WKPDID_D3DDebugObjectName, name.GetCharacterCount(), name.GetData());
  }

  virtual bool IsDisposed() const override { return DeviceTexture == nullptr; }
  virtual void Dispose() override
  {
    if (DeviceTexture != nullptr)
    {
      DeviceTexture->Release();
      DeviceTexture = nullptr;
    }
  }

  virtual ezEnum<RHIPixelFormat> GetFormat() const override { return Format; }

  virtual ezUInt32 GetWidth() const override { return Width; }

  virtual ezUInt32 GetHeight() const override { return Height; }

  virtual ezUInt32 GetDepth() const override { return Depth; }

  virtual ezUInt32 GetMipLevels() const override { return MipLevels; }

  virtual ezUInt32 GetArrayLayers() const override { return ArrayLayers; }

  virtual ezBitflags<RHITextureUsage> GetUsage() const override { return TextureUsage; }

  virtual ezEnum<RHITextureType> GetType() const override { return TextureType; }

  virtual ezEnum<RHITextureSampleCount> GetSampleCount() const override { return SampleCount; }

public:
  ID3D11Resource* GetDeviceTexture() { return DeviceTexture; }
  DXGI_FORMAT GetDxgiFormat() const { return DxgiFormat; }
  DXGI_FORMAT GetTypelessDxgiFormatt() const { return TypelessDxgiFormat; }

  D3D11Texture(ID3D11Device* device, const RHITextureDescription& description);

  D3D11Texture(ID3D11Texture2D* existingTexture, ezEnum<RHITextureType> type, ezEnum<RHIPixelFormat> format);

  RHITextureView* CreateFullTextureView(RHIGraphicsDevice* graphicsDevice);
};
