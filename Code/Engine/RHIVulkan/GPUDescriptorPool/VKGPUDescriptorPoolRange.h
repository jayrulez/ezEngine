#pragma once
#include <memory>
#include <functional>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKGPUBindlessDescriptorPoolTyped;

class VKGPUDescriptorPoolRange
{
public:
    VKGPUDescriptorPoolRange(VKGPUBindlessDescriptorPoolTyped& pool,
                             VkDescriptorSet descriptor_set,
                             uint32_t offset,
                             uint32_t size,
                             VkDescriptorType type);
    VkDescriptorSet GetDescriptoSet() const;
    uint32_t GetOffset() const;

private:
    std::reference_wrapper<VKGPUBindlessDescriptorPoolTyped> m_pool;
    VkDescriptorSet m_descriptor_set;
    uint32_t m_offset;
    uint32_t m_size;
    VkDescriptorType m_type;
    std::unique_ptr<VKGPUDescriptorPoolRange, std::function<void(VKGPUDescriptorPoolRange*)>> m_callback;
};
