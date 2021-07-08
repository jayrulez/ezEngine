#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/BindingSetLayout/BindingSetLayout.h>

class ezRHIVKBindingSetLayout : public ezRHIBindingSetLayout
{
public:
  ezRHIVKBindingSetLayout(ezRHIVKDevice& device, const ezDynamicArray<ezRHIBindKeyDescription>& descs);

  const ezMap<ezUInt32, vk::DescriptorType>& GetBindlessType() const;
  const ezDynamicArray<vk::UniqueDescriptorSetLayout>& GetDescriptorSetLayouts() const;
  const ezDynamicArray<ezMap<vk::DescriptorType, ezUInt32>>& GetDescriptorCountBySet() const;
  vk::PipelineLayout GetPipelineLayout() const;

private:
  ezMap<ezUInt32, vk::DescriptorType> m_BindlessType;
  ezDynamicArray<vk::UniqueDescriptorSetLayout> m_DescriptorSetLayouts;
  ezDynamicArray<ezMap<vk::DescriptorType, ezUInt32>> m_DescriptorCountBySet;
  vk::UniquePipelineLayout m_PipelineLayout;
};

vk::DescriptorType GetDescriptorType(ezRHIViewType view_type);
