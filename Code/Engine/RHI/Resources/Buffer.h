#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Resources/Resource.h>

class EZ_RHI_DLL ezRHIBuffer : public ezRHIResource<ezRHIGPUBufferDesc>
{
public:
protected:
  ezRHIBuffer(const ezRHIGPUBufferDesc& desc);

  virtual ~ezRHIBuffer();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;
  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};
