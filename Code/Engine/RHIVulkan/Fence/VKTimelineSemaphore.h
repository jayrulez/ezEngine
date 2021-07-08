#pragma once
#include <RHI/Fence/Fence.h>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;

class VKTimelineSemaphore : public Fence
{
public:
    VKTimelineSemaphore(VKDevice& device, uint64_t initial_value);
    uint64_t GetCompletedValue() override;
    void Wait(uint64_t value) override;
    void Signal(uint64_t value) override;

    const VkSemaphore& GetFence() const;

private:
    VKDevice& m_device;
    VkUniqueSemaphore m_timeline_semaphore;
};
