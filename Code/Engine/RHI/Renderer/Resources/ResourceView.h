
#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Resources/Resource.h>
#include <RHI/View/View.h>

class EZ_RHI_DLL ezRHIResourceView : public ezRHIObject<ezRHIResourceViewCreationDescription>
{
public:
  EZ_ALWAYS_INLINE ezRHIResourceBase* GetResource() const { return m_pResource; }

protected:
  friend class ezRHIRenderDeviceImpl;
  friend class ezMemoryUtils;

  EZ_ALWAYS_INLINE std::shared_ptr<View> GetBackendResourceView() const
  {
    return m_pView;
  }

  ezRHIResourceView(ezRHIResourceBase* pResource, const ezRHIResourceViewCreationDescription& description);

  virtual ~ezRHIResourceView();

  ezResult InitPlatform(ezRHIRenderDevice* pDevice);

  ezResult DeInitPlatform(ezRHIRenderDevice* pDevice);

  ezRHIResourceBase* m_pResource;

  std::shared_ptr<View> m_pView;
};
