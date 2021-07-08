#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

class ezRHIVKGPUDescriptorPoolRange
{
public:
  ezRHIVKGPUDescriptorPoolRange(ezRHIVKGPUBindlessDescriptorPoolTyped& pool,
    vk::DescriptorSet descriptorSet,
    ezUInt32 offset,
    ezUInt32 size,
    vk::DescriptorType type);
  vk::DescriptorSet GetDescriptoSet() const;
  ezUInt32 GetOffset() const;

private:
  std::reference_wrapper<ezRHIVKGPUBindlessDescriptorPoolTyped> m_Pool;
  vk::DescriptorSet m_DescriptorSet;
  ezUInt32 m_Offset;
  ezUInt32 m_Size;
  vk::DescriptorType m_Type;
  std::unique_ptr<ezRHIVKGPUDescriptorPoolRange, std::function<void(ezRHIVKGPUDescriptorPoolRange*)>> m_Callback;
};
