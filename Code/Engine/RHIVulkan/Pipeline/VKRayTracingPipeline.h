#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHIVulkan/Pipeline/VKPipeline.h>

class ezRHIVKRayTracingPipeline : public ezRHIVKPipeline
{
public:
  ezRHIVKRayTracingPipeline(ezRHIVKDevice& device, const ezRHIRayTracingPipelineCreationDescription& desc);
  ezRHIPipelineType GetPipelineType() const override;
  ezDynamicArray<ezUInt8> GetRayTracingShaderGroupHandles(ezUInt32 firstGroup, ezUInt32 groupCount) const override;

private:
  ezRHIRayTracingPipelineCreationDescription m_Desc;
};
