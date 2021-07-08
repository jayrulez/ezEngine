#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>
#include <RHI/Fence/Fence.h>

class ezRHIVKTimelineSemaphore : public ezRHIFence
{
public:
  ezRHIVKTimelineSemaphore(ezRHIVKDevice& device, ezUInt64 initialValue);
  ezUInt64 GetCompletedValue() override;
  void Wait(ezUInt64 value) override;
  void Signal(ezUInt64 value) override;

  const vk::Semaphore& GetFence() const;

private:
  ezRHIVKDevice& m_Device;
  vk::UniqueSemaphore m_TimelineSemaphore;
};
