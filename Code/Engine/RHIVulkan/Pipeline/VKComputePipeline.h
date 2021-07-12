#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <RHIVulkan/Pipeline/VKPipeline.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>

class VKDevice;

class VKComputePipeline : public VKPipeline
{
public:
  VKComputePipeline(VKDevice& device, const ComputePipelineDesc& desc);
  PipelineType GetPipelineType() const override;

private:
  ComputePipelineDesc m_desc;
};
