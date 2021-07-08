#include <RHIVulkanPCH.h>

#include <RHIVulkan/GPUDescriptorPool/VKGPUBindlessDescriptorPoolTyped.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPoolRange.h>
#include <RHIVulkan/Device/VKDevice.h>

ezRHIVKGPUBindlessDescriptorPoolTyped::ezRHIVKGPUBindlessDescriptorPoolTyped(ezRHIVKDevice& device, vk::DescriptorType type)
  : m_Device{device}
  , m_Type{type}
{
}

ezRHIVKGPUDescriptorPoolRange ezRHIVKGPUBindlessDescriptorPoolTyped::Allocate(ezUInt32 count)
{
  auto it = m_EmptyRanges.lower_bound(count);
  if (it != m_EmptyRanges.end())
  {
    ezUInt32 offset = it->second;
    ezUInt32 size = it->first;
    m_EmptyRanges.erase(it);
    return ezRHIVKGPUDescriptorPoolRange(*this, m_Descriptor.m_Set.get(), offset, size, m_Type);
  }
  if (m_Offset + count > m_Size)
    ResizeHeap(ezMath::Max(m_Offset + count, 2 * (m_Size + 1)));
  m_Offset += count;
  return ezRHIVKGPUDescriptorPoolRange(*this, m_Descriptor.m_Set.get(), m_Offset - count, count, m_Type);
}

void ezRHIVKGPUBindlessDescriptorPoolTyped::OnRangeDestroy(ezUInt32 offset, ezUInt32 size)
{
  m_EmptyRanges.emplace(size, offset);
}

vk::DescriptorSet ezRHIVKGPUBindlessDescriptorPoolTyped::GetDescriptorSet() const
{
  return m_Descriptor.m_Set.get();
}

void ezRHIVKGPUBindlessDescriptorPoolTyped::ResizeHeap(ezUInt32 reqSize)
{
  if (reqSize > MAX_BINDLESS_HEAP_SIZE)
  {
    EZ_REPORT_FAILURE("Requested size for bindless pool more than MAX_BINDLESS_HEAP_SIZE");
    return;
  }

  if (m_Size >= reqSize)
    return;

  Descriptor descriptor;

  vk::DescriptorPoolSize poolSize = {};
  poolSize.type = m_Type;
  poolSize.descriptorCount = reqSize;

  vk::DescriptorPoolCreateInfo poolInfo = {};
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;
  poolInfo.maxSets = 1;
  poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

  descriptor.m_Pool = m_Device.GetDevice().createDescriptorPoolUnique(poolInfo);

  vk::DescriptorSetLayoutBinding binding = {};
  binding.binding = 0;
  binding.descriptorType = m_Type;
  binding.descriptorCount = MAX_BINDLESS_HEAP_SIZE;
  binding.stageFlags = vk::ShaderStageFlagBits::eAll;

  vk::DescriptorBindingFlags bindingFlag = vk::DescriptorBindingFlagBits::eVariableDescriptorCount;

  vk::DescriptorSetLayoutBindingFlagsCreateInfo layoutFlagsInfo = {};
  layoutFlagsInfo.bindingCount = 1;
  layoutFlagsInfo.pBindingFlags = &bindingFlag;

  vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &binding;
  layoutInfo.pNext = &layoutFlagsInfo;

  descriptor.m_SetLayout = m_Device.GetDevice().createDescriptorSetLayoutUnique(layoutInfo);

  vk::DescriptorSetVariableDescriptorCountAllocateInfo variableDescriptorCountInfo = {};
  variableDescriptorCountInfo.descriptorSetCount = 1;
  variableDescriptorCountInfo.pDescriptorCounts = &reqSize;

  vk::DescriptorSetAllocateInfo allocInfo = {};
  allocInfo.descriptorPool = descriptor.m_Pool.get();
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &descriptor.m_SetLayout.get();
  allocInfo.pNext = &variableDescriptorCountInfo;

  descriptor.m_Set = std::move(m_Device.GetDevice().allocateDescriptorSetsUnique(allocInfo).front());

  if (m_Size)
  {
    vk::CopyDescriptorSet copyDescriptors;
    copyDescriptors.srcSet = m_Descriptor.m_Set.get();
    copyDescriptors.dstSet = descriptor.m_Set.get();
    copyDescriptors.descriptorCount = m_Size;
    m_Device.GetDevice().updateDescriptorSets(0, nullptr, 1, &copyDescriptors);
  }

  m_Size = reqSize;

  m_Descriptor.m_Set.release();
  m_Descriptor = std::move(descriptor);
}
