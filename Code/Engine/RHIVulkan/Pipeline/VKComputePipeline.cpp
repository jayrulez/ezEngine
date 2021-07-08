#include <RHIVulkanPCH.h>

#include <RHIVulkan/Pipeline/VKComputePipeline.h>
#include <RHIVulkan/Device/VKDevice.h>

ezRHIVKComputePipeline::ezRHIVKComputePipeline(ezRHIVKDevice& device, const ezRHIComputePipelineCreationDescription& desc)
  : ezRHIVKPipeline{device, desc.m_Program, desc.m_Layout}
  , m_Desc{desc}
{
  vk::ComputePipelineCreateInfo pipelineInfo = {};
  EZ_ASSERT_ALWAYS(m_ShaderStageCreateInfo.GetCount() == 1, "");
  pipelineInfo.stage = m_ShaderStageCreateInfo[0];
  pipelineInfo.layout = m_PipelineLayout;
  m_Pipeline = m_Device.GetDevice().createComputePipelineUnique({}, pipelineInfo).value;
}

ezRHIPipelineType ezRHIVKComputePipeline::GetPipelineType() const
{
  return ezRHIPipelineType::Compute;
}
