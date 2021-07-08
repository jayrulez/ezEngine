#pragma once
#include <RHI/Instance/Instance.h>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKInstance : public Instance
{
public:
  VKInstance();
  std::vector<std::shared_ptr<Adapter>> EnumerateAdapters() override;
  VkInstance& GetInstance();

private:
  //vk::DynamicLoader m_dl;
  VkInstance m_instance;
  VkDebugReportCallbackEXT m_callback;
};
