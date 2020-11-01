#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Descriptors/TextureDescription.h>
#include <RHI/Descriptors/TextureViewDescription.h>

#include <RHI/Resources/Texture.h>
#include <dxgi.h>
#include <dxgiformat.h>

struct ID3D11Device;
struct ID3D11Resource;
struct ID3D11Texture2D;

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
  virtual ezString GetName() const override;

  virtual void SetName(const ezString& name) override;

  virtual bool IsDisposed() const override;
  virtual void Dispose() override;

  virtual ezEnum<RHIPixelFormat> GetFormat() const override;

  virtual ezUInt32 GetWidth() const override;

  virtual ezUInt32 GetHeight() const override;

  virtual ezUInt32 GetDepth() const override;

  virtual ezUInt32 GetMipLevels() const override;

  virtual ezUInt32 GetArrayLayers() const override;

  virtual ezBitflags<RHITextureUsage> GetUsage() const override;

  virtual ezEnum<RHITextureType> GetType() const override;

  virtual ezEnum<RHITextureSampleCount> GetSampleCount() const override;

public:
  ID3D11Resource* GetDeviceTexture();
  DXGI_FORMAT GetDxgiFormat() const;
  DXGI_FORMAT GetTypelessDxgiFormatt() const;

  D3D11Texture(ID3D11Device* device, const RHITextureDescription& description);

  D3D11Texture(ID3D11Texture2D* existingTexture, ezEnum<RHITextureType> type, ezEnum<RHIPixelFormat> format);

protected:
  virtual RHITextureView* CreateFullTextureView(RHIGraphicsDevice* graphicsDevice) override;

private:
  void DisposeCore() override;
};
