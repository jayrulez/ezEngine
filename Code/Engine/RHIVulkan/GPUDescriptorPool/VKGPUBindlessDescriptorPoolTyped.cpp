#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUBindlessDescriptorPoolTyped.h>
#include <stdexcept>

VKGPUBindlessDescriptorPoolTyped::VKGPUBindlessDescriptorPoolTyped(VKDevice& device, VkDescriptorType type)
  : m_device(device)
  , m_type(type)
{
}

void VKGPUBindlessDescriptorPoolTyped::ResizeHeap(uint32_t req_size)
{
  if (req_size > max_bindless_heap_size)
  {
    EZ_REPORT_FAILURE("Requested size for bindless pool more than max_bindless_heap_size");
    return;
  }

  if (m_size >= req_size)
    return;

  Descriptor descriptor;

  VkDescriptorPoolSize pool_size = {};
  pool_size.type = m_type;
  pool_size.descriptorCount = req_size;

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = 1;
  pool_info.pPoolSizes = &pool_size;
  pool_info.maxSets = 1;
  pool_info.flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  vkCreateDescriptorPool(m_device.GetDevice(), &pool_info, nullptr, &descriptor.pool);

  VkDescriptorSetLayoutBinding binding = {};
  binding.binding = 0;
  binding.descriptorType = m_type;
  binding.descriptorCount = max_bindless_heap_size;
  binding.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

  VkDescriptorBindingFlags binding_flag = VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;

  VkDescriptorSetLayoutBindingFlagsCreateInfo layout_flags_info = {};
  layout_flags_info.bindingCount = 1;
  layout_flags_info.pBindingFlags = &binding_flag;

  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = 1;
  layout_info.pBindings = &binding;
  layout_info.pNext = &layout_flags_info;
  vkCreateDescriptorSetLayout(m_device.GetDevice(), &layout_info, nullptr, &descriptor.set_layout);

  VkDescriptorSetVariableDescriptorCountAllocateInfo variable_descriptor_count_info = {};
  variable_descriptor_count_info.descriptorSetCount = 1;
  variable_descriptor_count_info.pDescriptorCounts = &req_size;

  VkDescriptorSetAllocateInfo alloc_info = {};
  alloc_info.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = descriptor.pool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &descriptor.set_layout;
  alloc_info.pNext = &variable_descriptor_count_info;
  vkAllocateDescriptorSets(m_device.GetDevice(), &alloc_info, &descriptor.set);

  if (m_size)
  {
    VkCopyDescriptorSet copy_descriptors;
    copy_descriptors.srcSet = m_descriptor.set;
    copy_descriptors.dstSet = descriptor.set;
    copy_descriptors.descriptorCount = m_size;
    vkUpdateDescriptorSets(m_device.GetDevice() , 0, nullptr, 1, &copy_descriptors);
  }

  m_size = req_size;

  m_descriptor.set = VK_NULL_HANDLE;
  m_descriptor = std::move(descriptor);
}

VKGPUDescriptorPoolRange VKGPUBindlessDescriptorPoolTyped::Allocate(uint32_t count)
{
  auto it = m_empty_ranges.lower_bound(count);
  if (it != m_empty_ranges.end())
  {
    ezUInt32 offset = it->second;
    ezUInt32 size = it->first;
    m_empty_ranges.erase(it);
    return VKGPUDescriptorPoolRange(*this, m_descriptor.set, offset, size, m_type);
  }
  if (m_offset + count > m_size)
    ResizeHeap(ezMath::Max(m_offset + count, 2 * (m_size + 1)));
  m_offset += count;
  return VKGPUDescriptorPoolRange(*this, m_descriptor.set, m_offset - count, count, m_type);
}

void VKGPUBindlessDescriptorPoolTyped::OnRangeDestroy(uint32_t offset, uint32_t size)
{
  m_empty_ranges.emplace(size, offset);
}

VkDescriptorSet VKGPUBindlessDescriptorPoolTyped::GetDescriptorSet() const
{
  return m_descriptor.set;
}
