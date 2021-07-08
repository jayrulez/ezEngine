#pragma once
#include <map>
#include <algorithm>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;

struct DescriptorSetPool
{
    VkDescriptorPool pool;
    VkDescriptorSet set;
};

class VKGPUDescriptorPool
{
public:
    VKGPUDescriptorPool(VKDevice& device);
    DescriptorSetPool AllocateDescriptorSet(const VkDescriptorSetLayout& set_layout, const std::map<VkDescriptorType, size_t>& count);

private:
    VkDescriptorPool CreateDescriptorPool(const std::map<VkDescriptorType, size_t>& count);

    VKDevice& m_device;
};
