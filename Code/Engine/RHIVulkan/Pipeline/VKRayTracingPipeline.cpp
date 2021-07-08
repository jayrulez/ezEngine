#include <RHIVulkanPCH.h>

#include <RHIVulkan/Pipeline/VKRayTracingPipeline.h>
#include <RHIVulkan/Device/VKDevice.h>

ezRHIVKRayTracingPipeline::ezRHIVKRayTracingPipeline(ezRHIVKDevice& device, const ezRHIRayTracingPipelineCreationDescription& desc)
  : ezRHIVKPipeline(device, desc.m_Program, desc.m_Layout)
  , m_Desc(desc)
{
  ezDynamicArray<vk::RayTracingShaderGroupCreateInfoKHR> groups;
  groups.SetCount(m_Desc.m_Groups.GetCount());

  auto get = [&](ezUInt64 id) -> ezUInt32 {
    auto it = m_ShaderIds.Find(id);
    if (it == end(m_ShaderIds))
    {
      return VK_SHADER_UNUSED_KHR;
    }
    return it.Value();
  };

  for (ezUInt32 i = 0; i < m_Desc.m_Groups.GetCount(); ++i)
  {
    decltype(auto) group = groups[i];
    group.generalShader = VK_SHADER_UNUSED_KHR;
    group.closestHitShader = VK_SHADER_UNUSED_KHR;
    group.anyHitShader = VK_SHADER_UNUSED_KHR;
    group.intersectionShader = VK_SHADER_UNUSED_KHR;

    switch (m_Desc.m_Groups[i].m_Type)
    {
      case ezRHIRayTracingShaderGroupType::General:
        group.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
        group.generalShader = get(m_Desc.m_Groups[i].m_General);
        break;
      case ezRHIRayTracingShaderGroupType::TrianglesHitGroup:
        group.type = vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup;
        group.closestHitShader = get(m_Desc.m_Groups[i].m_ClosestHit);
        group.anyHitShader = get(m_Desc.m_Groups[i].m_AnyHit);
        break;
      case ezRHIRayTracingShaderGroupType::ProceduralHitGroup:
        group.type = vk::RayTracingShaderGroupTypeKHR::eProceduralHitGroup;
        group.intersectionShader = get(m_Desc.m_Groups[i].m_Intersection);
        break;
    }
  }

  vk::RayTracingPipelineCreateInfoKHR rayPipelineInfo{};
  rayPipelineInfo.stageCount = m_ShaderStageCreateInfo.GetCount();
  rayPipelineInfo.pStages = m_ShaderStageCreateInfo.GetData();
  rayPipelineInfo.groupCount = groups.GetCount();
  rayPipelineInfo.pGroups = groups.GetData();
  rayPipelineInfo.maxPipelineRayRecursionDepth = 1;
  rayPipelineInfo.layout = m_PipelineLayout;

  m_Pipeline = m_Device.GetDevice().createRayTracingPipelineKHRUnique({}, {}, rayPipelineInfo).value;
}

ezRHIPipelineType ezRHIVKRayTracingPipeline::GetPipelineType() const
{
  return ezRHIPipelineType::RayTracing;
}

ezDynamicArray<ezUInt8> ezRHIVKRayTracingPipeline::GetRayTracingShaderGroupHandles(ezUInt32 firstGroup, ezUInt32 groupCount) const
{
  ezDynamicArray<ezUInt8> shaderHandlesStorage;
  shaderHandlesStorage.SetCount(groupCount * m_Device.GetCapabilities().m_ShaderGroupHandleSize);

  vk::Result result = m_Device.GetDevice().getRayTracingShaderGroupHandlesKHR(m_Pipeline.get(), firstGroup, groupCount, shaderHandlesStorage.GetCount(), shaderHandlesStorage.GetData());
  return shaderHandlesStorage;
}
