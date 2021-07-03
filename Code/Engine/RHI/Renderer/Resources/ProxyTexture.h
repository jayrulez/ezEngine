
#pragma once

#include <RHI/Renderer/Resources/Texture.h>

class EZ_RHI_DLL ezRHIProxyTexture : public ezRHITexture
{
public:
  virtual ~ezRHIProxyTexture();

  virtual const ezRHIResourceBase* GetParentResource() const override;

protected:
  friend class ezRHIRenderDeviceImpl;

  ezRHIProxyTexture(const ezRHITexture& parentTexture);

  virtual ezResult InitPlatform(ezRHIRenderDevice* pDevice, ezArrayPtr<ezRHISystemMemoryDescription> pInitialData) override;
  virtual ezResult DeInitPlatform(ezRHIRenderDevice* pDevice) override;
  virtual ezResult ReplaceExisitingNativeObject(void* pExisitingNativeObject) override;

  virtual void SetDebugNamePlatform(const char* szName) const override;

  const ezRHITexture* m_pParentTexture;
};
