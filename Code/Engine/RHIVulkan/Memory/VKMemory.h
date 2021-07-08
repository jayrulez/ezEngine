#pragma once
#include <RHI/Memory/Memory.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;

class VKMemory : public Memory
{
public:
    VKMemory(VKDevice& device, uint64_t size, MemoryType memory_type, uint32_t memory_type_bits, const VkMemoryDedicatedAllocateInfoKHR* dedicated_allocate_info);
    MemoryType GetMemoryType() const override;
    VkDeviceMemory GetMemory() const;

private:
    MemoryType m_memory_type;
    VkDeviceMemory m_memory;
};
