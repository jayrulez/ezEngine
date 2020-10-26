#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/DeviceResource.h>

/// <summary>
/// A bindable device resource which controls how texture values are sampled within a shader.
/// See <see cref="SamplerDescription"/>.
/// </summary>
class EZ_RHI_DLL RHISampler : public RHIDeviceResource
{
public:
  RHISampler()
  {
    Flags.Add(RHIDeviceResourceFlags::Bindable);
  }
};
