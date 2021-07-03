#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/Device/VKDevice.h>

VKAdapter::VKAdapter(VKInstance& instance, const vk::PhysicalDevice& physical_device)
    : m_instance(instance)
    , m_physical_device(physical_device)
    , m_name(physical_device.getProperties().deviceName.data())
{
}

const std::string& VKAdapter::GetName() const
{
    return m_name;
}

ezSharedPtr<Device> VKAdapter::CreateDevice()
{
  return EZ_DEFAULT_NEW(VKDevice, *this);
}

VKInstance& VKAdapter::GetInstance()
{
    return m_instance;
}

vk::PhysicalDevice& VKAdapter::GetPhysicalDevice()
{
    return m_physical_device;
}
