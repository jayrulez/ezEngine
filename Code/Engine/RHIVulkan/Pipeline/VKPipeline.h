#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Pipeline/Pipeline.h>
#include <RHI/Program/Program.h>
#include <deque>

class VKDevice;

class VKPipeline : public Pipeline
{
public:
  VKPipeline(VKDevice& device, const std::shared_ptr<Program>& program, const std::shared_ptr<BindingSetLayout>& layout);
  vk::PipelineLayout GetPipelineLayout() const;
  vk::Pipeline GetPipeline() const;
  std::vector<uint8_t> GetRayTracingShaderGroupHandles(uint32_t first_group, uint32_t group_count) const override;

protected:
  VKDevice& m_device;
  std::deque<ezString> entry_point_names;
  std::vector<vk::PipelineShaderStageCreateInfo> m_shader_stage_create_info;
  std::vector<vk::UniqueShaderModule> m_shader_modules;
  vk::UniquePipeline m_pipeline;
  vk::PipelineLayout m_pipeline_layout;
  ezMap<uint64_t, uint32_t> m_shader_ids;
};
