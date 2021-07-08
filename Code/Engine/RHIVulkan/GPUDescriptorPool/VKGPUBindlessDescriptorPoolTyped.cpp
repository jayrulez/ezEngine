#include <RHIVulkan/GPUDescriptorPool/VKGPUBindlessDescriptorPoolTyped.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <stdexcept>

VKGPUBindlessDescriptorPoolTyped::VKGPUBindlessDescriptorPoolTyped(VKDevice& device, VkDescriptorType type)
    : m_device(device)
    , m_type(type)
{
}

void VKGPUBindlessDescriptorPoolTyped::ResizeHeap(uint32_t req_size)
{
    if (req_size > max_bindless_heap_size)
        throw std::runtime_error("Requested size for bindless pool more than max_bindless_heap_size");

    if (m_size >= req_size)
        return;

    Descriptor descriptor;

    VkDescriptorPoolSize pool_size = {};
    pool_size.type = m_type;
    pool_size.descriptorCount = req_size;

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.maxSets = 1;
    pool_info.flags = VkDescriptorPoolCreateFlagBits::eFreeDescriptorSet;

    descriptor.pool = m_device.GetDevice().createDescriptorPoolUnique(pool_info);

    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = 0;
    binding.descriptorType = m_type;
    binding.descriptorCount = max_bindless_heap_size;
    binding.stageFlags = VkShaderStageFlagBits::eAll;

    VkDescriptorBindingFlags binding_flag = VkDescriptorBindingFlagBits::eVariableDescriptorCount;

    VkDescriptorSetLayoutBindingFlagsCreateInfo layout_flags_info = {};
    layout_flags_info.bindingCount = 1;
    layout_flags_info.pBindingFlags = &binding_flag;

    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.bindingCount = 1;
    layout_info.pBindings = &binding;
    layout_info.pNext = &layout_flags_info;

    descriptor.set_layout = m_device.GetDevice().createDescriptorSetLayoutUnique(layout_info);

    VkDescriptorSetVariableDescriptorCountAllocateInfo variable_descriptor_count_info = {};
    variable_descriptor_count_info.descriptorSetCount = 1;
    variable_descriptor_count_info.pDescriptorCounts = &req_size;

    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.descriptorPool = descriptor.pool.get();
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &descriptor.set_layout.get();
    alloc_info.pNext = &variable_descriptor_count_info;

    descriptor.set = std::move(m_device.GetDevice().allocateDescriptorSetsUnique(alloc_info).front());

    if (m_size)
    {
        VkCopyDescriptorSet copy_descriptors;
        copy_descriptors.srcSet = m_descriptor.set.get();
        copy_descriptors.dstSet = descriptor.set.get();
        copy_descriptors.descriptorCount = m_size;
        m_device.GetDevice().updateDescriptorSets(0, nullptr, 1, &copy_descriptors);
    }

    m_size = req_size;

    m_descriptor.set.release();
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
        return VKGPUDescriptorPoolRange(*this, m_descriptor.set.get(), offset, size, m_type);
    }
    if (m_offset + count > m_size)
        ResizeHeap(ezMath::Max(m_offset + count, 2 * (m_size + 1)));
    m_offset += count;
    return VKGPUDescriptorPoolRange(*this, m_descriptor.set.get(), m_offset - count, count, m_type);
}

void VKGPUBindlessDescriptorPoolTyped::OnRangeDestroy(uint32_t offset, uint32_t size)
{
    m_empty_ranges.emplace(size, offset);
}

VkDescriptorSet VKGPUBindlessDescriptorPoolTyped::GetDescriptorSet() const
{
    return m_descriptor.set.get();
}
