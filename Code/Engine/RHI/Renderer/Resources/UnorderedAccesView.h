
#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Resources/Resource.h>

class EZ_RHI_DLL ezRHIUnorderedAccessView : public ezRHIObject<ezRHIUnorderedAccessViewCreationDescription>
{
public:
  EZ_ALWAYS_INLINE ezRHIResourceBase* GetResource() const { return m_pResource; }

protected:
  friend class ezRHIRenderDeviceImpl;

  ezRHIUnorderedAccessView(ezRHIResourceBase* pResource, const ezRHIUnorderedAccessViewCreationDescription& description);

  virtual ~ezRHIUnorderedAccessView();

  virtual ezResult InitPlatform(ezRHIRenderDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIRenderDevice* pDevice) = 0;

  ezRHIResourceBase* m_pResource;
};
