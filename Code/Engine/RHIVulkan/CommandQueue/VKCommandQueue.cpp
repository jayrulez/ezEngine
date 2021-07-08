#include <RHIVulkanPCH.h>

#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/CommandList/VKCommandList.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>

ezRHIVKCommandQueue::ezRHIVKCommandQueue(ezRHIVKDevice& device, ezRHICommandListType type, ezUInt32 queueFamilyIndex)
  : m_Device{device}
  , m_QueueFamilyIndex{queueFamilyIndex}
{
  m_Queue = m_Device.GetDevice().getQueue(m_QueueFamilyIndex, 0);
}

void ezRHIVKCommandQueue::Wait(const ezRHIFence* fence, ezUInt64 value)
{
  decltype(auto) vkFence = fence->As<ezRHIVKTimelineSemaphore>();
  vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
  timelineInfo.waitSemaphoreValueCount = 1;
  timelineInfo.pWaitSemaphoreValues = &value;

  vk::SubmitInfo signalSubmitInfo = {};
  signalSubmitInfo.pNext = &timelineInfo;
  signalSubmitInfo.waitSemaphoreCount = 1;
  signalSubmitInfo.pWaitSemaphores = &vkFence->GetFence();
  vk::PipelineStageFlags wait_dst_stage_mask = vk::PipelineStageFlagBits::eAllCommands;
  signalSubmitInfo.pWaitDstStageMask = &wait_dst_stage_mask;
  vk::Result result = m_Queue.submit(1, &signalSubmitInfo, {});
}

void ezRHIVKCommandQueue::Signal(const ezRHIFence* fence, ezUInt64 value)
{
  decltype(auto) vkFence = fence->As<ezRHIVKTimelineSemaphore>();
  vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
  timelineInfo.signalSemaphoreValueCount = 1;
  timelineInfo.pSignalSemaphoreValues = &value;

  vk::SubmitInfo signalSubmitInfo = {};
  signalSubmitInfo.pNext = &timelineInfo;
  signalSubmitInfo.signalSemaphoreCount = 1;
  signalSubmitInfo.pSignalSemaphores = &vkFence->GetFence();
  vk::Result result = m_Queue.submit(1, &signalSubmitInfo, {});
}

void ezRHIVKCommandQueue::ExecuteCommandLists(const ezDynamicArray<ezRHICommandList*>& commandLists)
{
  ezDynamicArray<vk::CommandBuffer> vkCommandLists;
  for (auto& commandList : commandLists)
  {
    if (!commandList)
      continue;
    decltype(auto) vkCommandList = commandList->As<ezRHIVKCommandList>();
    vkCommandLists.PushBack(vkCommandList->GetCommandList());
  }

  vk::SubmitInfo submitInfo = {};
  submitInfo.commandBufferCount = vkCommandLists.GetCount();
  submitInfo.pCommandBuffers = vkCommandLists.GetData();

  vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eAllCommands;
  submitInfo.pWaitDstStageMask = &waitDstStageMask;

  vk::Result result = m_Queue.submit(1, &submitInfo, {});
}

ezRHIVKDevice& ezRHIVKCommandQueue::GetDevice()
{
  return m_Device;
}

ezUInt32 ezRHIVKCommandQueue::GetQueueFamilyIndex()
{
  return m_QueueFamilyIndex;
}

vk::Queue ezRHIVKCommandQueue::GetQueue()
{
  return m_Queue;
}
