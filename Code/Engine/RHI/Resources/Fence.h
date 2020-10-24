#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/DeviceResource.h>

// A GPU-CPU sync point
/// <summary>
/// A synchronization primitive which allows the GPU to communicate when submitted work items have finished executing.
/// </summary>
class EZ_RHI_DLL RHIFence : public RHIDeviceResource
{
public:
  /// <summary>
  /// Gets a value indicating whether the Fence is currently signaled. A Fence is signaled after a CommandList finishes
  /// execution after it was submitted with a Fence instance.
  /// </summary>
  virtual bool GetSignaled() = 0;

  /// <summary>
  /// Sets this instance to the unsignaled state.
  /// </summary>
  virtual bool Reset() = 0;
};