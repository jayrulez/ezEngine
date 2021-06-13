#include <RHIPCH.h>

#include <RHI/Device/Device.h>

ezRHIDevice* ezRHIDevice::s_pDefaultDevice = nullptr;

ezRHIDevice::ezRHIDevice(const ezRHIDeviceDesc& desc)
  : m_Allocator("RHIDevice", ezFoundation::GetDefaultAllocator())
  , m_AllocatorWrapper(&m_Allocator)
  , m_Desc{desc}
{
}

ezRHIDevice::~ezRHIDevice() {

}
