#include <RHIVulkanPCH.h>

#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPool.h>

ezRHIVKGPUDescriptorPool::ezRHIVKGPUDescriptorPool(ezRHIVKDevice& device)
  : m_Device{device}
{
}

ezRHIVKDescriptorSetPool ezRHIVKGPUDescriptorPool::AllocateDescriptorSet(const vk::DescriptorSetLayout& setLayout, const ezMap<vk::DescriptorType, ezUInt32>& count)
{
  ezRHIVKDescriptorSetPool res = {};
  res.m_Pool = CreateDescriptorPool(count);

  vk::DescriptorSetAllocateInfo allocInfo = {};
  allocInfo.descriptorPool = res.m_Pool.get();
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &setLayout;
  auto descriptorSets = m_Device.GetDevice().allocateDescriptorSetsUnique(allocInfo);
  res.m_Set = std::move(descriptorSets.front());

  return res;
}

vk::UniqueDescriptorPool ezRHIVKGPUDescriptorPool::CreateDescriptorPool(const ezMap<vk::DescriptorType, ezUInt32>& count)
{

  ezDynamicArray<vk::DescriptorPoolSize> poolSizes;
  for (auto& x : count)
  {

    vk::DescriptorPoolSize& poolSize = poolSizes.ExpandAndGetRef();
    poolSize.type = x.Key();
    poolSize.descriptorCount = x.Value();
  }

  // TODO: fix me
  if (count.IsEmpty())
  {
    vk::DescriptorPoolSize& poolSize = poolSizes.ExpandAndGetRef();
    poolSize.type = vk::DescriptorType::eSampler;
    poolSize.descriptorCount = 1;
  }

  vk::DescriptorPoolCreateInfo poolInfo = {};
  poolInfo.poolSizeCount = poolSizes.GetCount();
  poolInfo.pPoolSizes = poolSizes.GetData();
  poolInfo.maxSets = 1;
  poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

  return m_Device.GetDevice().createDescriptorPoolUnique(poolInfo);
}
