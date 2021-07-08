#include <RHIVulkanPCH.h>

#include <RHIVulkan/Pipeline/VKPipeline.h>
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Program/VKProgram.h>
#include <RHI/Shader/Shader.h>

vk::ShaderStageFlagBits ExecutionModel2Bit(ezRHIShaderKind kind)
{
  switch (kind)
  {
    case ezRHIShaderKind::Vertex:
      return vk::ShaderStageFlagBits::eVertex;
    case ezRHIShaderKind::Pixel:
      return vk::ShaderStageFlagBits::eFragment;
    case ezRHIShaderKind::Compute:
      return vk::ShaderStageFlagBits::eCompute;
    case ezRHIShaderKind::Geometry:
      return vk::ShaderStageFlagBits::eGeometry;
    case ezRHIShaderKind::Amplification:
      return vk::ShaderStageFlagBits::eTaskNV;
    case ezRHIShaderKind::Mesh:
      return vk::ShaderStageFlagBits::eMeshNV;
    case ezRHIShaderKind::RayGeneration:
      return vk::ShaderStageFlagBits::eRaygenKHR;
    case ezRHIShaderKind::Intersection:
      return vk::ShaderStageFlagBits::eIntersectionKHR;
    case ezRHIShaderKind::AnyHit:
      return vk::ShaderStageFlagBits::eAnyHitKHR;
    case ezRHIShaderKind::ClosestHit:
      return vk::ShaderStageFlagBits::eClosestHitKHR;
    case ezRHIShaderKind::Miss:
      return vk::ShaderStageFlagBits::eMissKHR;
    case ezRHIShaderKind::Callable:
      return vk::ShaderStageFlagBits::eCallableKHR;
  }
  EZ_ASSERT_NOT_IMPLEMENTED;
  return {};
}

ezRHIVKPipeline::ezRHIVKPipeline(ezRHIVKDevice& device, const ezRHIProgram* pProgram, const ezRHIBindingSetLayout* pLayout)
  : m_Device{device}
{
  decltype(auto) vkLayout = pLayout->As<ezRHIVKBindingSetLayout>();
  m_PipelineLayout = vkLayout->GetPipelineLayout();

  decltype(auto) shaders = pProgram->GetShaders();
  for (const auto& shader : shaders)
  {
    decltype(auto) blob = shader->GetBlob();
    vk::ShaderModuleCreateInfo shaderModuleInfo = {};
    shaderModuleInfo.codeSize = blob.GetCount();
    shaderModuleInfo.pCode = (ezUInt32*)blob.GetData();
    m_ShaderModules.PushBack(m_Device.GetDevice().createShaderModuleUnique(shaderModuleInfo));

    decltype(auto) reflection = shader->GetReflection();
    decltype(auto) entryPoints = reflection->GetEntryPoints();
    for (const auto& entryPoint : entryPoints)
    {
      m_ShaderIds[shader->GetId(entryPoint.m_Name)] = m_ShaderStageCreateInfo.GetCount();
      decltype(auto) shaderStageCreateInfo = m_ShaderStageCreateInfo.ExpandAndGetRef();
      shaderStageCreateInfo.stage = ExecutionModel2Bit(entryPoint.m_Kind);
      shaderStageCreateInfo.module = m_ShaderModules.PeekBack().get();
       m_EntryPointNames.PushBack(entryPoint.m_Name);
      decltype(auto) name = m_EntryPointNames.PeekBack();
      shaderStageCreateInfo.pName = name.GetData();
    }
  }
}

vk::PipelineLayout ezRHIVKPipeline::GetPipelineLayout() const
{
  return m_PipelineLayout;
}

vk::Pipeline ezRHIVKPipeline::GetPipeline() const
{
  return m_Pipeline.get();
}

ezDynamicArray<ezUInt8> ezRHIVKPipeline::GetRayTracingShaderGroupHandles(ezUInt32 firstGroup, ezUInt32 groupCount) const
{
  return {};
}
