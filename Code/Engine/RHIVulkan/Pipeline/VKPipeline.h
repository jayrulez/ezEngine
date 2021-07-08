#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/Pipeline/Pipeline.h>

class ezRHIVKPipeline : public ezRHIPipeline
{
public:
  ezRHIVKPipeline(ezRHIVKDevice& device, const ezRHIProgram* pProgram, const ezRHIBindingSetLayout* pLayout);
  vk::PipelineLayout GetPipelineLayout() const;
  vk::Pipeline GetPipeline() const;
  ezDynamicArray<ezUInt8> GetRayTracingShaderGroupHandles(ezUInt32 firstGroup, ezUInt32 groupCount) const override;

protected:
  ezRHIVKDevice& m_Device;
  ezDeque<ezString> m_EntryPointNames;
  ezDynamicArray<vk::PipelineShaderStageCreateInfo> m_ShaderStageCreateInfo;
  ezDynamicArray<vk::UniqueShaderModule> m_ShaderModules;
  vk::UniquePipeline m_Pipeline;
  vk::PipelineLayout m_PipelineLayout;
  ezMap<ezUInt64, ezUInt32> m_ShaderIds;
};
