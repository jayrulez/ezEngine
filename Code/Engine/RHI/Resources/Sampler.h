#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Resource.h>

/// <summary>
/// A bindable device resource which controls how texture values are sampled within a shader.
/// See <see cref="SamplerDescription"/>.
/// </summary>
class EZ_RHI_DLL RHISampler : public RHIResource
{
public:
  RHISampler()
  {
    Flags.Add(RHIResourceFlags::Bindable);
  }
};
