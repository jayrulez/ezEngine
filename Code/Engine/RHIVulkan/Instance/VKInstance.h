#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/Instance/Instance.h>

class ezRHIVKInstance : public ezRHIInstance
{
public:
  ezRHIVKInstance();
  ezDynamicArray<ezRHIAdapter*> EnumerateAdapters() override;
  vk::Instance& GetInstance();

private:
  vk::DynamicLoader m_DynamicLoader;
  vk::UniqueInstance m_Instance;
  vk::UniqueDebugReportCallbackEXT m_Callback;
};
