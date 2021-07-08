#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/BindingSetLayout/BindingSetLayout.h>

class VKDevice;

class VKBindingSetLayout
    : public BindingSetLayout
{
public:
    VKBindingSetLayout(VKDevice& device, const std::vector<BindKey>& descs);

    const std::map<uint32_t, VkDescriptorType>& GetBindlessType() const;
    const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() const;
    const std::vector<std::map<VkDescriptorType, size_t>>& GetDescriptorCountBySet() const;
    VkPipelineLayout GetPipelineLayout() const;

private:
    std::map<uint32_t, VkDescriptorType> m_bindless_type;
    std::vector<VkDescriptorSetLayout> m_descriptor_set_layouts;
    std::vector<std::map<VkDescriptorType, size_t>> m_descriptor_count_by_set;
    VkPipelineLayout m_pipeline_layout;
};

VkDescriptorType GetDescriptorType(ViewType view_type);
