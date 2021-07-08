#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHIVulkan/Pipeline/VKPipeline.h>


class ezRHIVKComputePipeline : public ezRHIVKPipeline
{
public:
  ezRHIVKComputePipeline(ezRHIVKDevice& device, const ezRHIComputePipelineCreationDescription& desc);
  ezRHIPipelineType GetPipelineType() const override;

private:
  ezRHIComputePipelineCreationDescription m_Desc;
};
