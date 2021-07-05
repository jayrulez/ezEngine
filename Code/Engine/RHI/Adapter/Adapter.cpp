#include <RHIPCH.h>

#include <RHI/Adapter/Adapter.h>

const ezString& ezRHIAdapter::GetName() const
{
  return GetNamePlatform();
}

ezRHIDevice* ezRHIAdapter::CreateDevice()
{
  return CreateDevicePlatform();
}
