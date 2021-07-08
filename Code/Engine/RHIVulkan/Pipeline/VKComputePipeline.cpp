#include <RHI/Shader/Shader.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Pipeline/VKComputePipeline.h>
#include <RHIVulkan/Pipeline/VKGraphicsPipeline.h>
#include <RHIVulkan/Program/VKProgram.h>
#include <map>

VKComputePipeline::VKComputePipeline(VKDevice& device, const ComputePipelineDesc& desc)
  : VKPipeline(device, desc.program, desc.layout)
  , m_desc(desc)
{
  VkComputePipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VkStructureType::VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  assert(m_shader_stage_create_info.size() == 1);
  pipeline_info.stage = m_shader_stage_create_info.front();
  pipeline_info.layout = m_pipeline_layout;

  vkCreateComputePipelines(m_device.GetDevice(), {}, 1, &pipeline_info,nullptr, &m_pipeline);
}

PipelineType VKComputePipeline::GetPipelineType() const
{
  return PipelineType::kCompute;
}
