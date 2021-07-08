#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/Memory/Memory.h>

class ezRHIVKMemory : public ezRHIMemory
{
public:
  ezRHIVKMemory(ezRHIVKDevice& device, ezUInt64 size, ezRHIMemoryType memoryType, ezUInt32 memoryTypeBits, const vk::MemoryDedicatedAllocateInfoKHR* dedicatedAllocateInfo);
  ezRHIMemoryType GetMemoryType() const override;
  vk::DeviceMemory GetMemory() const;

private:
  ezRHIMemoryType m_MemoryType;
  vk::UniqueDeviceMemory m_Memory;
};
