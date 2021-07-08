#include <RHIVulkanPCH.h>

#include <RHIVulkan/Adapter/VKAdapter.h>

#include <RHIVulkan/Device/VKDevice.h>

ezRHIVKAdapter::ezRHIVKAdapter(ezRHIVKInstance& instance, const vk::PhysicalDevice& physicalDevice)
  : m_Instance{instance}
  , m_PhysicalDevice{physicalDevice}
  , m_Name{physicalDevice.getProperties().deviceName.data()}
{
}

const ezString& ezRHIVKAdapter::GetNamePlatform() const
{
  return m_Name;
}

ezRHIDevice* ezRHIVKAdapter::CreateDevicePlatform()
{
  return EZ_DEFAULT_NEW(ezRHIVKDevice, *this);
}

ezRHIVKInstance& ezRHIVKAdapter::GetInstance()
{
  return m_Instance;
}

vk::PhysicalDevice& ezRHIVKAdapter::GetPhysicalDevice()
{
  return m_PhysicalDevice;
}
