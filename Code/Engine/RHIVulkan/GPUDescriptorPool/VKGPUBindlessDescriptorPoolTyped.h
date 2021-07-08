#pragma once
#include <map>
#include <algorithm>
#include <RHIVulkan/RHIVulkanDLL.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPoolRange.h>

constexpr uint32_t max_bindless_heap_size = 10000;

class VKDevice;

class VKGPUBindlessDescriptorPoolTyped
{
public:
    VKGPUBindlessDescriptorPoolTyped(VKDevice& device, VkDescriptorType type);
    VKGPUDescriptorPoolRange Allocate(uint32_t count);
    void OnRangeDestroy(uint32_t offset, uint32_t size);
    VkDescriptorSet GetDescriptorSet() const;

private:
    void ResizeHeap(uint32_t req_size);

    VKDevice& m_device;
    VkDescriptorType m_type;
    uint32_t m_size = 0;
    uint32_t m_offset = 0;
    struct Descriptor
    {
        VkUniqueDescriptorPool pool;
        VkUniqueDescriptorSetLayout set_layout;
        VkUniqueDescriptorSet set;
    } m_descriptor;
    std::multimap<uint32_t, uint32_t> m_empty_ranges;
};
