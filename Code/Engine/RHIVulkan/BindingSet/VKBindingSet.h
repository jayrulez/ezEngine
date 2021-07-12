#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/BindingSet/BindingSet.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPool.h>

class VKDevice;
class VKBindingSetLayout;

class VKBindingSet
    : public BindingSet
{
public:
    VKBindingSet(VKDevice& device, const std::shared_ptr<VKBindingSetLayout>& layout);

    void WriteBindings(const std::vector<BindingDesc>& bindings) override;
    const std::vector<vk::DescriptorSet>& GetDescriptorSets() const;

private:
    VKDevice& m_device;
    std::vector<DescriptorSetPool> m_descriptors;
    std::vector<vk::DescriptorSet> m_descriptor_sets;
    std::shared_ptr<VKBindingSetLayout> m_layout;
};
