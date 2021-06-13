#include <RHI/Device/Device.h>

ezRHIDevice::ezRHIDevice(const ezRHIDeviceDesc& desc)
  : m_Allocator("RHIDevice", ezFoundation::GetDefaultAllocator())
  , m_AllocatorWrapper(&m_Allocator)
  ,m_Desc{desc}
{
}
