#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Resources/Resource.h>

class EZ_RHI_DLL ezRHIQuery : public ezRHIResource<ezRHIQueryCreationDescription>
{
public:
protected:
  friend class ezRHIRenderDeviceImpl;
  friend class ezRHICommandEncoder;

  ezRHIQuery(const ezRHIQueryCreationDescription& Description);

  virtual ~ezRHIQuery();

  virtual ezResult InitPlatform(ezRHIRenderDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIRenderDevice* pDevice) = 0;

  bool m_bStarted;
};
