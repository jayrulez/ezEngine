#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/Adapter/Adapter.h>

class ezRHIVKAdapter : public ezRHIAdapter
{
public:
  ezRHIVKAdapter(ezRHIVKInstance& instance, const vk::PhysicalDevice& physicalDevice);
  const ezString& GetNamePlatform() const override;
  ezRHIDevice* CreateDevicePlatform() override;
  ezRHIVKInstance& GetInstance();
  vk::PhysicalDevice& GetPhysicalDevice();

private:
  ezRHIVKInstance& m_Instance;
  vk::PhysicalDevice m_PhysicalDevice;
  ezString m_Name;
};
