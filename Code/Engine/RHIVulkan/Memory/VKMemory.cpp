#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Memory/VKMemory.h>

VKMemory::VKMemory(VKDevice& device, uint64_t size, MemoryType memory_type, uint32_t memory_type_bits, const VkMemoryDedicatedAllocateInfoKHR* dedicated_allocate_info)
  : m_memory_type(memory_type)
{
  VkMemoryAllocateFlagsInfo alloc_flag_info = {};
  alloc_flag_info.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
  alloc_flag_info.pNext = dedicated_allocate_info;
  alloc_flag_info.flags = VkMemoryAllocateFlagBits::VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

  VkMemoryPropertyFlags properties = {};
  if (memory_type == MemoryType::kDefault)
    properties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  else if (memory_type == MemoryType::kUpload)
    properties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  else if (memory_type == MemoryType::kReadback)
    properties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = &alloc_flag_info;
  alloc_info.allocationSize = size;
  alloc_info.memoryTypeIndex = device.FindMemoryType(memory_type_bits, properties);
  vkAllocateMemory(device.GetDevice(), &alloc_info, nullptr, &m_memory);
}

MemoryType VKMemory::GetMemoryType() const
{
  return m_memory_type;
}

VkDeviceMemory VKMemory::GetMemory() const
{
  return m_memory;
}
