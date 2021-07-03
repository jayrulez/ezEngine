
#pragma once

#include <RHI/Renderer/Resources/Texture.h>

class EZ_RHI_DLL ezRHIProxyTexture : public ezRHITexture
{
public:
  virtual ~ezRHIProxyTexture();

  virtual const ezRHIResourceBase* GetParentResource() const override;

protected:
  friend class ezRHIDevice;

  ezRHIProxyTexture(const ezRHITexture& parentTexture);

  virtual ezResult InitPlatform(ezRHIDevice* pDevice, ezArrayPtr<ezRHISystemMemoryDescription> pInitialData) override;
  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) override;
  virtual ezResult ReplaceExisitingNativeObject(void* pExisitingNativeObject) override;

  virtual void SetDebugNamePlatform(const char* szName) const override;

  const ezRHITexture* m_pParentTexture;
};
