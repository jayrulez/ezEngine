#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>

VKTimelineSemaphore::VKTimelineSemaphore(VKDevice& device, uint64_t initial_value)
  : m_device(device)
{
  VkSemaphoreTypeCreateInfo timeline_create_info = {};
  timeline_create_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
  timeline_create_info.initialValue = initial_value;
  timeline_create_info.semaphoreType = VkSemaphoreType::VK_SEMAPHORE_TYPE_TIMELINE;
  VkSemaphoreCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  create_info.pNext = &timeline_create_info;
  vkCreateSemaphore(device.GetDevice(), &create_info, nullptr, &m_timeline_semaphore);
}

uint64_t VKTimelineSemaphore::GetCompletedValue()
{
  ezUInt64 ret = 0;
  vkGetSemaphoreCounterValueKHR(m_device.GetDevice(), m_timeline_semaphore, &ret);

  return ret;
}

void VKTimelineSemaphore::Wait(uint64_t value)
{
  VkSemaphoreWaitInfo wait_info = {};
  wait_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
  wait_info.semaphoreCount = 1;
  wait_info.pSemaphores = &m_timeline_semaphore;
  wait_info.pValues = &value;
  VkResult res = vkWaitSemaphoresKHR(m_device.GetDevice(), &wait_info, UINT64_MAX);
}

void VKTimelineSemaphore::Signal(uint64_t value)
{
  VkSemaphoreSignalInfo signal_info = {};
  signal_info.semaphore = m_timeline_semaphore;
  signal_info.value = value;
  vkSignalSemaphoreKHR(m_device.GetDevice(), &signal_info);
}

const VkSemaphore& VKTimelineSemaphore::GetFence() const
{
  return m_timeline_semaphore;
}
