#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Resource.h>

/// <summary>
/// A device resource used to store arbitrary graphics data in various formats.
/// The size of a <see cref="RHIBuffer"/> is fixed upon creation, and resizing is not possible.
/// See <see cref="RHIBufferDescription"/>.
/// </summary>
class EZ_RHI_DLL RHIBuffer : public RHIResource
{
public:
  RHIBuffer()
  {
    Flags.Add(RHIResourceFlags::Bindable);
    Flags.Add(RHIResourceFlags::Mappable);
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
