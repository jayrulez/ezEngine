
#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Resources/Resource.h>

class EZ_RHI_DLL ezRHIRenderTargetView : public ezRHIObject<ezRHIRenderTargetViewCreationDescription>
{
public:
  EZ_ALWAYS_INLINE ezRHITexture* GetTexture() const { return m_pTexture; }

protected:
  friend class ezRHIRenderDeviceImpl;

  ezRHIRenderTargetView(ezRHITexture* pTexture, const ezRHIRenderTargetViewCreationDescription& description);

  virtual ~ezRHIRenderTargetView();

  virtual ezResult InitPlatform(ezRHIRenderDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIRenderDevice* pDevice) = 0;

  ezRHITexture* m_pTexture;
};
