#include <RHIVulkanPCH.h>

#include <Foundation/Containers/Map.h>
#include <RHIVulkan/BindingSet/VKBindingSet.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/View/VKView.h>

VKBindingSet::VKBindingSet(VKDevice& device, const std::shared_ptr<VKBindingSetLayout>& layout)
  : m_device(device)
  , m_layout(layout)
{
  decltype(auto) bindless_type = m_layout->GetBindlessType();
  decltype(auto) descriptor_set_layouts = m_layout->GetDescriptorSetLayouts();
  decltype(auto) descriptor_count_by_set = m_layout->GetDescriptorCountBySet();
  for (ezUInt32 i = 0; i < (ezUInt32)descriptor_set_layouts.size(); ++i)
  {
    if (bindless_type.Contains(i))
    {
      m_descriptor_sets.emplace_back(m_device.GetGPUBindlessDescriptorPool(bindless_type.Find(i).Value())->GetDescriptorSet());
    }
    else
    {
      m_descriptors.emplace_back(m_device.GetGPUDescriptorPool().AllocateDescriptorSet(descriptor_set_layouts[i].get(), descriptor_count_by_set[i]));
      m_descriptor_sets.emplace_back(m_descriptors.back().set.get());
    }
  }
}

void VKBindingSet::WriteBindings(const std::vector<BindingDesc>& bindings)
{
  std::vector<vk::WriteDescriptorSet> descriptors;
  for (const auto& binding : bindings)
  {
    decltype(auto) vk_view = binding.view->As<VKView>();
    vk::WriteDescriptorSet descriptor = vk_view.GetDescriptor();
    descriptor.descriptorType = GetDescriptorType(binding.bind_key.view_type);
    descriptor.dstSet = m_descriptor_sets[binding.bind_key.space];
    descriptor.dstBinding = binding.bind_key.slot;
    descriptor.dstArrayElement = 0;
    descriptor.descriptorCount = 1;
    if (descriptor.pImageInfo || descriptor.pBufferInfo || descriptor.pTexelBufferView || descriptor.pNext)
    {
      descriptors.emplace_back(descriptor);
    }
  }

  if (!descriptors.empty())
  {
    m_device.GetDevice().updateDescriptorSets((ezUInt32)descriptors.size(), descriptors.data(), 0, nullptr);
  }
}

const std::vector<vk::DescriptorSet>& VKBindingSet::GetDescriptorSets() const
{
  return m_descriptor_sets;
}
