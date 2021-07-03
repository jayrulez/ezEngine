#pragma once
#include <RHI/BindingSet/BindingSet.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPool.h>
#include <vulkan/vulkan.hpp>

class VKDevice;
class VKBindingSetLayout;

class VKBindingSet
    : public BindingSet
{
public:
    VKBindingSet(VKDevice& device, const ezSharedPtr<VKBindingSetLayout>& layout);

    void WriteBindings(const std::vector<BindingDesc>& bindings) override;
    const std::vector<vk::DescriptorSet>& GetDescriptorSets() const;

private:
    VKDevice& m_device;
    std::vector<DescriptorSetPool> m_descriptors;
    std::vector<vk::DescriptorSet> m_descriptor_sets;
    ezSharedPtr<VKBindingSetLayout> m_layout;
};
