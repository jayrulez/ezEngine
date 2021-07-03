
#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Resources/Resource.h>

class EZ_RHI_DLL ezRHIResourceView : public ezRHIObject<ezRHIResourceViewCreationDescription>
{
public:
  EZ_ALWAYS_INLINE ezRHIResourceBase* GetResource() const { return m_pResource; }

protected:
  friend class ezRHIDevice;

  ezRHIResourceView(ezRHIResourceBase* pResource, const ezRHIResourceViewCreationDescription& description);

  virtual ~ezRHIResourceView();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;

  ezRHIResourceBase* m_pResource;
};
