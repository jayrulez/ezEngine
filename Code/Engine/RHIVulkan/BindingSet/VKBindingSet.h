#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/BindingSet/BindingSet.h>

class ezRHIVKBindingSet : public ezRHIBindingSet
{
public:
  ezRHIVKBindingSet(ezRHIVKDevice& device, ezRHIVKBindingSetLayout* pLayout);

  void WriteBindingsPlatform(const ezDynamicArray<ezRHIBindingDescription>& bindings) override;
  const ezDynamicArray<vk::DescriptorSet>& GetDescriptorSets() const;

private:
  ezRHIVKDevice& m_Device;
  ezDynamicArray<ezRHIVKDescriptorSetPool> m_Descriptors;
  ezDynamicArray<vk::DescriptorSet> m_DescriptorSets;
  ezRHIVKBindingSetLayout* m_Layout;
};
