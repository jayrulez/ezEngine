#include <RHIVulkan/CommandList/VKCommandList.h>
#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>

VKCommandQueue::VKCommandQueue(VKDevice& device, CommandListType type, uint32_t queue_family_index)
  : m_device(device)
  , m_queue_family_index(queue_family_index)
{
  vkGetDeviceQueue(m_device.GetDevice(), m_queue_family_index, 0, &m_queue);
}

void VKCommandQueue::Wait(const std::shared_ptr<Fence>& fence, uint64_t value)
{
  decltype(auto) vk_fence = fence->As<VKTimelineSemaphore>();
  VkTimelineSemaphoreSubmitInfo timeline_info = {};
  timeline_info.sType = VkStructureType::VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
  timeline_info.waitSemaphoreValueCount = 1;
  timeline_info.pWaitSemaphoreValues = &value;

  VkSubmitInfo signal_submit_info = {};
  signal_submit_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
  signal_submit_info.pNext = &timeline_info;
  signal_submit_info.waitSemaphoreCount = 1;
  signal_submit_info.pWaitSemaphores = &vk_fence.GetFence();
  VkPipelineStageFlags wait_dst_stage_mask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
  signal_submit_info.pWaitDstStageMask = &wait_dst_stage_mask;

  VkResult res = vkQueueSubmit(m_queue, 1, &signal_submit_info, {});
}

void VKCommandQueue::Signal(const std::shared_ptr<Fence>& fence, uint64_t value)
{
  decltype(auto) vk_fence = fence->As<VKTimelineSemaphore>();
  VkTimelineSemaphoreSubmitInfo timeline_info = {};
  timeline_info.sType = VkStructureType::VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
  timeline_info.signalSemaphoreValueCount = 1;
  timeline_info.pSignalSemaphoreValues = &value;

  VkSubmitInfo signal_submit_info = {};
  signal_submit_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
  signal_submit_info.pNext = &timeline_info;
  signal_submit_info.signalSemaphoreCount = 1;
  signal_submit_info.pSignalSemaphores = &vk_fence.GetFence();
  VkResult res = vkQueueSubmit(m_queue, 1, &signal_submit_info, {});
}

void VKCommandQueue::ExecuteCommandLists(const std::vector<std::shared_ptr<CommandList>>& command_lists)
{
  std::vector<VkCommandBuffer> vk_command_lists;
  for (auto& command_list : command_lists)
  {
    if (!command_list)
      continue;
    decltype(auto) vk_command_list = command_list->As<VKCommandList>();
    vk_command_lists.emplace_back(vk_command_list.GetCommandList());
  }

  VkSubmitInfo submit_info = {};
  submit_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = (ezUInt32)vk_command_lists.size();
  submit_info.pCommandBuffers = vk_command_lists.data();

  VkPipelineStageFlags wait_dst_stage_mask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
  submit_info.pWaitDstStageMask = &wait_dst_stage_mask;

  VkResult res = vkQueueSubmit(m_queue, 1, &submit_info, {});
}

VKDevice& VKCommandQueue::GetDevice()
{
  return m_device;
}

uint32_t VKCommandQueue::GetQueueFamilyIndex()
{
  return m_queue_family_index;
}

VkQueue VKCommandQueue::GetQueue()
{
  return m_queue;
}
