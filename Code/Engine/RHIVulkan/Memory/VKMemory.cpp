#include <RHIVulkan/Memory/VKMemory.h>
#include <RHIVulkan/Device/VKDevice.h>

VKMemory::VKMemory(VKDevice& device, uint64_t size, MemoryType memory_type, uint32_t memory_type_bits, const VkMemoryDedicatedAllocateInfoKHR* dedicated_allocate_info)
    : m_memory_type(memory_type)
{
    VkMemoryAllocateFlagsInfo alloc_flag_info = {};
    alloc_flag_info.pNext = dedicated_allocate_info;
    alloc_flag_info.flags = VkMemoryAllocateFlagBits::eDeviceAddress;

    VkMemoryPropertyFlags properties = {};
    if (memory_type == MemoryType::kDefault)
        properties = VkMemoryPropertyFlagBits::eDeviceLocal;
    else if (memory_type == MemoryType::kUpload)
        properties = VkMemoryPropertyFlagBits::eHostVisible;
    else if (memory_type == MemoryType::kReadback)
        properties = VkMemoryPropertyFlagBits::eHostVisible;

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.pNext = &alloc_flag_info;
    alloc_info.allocationSize = size;
    alloc_info.memoryTypeIndex = device.FindMemoryType(memory_type_bits, properties);
    m_memory = device.GetDevice().allocateMemoryUnique(alloc_info);
}

MemoryType VKMemory::GetMemoryType() const
{
    return m_memory_type;
}

VkDeviceMemory VKMemory::GetMemory() const
{
    return m_memory.get();
}
