#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Resources/Resource.h>

class EZ_RHI_DLL ezRHIQuery : public ezRHIResource<ezRHIQueryCreationDescription>
{
public:
protected:
  friend class ezRHIDevice;
  friend class ezRHICommandEncoder;

  ezRHIQuery(const ezRHIQueryCreationDescription& Description);

  virtual ~ezRHIQuery();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;

  bool m_bStarted;
};
