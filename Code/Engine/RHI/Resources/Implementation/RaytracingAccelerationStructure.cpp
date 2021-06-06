#include <RHI/Resources/RaytracingAccelerationStructure.h>

ezRHIRaytracingAccelerationStructure::ezRHIRaytracingAccelerationStructure(const ezRHIRaytracingAccelerationStructureDesc& desc)
  : ezRHIResource(desc, ezRHIGPUResourceType::RayTracingAccelerationStructure)
{
}

ezRHIRaytracingAccelerationStructure::~ezRHIRaytracingAccelerationStructure()
{
}
