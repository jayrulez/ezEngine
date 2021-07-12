#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Memory/Memory.h>
#include <RHI/Instance/BaseTypes.h>

class VKDevice;

class VKMemory : public Memory
{
public:
    VKMemory(VKDevice& device, uint64_t size, MemoryType memory_type, uint32_t memory_type_bits, const vk::MemoryDedicatedAllocateInfoKHR* dedicated_allocate_info);
    MemoryType GetMemoryType() const override;
    vk::DeviceMemory GetMemory() const;

private:
    MemoryType m_memory_type;
    vk::UniqueDeviceMemory m_memory;
};
