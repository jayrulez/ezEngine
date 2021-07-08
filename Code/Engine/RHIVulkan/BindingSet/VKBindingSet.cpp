#include <RHIVulkanPCH.h>

#include <RHIVulkan/BindingSet/VKBindingSet.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/View/VKView.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUBindlessDescriptorPoolTyped.h>


ezRHIVKBindingSet::ezRHIVKBindingSet(ezRHIVKDevice& device, ezRHIVKBindingSetLayout* pLayout)
  : m_Device(device)
  , m_Layout(pLayout)
{
  decltype(auto) bindlessType = m_Layout->GetBindlessType();
  decltype(auto) descriptorSetLayouts = m_Layout->GetDescriptorSetLayouts();
  decltype(auto) descriptorCountBySet = m_Layout->GetDescriptorCountBySet();
  for (ezUInt32 i = 0; i < descriptorSetLayouts.GetCount(); ++i)
  {
    if (bindlessType.Contains(i))
    {
      m_DescriptorSets.PushBack(m_Device.GetGPUBindlessDescriptorPool((*bindlessType.GetValue(i))).GetDescriptorSet());
    }
    else
    {
      m_Descriptors.PushBack(m_Device.GetGPUDescriptorPool().AllocateDescriptorSet(descriptorSetLayouts[i].get(), descriptorCountBySet[i]));
      m_DescriptorSets.PushBack(m_Descriptors.PeekBack().m_Set.get());
    }
  }
}

void ezRHIVKBindingSet::WriteBindingsPlatform(const ezDynamicArray<ezRHIBindingDescription>& bindings)
{
  ezDynamicArray<vk::WriteDescriptorSet> descriptors;
  for (const auto& binding : bindings)
  {
    decltype(auto) vk_view = binding.m_View->As<ezRHIVKView>();
    vk::WriteDescriptorSet descriptor = vk_view->GetDescriptor();
    descriptor.descriptorType = GetDescriptorType(binding.m_BindKey.m_ViewType);
    descriptor.dstSet = m_DescriptorSets[binding.m_BindKey.m_Space];
    descriptor.dstBinding = binding.m_BindKey.m_Slot;
    descriptor.dstArrayElement = 0;
    descriptor.descriptorCount = 1;
    if (descriptor.pImageInfo || descriptor.pBufferInfo || descriptor.pTexelBufferView || descriptor.pNext)
    {
      descriptors.PushBack(descriptor);
    }
  }

  if (!descriptors.IsEmpty())
  {
    m_Device.GetDevice().updateDescriptorSets(descriptors.GetCount(), descriptors.GetData(), 0, nullptr);
  }
}

const ezDynamicArray<vk::DescriptorSet>& ezRHIVKBindingSet::GetDescriptorSets() const
{
  return m_DescriptorSets;
}
