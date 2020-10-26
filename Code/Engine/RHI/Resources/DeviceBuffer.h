#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/DeviceResource.h>

/// <summary>
/// A device resource used to store arbitrary graphics data in various formats.
/// The size of a <see cref="RHIDeviceBuffer"/> is fixed upon creation, and resizing is not possible.
/// See <see cref="RHIBufferDescription"/>.
/// </summary>
class EZ_RHI_DLL RHIDeviceBuffer : public RHIDeviceResource
{
public:
  RHIDeviceBuffer()
  {
    Flags.Add(RHIDeviceResourceFlags::Bindable);
    Flags.Add(RHIDeviceResourceFlags::Mappable);
  }

  /// <summary>
  /// The total capacity, in bytes, of the buffer. This value is fixed upon creation.
  /// </summary>
  virtual ezUInt32 GetSize() const = 0;

  /// <summary>
  /// A bitmask indicating how this instance is permitted to be used.
  /// </summary>
  virtual ezBitflags<RHIBufferUsage> GetUsage() const = 0;
};
