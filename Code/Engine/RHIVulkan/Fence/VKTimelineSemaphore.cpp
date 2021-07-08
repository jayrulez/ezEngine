#include <RHIVulkanPCH.h>

#include <RHIVulkan/Fence/VKTimelineSemaphore.h>
#include <RHIVulkan/Device/VKDevice.h>

ezRHIVKTimelineSemaphore::ezRHIVKTimelineSemaphore(ezRHIVKDevice& device, ezUInt64 initialValue)
  : m_Device{device}
{
  vk::SemaphoreTypeCreateInfo timelineCreateInfo = {};
  timelineCreateInfo.initialValue = initialValue;
  timelineCreateInfo.semaphoreType = vk::SemaphoreType::eTimeline;
  vk::SemaphoreCreateInfo create_info;
  create_info.pNext = &timelineCreateInfo;
  m_TimelineSemaphore = device.GetDevice().createSemaphoreUnique(create_info);
}

ezUInt64 ezRHIVKTimelineSemaphore::GetCompletedValue()
{
  return m_Device.GetDevice().getSemaphoreCounterValueKHR(m_TimelineSemaphore.get());
}

void ezRHIVKTimelineSemaphore::Wait(ezUInt64 value)
{
  vk::SemaphoreWaitInfo waitInfo = {};
  waitInfo.semaphoreCount = 1;
  waitInfo.pSemaphores = &m_TimelineSemaphore.get();
  waitInfo.pValues = &value;
  vk::Result result = m_Device.GetDevice().waitSemaphoresKHR(waitInfo, UINT64_MAX);
}

void ezRHIVKTimelineSemaphore::Signal(ezUInt64 value)
{
  vk::SemaphoreSignalInfo signalInfo = {};
  signalInfo.semaphore = m_TimelineSemaphore.get();
  signalInfo.value = value;
  m_Device.GetDevice().signalSemaphoreKHR(signalInfo);
}

const vk::Semaphore& ezRHIVKTimelineSemaphore::GetFence() const
{
  return m_TimelineSemaphore.get();
}
