#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <map>

class ezRHIVKGPUBindlessDescriptorPoolTyped
{
public:
  ezRHIVKGPUBindlessDescriptorPoolTyped(ezRHIVKDevice& device, vk::DescriptorType type);
  ezRHIVKGPUDescriptorPoolRange Allocate(ezUInt32 count);
  void OnRangeDestroy(ezUInt32 offset, ezUInt32 size);
  vk::DescriptorSet GetDescriptorSet() const;

private:
  void ResizeHeap(ezUInt32 reqSize);

  ezRHIVKDevice& m_Device;
  vk::DescriptorType m_Type;
  ezUInt32 m_Size = 0;
  ezUInt32 m_Offset = 0;
  struct Descriptor
  {
    vk::UniqueDescriptorPool m_Pool = {};
    vk::UniqueDescriptorSetLayout m_SetLayout = {};
    vk::UniqueDescriptorSet m_Set = {};
  } m_Descriptor;
  std::multimap<ezUInt32, ezUInt32> m_EmptyRanges; // todo: remove std multimap
};
