#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

class ezRHIVKGPUDescriptorPool
{
public:
  ezRHIVKGPUDescriptorPool(ezRHIVKDevice& device);
  ezRHIVKDescriptorSetPool AllocateDescriptorSet(const vk::DescriptorSetLayout& setLayout, const ezMap<vk::DescriptorType, ezUInt32>& count);

private:
  vk::UniqueDescriptorPool CreateDescriptorPool(const ezMap<vk::DescriptorType, ezUInt32>& count);

  ezRHIVKDevice& m_Device;
};
