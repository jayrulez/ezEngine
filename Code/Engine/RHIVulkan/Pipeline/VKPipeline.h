#pragma once
#include <RHI/Pipeline/Pipeline.h>
#include <RHI/Program/Program.h>
#include <deque>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;

class VKPipeline : public Pipeline
{
public:
  VKPipeline(VKDevice& device, const std::shared_ptr<Program>& program, const std::shared_ptr<BindingSetLayout>& layout);
  VkPipelineLayout GetPipelineLayout() const;
  VkPipeline GetPipeline() const;
  std::vector<uint8_t> GetRayTracingShaderGroupHandles(uint32_t first_group, uint32_t group_count) const override;

protected:
  VKDevice& m_device;
  std::deque<std::string> entry_point_names;
  std::vector<VkPipelineShaderStageCreateInfo> m_shader_stage_create_info;
  std::vector<VkUniqueShaderModule> m_shader_modules;
  VkUniquePipeline m_pipeline;
  VkPipelineLayout m_pipeline_layout;
  std::map<uint64_t, uint32_t> m_shader_ids;
};
