#pragma once

#include <RendererFoundation/Resources/Texture.h>

class ezGALTextureVk : public ezGALTexture
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALTextureVk(const ezGALTextureCreationDescription& Description);

  ~ezGALTextureVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice, ezArrayPtr<ezGALSystemMemoryDescription> pInitialData) override;
  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
  virtual ezResult ReplaceExisitingNativeObject(void* pExisitingNativeObject) override;

  virtual void SetDebugNamePlatform(const char* szName) const override;

  ezResult CreateStagingTexture(ezGALDeviceVk* pDevice);

  void* m_pExisitingNativeObject = nullptr;
};

#include <RendererVk/Resources/Implementation/TextureVk_inl.h>
