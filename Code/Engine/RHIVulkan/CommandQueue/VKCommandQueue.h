#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>
#include <RHI/CommandQueue/CommandQueue.h>

class ezRHIVKCommandQueue : public ezRHICommandQueue
{
public:
  ezRHIVKCommandQueue(ezRHIVKDevice& device, ezRHICommandListType type, ezUInt32 queueFamilyIndex);
  void Wait(const ezRHIFence* fence, ezUInt64 value) override;
  void Signal(const ezRHIFence* fence, ezUInt64 value) override;
  void ExecuteCommandLists(const ezDynamicArray<ezRHICommandList*>& commandLists) override;

  ezRHIVKDevice& GetDevice();
  ezUInt32 GetQueueFamilyIndex();
  vk::Queue GetQueue();

private:
  ezRHIVKDevice& m_Device;
  ezUInt32 m_QueueFamilyIndex;
  vk::Queue m_Queue;
};
