#pragma once
#include <RHIVulkan/Pipeline/VKPipeline.h>
#include <RHI/Instance/BaseTypes.h>
#include <vulkan/vulkan.hpp>
#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <RHI/ShaderReflection/ShaderReflection.h>

class VKDevice;

class VKComputePipeline : public VKPipeline
{
public:
    VKComputePipeline(VKDevice& device, const ComputePipelineDesc& desc);
    PipelineType GetPipelineType() const override;

private:
    ComputePipelineDesc m_desc;
};
