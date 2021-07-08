#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>
#include <RHI/Adapter/Adapter.h>

class VKInstance;

class VKAdapter : public Adapter
{
public:
  VKAdapter(VKInstance& instance, VkPhysicalDevice physical_device);
  const std::string& GetName() const override;
  std::shared_ptr<Device> CreateDevice() override;
  VKInstance& GetInstance();
  VkPhysicalDevice& GetPhysicalDevice();

private:
  VKInstance& m_instance;
  VkPhysicalDevice m_physical_device;
  std::string m_name;
};
