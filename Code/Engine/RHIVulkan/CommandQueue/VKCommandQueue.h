#pragma once
#include <RHI/CommandQueue/CommandQueue.h>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;

class VKCommandQueue : public CommandQueue
{
public:
    VKCommandQueue(VKDevice& device, CommandListType type, uint32_t queue_family_index);
    void Wait(const std::shared_ptr<Fence>& fence, uint64_t value) override;
    void Signal(const std::shared_ptr<Fence>& fence, uint64_t value) override;
    void ExecuteCommandLists(const std::vector<std::shared_ptr<CommandList>>& command_lists) override;

    VKDevice& GetDevice();
    uint32_t GetQueueFamilyIndex();
    VkQueue GetQueue();

private:
    VKDevice& m_device;
    uint32_t m_queue_family_index;
    VkQueue m_queue;
};
