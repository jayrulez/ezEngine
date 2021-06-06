#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Resources/Resource.h>

class EZ_RHI_DLL ezRHIRaytracingAccelerationStructure : public ezRHIResource<ezRHIRaytracingAccelerationStructureDesc>
{
public:
protected:
  ezRHIRaytracingAccelerationStructure(const ezRHIRaytracingAccelerationStructureDesc& desc);

  virtual ~ezRHIRaytracingAccelerationStructure();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;
  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};
