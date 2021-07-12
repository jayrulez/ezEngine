#include <RHIVulkanPCH.h>

#include <RHI/Shader/Shader.h>
#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Pipeline/VKGraphicsPipeline.h>
#include <RHIVulkan/Pipeline/VKRayTracingPipeline.h>
#include <RHIVulkan/Program/VKProgram.h>
#include <map>

VKRayTracingPipeline::VKRayTracingPipeline(VKDevice& device, const RayTracingPipelineDesc& desc)
  : VKPipeline(device, desc.program, desc.layout)
  , m_desc(desc)
{
  std::vector<vk::RayTracingShaderGroupCreateInfoKHR> groups(m_desc.groups.size());

  auto get = [&](uint64_t id) -> uint32_t {
    auto it = m_shader_ids.Find(id);
    if (it == end(m_shader_ids))
    {
      return VK_SHADER_UNUSED_KHR;
    }
    return it.Value();
  };

  for (size_t i = 0; i < m_desc.groups.size(); ++i)
  {
    decltype(auto) group = groups[i];
    group.generalShader = VK_SHADER_UNUSED_KHR;
    group.closestHitShader = VK_SHADER_UNUSED_KHR;
    group.anyHitShader = VK_SHADER_UNUSED_KHR;
    group.intersectionShader = VK_SHADER_UNUSED_KHR;

    switch (m_desc.groups[i].type)
    {
      case RayTracingShaderGroupType::kGeneral:
        group.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
        group.generalShader = get(m_desc.groups[i].general);
        break;
      case RayTracingShaderGroupType::kTrianglesHitGroup:
        group.type = vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup;
        group.closestHitShader = get(m_desc.groups[i].closest_hit);
        group.anyHitShader = get(m_desc.groups[i].any_hit);
        break;
      case RayTracingShaderGroupType::kProceduralHitGroup:
        group.type = vk::RayTracingShaderGroupTypeKHR::eProceduralHitGroup;
        group.intersectionShader = get(m_desc.groups[i].intersection);
        break;
    }
  }

  vk::RayTracingPipelineCreateInfoKHR ray_pipeline_info{};
  ray_pipeline_info.stageCount = static_cast<uint32_t>(m_shader_stage_create_info.size());
  ray_pipeline_info.pStages = m_shader_stage_create_info.data();
  ray_pipeline_info.groupCount = static_cast<uint32_t>(groups.size());
  ray_pipeline_info.pGroups = groups.data();
  ray_pipeline_info.maxPipelineRayRecursionDepth = 1;
  ray_pipeline_info.layout = m_pipeline_layout;

  m_pipeline = m_device.GetDevice().createRayTracingPipelineKHRUnique({}, {}, ray_pipeline_info).value;
}

PipelineType VKRayTracingPipeline::GetPipelineType() const
{
  return PipelineType::kRayTracing;
}

std::vector<uint8_t> VKRayTracingPipeline::GetRayTracingShaderGroupHandles(uint32_t first_group, uint32_t group_count) const
{
  std::vector<uint8_t> shader_handles_storage(group_count * m_device.GetShaderGroupHandleSize());
  vk::Result res = m_device.GetDevice().getRayTracingShaderGroupHandlesKHR(m_pipeline.get(), first_group, group_count, (ezUInt32)shader_handles_storage.size(), shader_handles_storage.data());
  return shader_handles_storage;
}
