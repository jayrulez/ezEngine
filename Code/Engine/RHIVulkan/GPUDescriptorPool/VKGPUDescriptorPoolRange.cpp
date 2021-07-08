#include <RHIVulkanPCH.h>

#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPoolRange.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUBindlessDescriptorPoolTyped.h>

ezRHIVKGPUDescriptorPoolRange::ezRHIVKGPUDescriptorPoolRange(ezRHIVKGPUBindlessDescriptorPoolTyped& pool,
  vk::DescriptorSet descriptorSet,
  ezUInt32 offset,
  ezUInt32 size,
  vk::DescriptorType type)
  : m_Pool{pool}
  , m_DescriptorSet{descriptorSet}
  , m_Offset{offset}
  , m_Size{size}
  , m_Type{type}
  , m_Callback(this, [m_Offset = m_Offset, m_Size = m_Size, m_Pool = m_Pool](auto) { m_Pool.get().OnRangeDestroy(m_Offset, m_Size); })
{
}

vk::DescriptorSet ezRHIVKGPUDescriptorPoolRange::GetDescriptoSet() const
{
  return vk::DescriptorSet();
}

ezUInt32 ezRHIVKGPUDescriptorPoolRange::GetOffset() const
{
  return m_Offset;
}
