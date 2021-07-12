#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <memory>

class VKGPUBindlessDescriptorPoolTyped;

class VKGPUDescriptorPoolRange
{
public:
  VKGPUDescriptorPoolRange(VKGPUBindlessDescriptorPoolTyped& pool,
    vk::DescriptorSet descriptor_set,
    uint32_t offset,
    uint32_t size,
    vk::DescriptorType type);
  ~VKGPUDescriptorPoolRange();

  vk::DescriptorSet GetDescriptoSet() const;
  uint32_t GetOffset() const;

private:
  std::reference_wrapper<VKGPUBindlessDescriptorPoolTyped> m_pool;
  vk::DescriptorSet m_descriptor_set;
  uint32_t m_offset;
  uint32_t m_size;
  vk::DescriptorType m_type;
  //std::unique_ptr<VKGPUDescriptorPoolRange, std::function<void(VKGPUDescriptorPoolRange*)>> m_callback;
};
