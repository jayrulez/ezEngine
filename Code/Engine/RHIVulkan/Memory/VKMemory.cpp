#include <RHIVulkanPCH.h>

#include <RHIVulkan/Memory/VKMemory.h>
#include <RHIVulkan/Device/VKDevice.h>

ezRHIVKMemory::ezRHIVKMemory(ezRHIVKDevice& device, ezUInt64 size, ezRHIMemoryType memoryType, ezUInt32 memoryTypeBits, const vk::MemoryDedicatedAllocateInfoKHR* dedicatedAllocateInfo)
  : m_MemoryType{memoryType}
{
  vk::MemoryAllocateFlagsInfo allocFlagInfo = {};
  allocFlagInfo.pNext = dedicatedAllocateInfo;
  allocFlagInfo.flags = vk::MemoryAllocateFlagBits::eDeviceAddress;

  vk::MemoryPropertyFlags properties = {};
  if (memoryType == ezRHIMemoryType::Default)
    properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
  else if (memoryType == ezRHIMemoryType::Upload)
    properties = vk::MemoryPropertyFlagBits::eHostVisible;
  else if (memoryType == ezRHIMemoryType::Readback)
    properties = vk::MemoryPropertyFlagBits::eHostVisible;

  vk::MemoryAllocateInfo allocInfo = {};
  allocInfo.pNext = &allocFlagInfo;
  allocInfo.allocationSize = size;
  allocInfo.memoryTypeIndex = device.FindMemoryType(memoryTypeBits, properties);
  m_Memory = device.GetDevice().allocateMemoryUnique(allocInfo);
}

ezRHIMemoryType ezRHIVKMemory::GetMemoryType() const
{
  return m_MemoryType;
}

vk::DeviceMemory ezRHIVKMemory::GetMemory() const
{
  return m_Memory.get();
}
