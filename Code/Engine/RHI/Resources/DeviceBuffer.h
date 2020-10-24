#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <RHI/Resources/DeviceResource.h>

/// <summary>
/// Describes a <see cref="RHIDeviceBuffer"/>, used in the creation of <see cref="RHIDeviceBuffer"/> objects by a
/// <see cref="RHIResourceFactory"/>.
/// </summary>
struct EZ_RHI_DLL RHIBufferDescription : public ezHashableStruct<RHIBufferDescription>
{
  /// <summary>
  /// The desired capacity, in bytes, of the <see cref="RHIDeviceBuffer"/>.
  /// </summary>
  ezUInt32 Size;

  /// <summary>
  /// Indicates how the <see cref="RHIDeviceBuffer"/> will be used.
  /// </summary>
  ezBitflags<RHIBufferUsage> Usage;

  /// <summary>
  /// For structured buffers, this value indicates the size in bytes of a single structure element, and must be non-zero.
  /// For all other buffer types, this value must be zero.
  /// </summary>
  ezUInt32 StructureByteStride;

  /// <summary>
  /// Indicates that this is a raw buffer. This should be combined with
  /// <see cref="BufferUsage.StructuredBufferReadWrite"/>. This affects how the buffer is bound in the D3D11 backend.
  /// </summary>
  bool RawBuffer;

  /// <summary>
  /// Constructs a new <see cref="RHIBufferDescription"/> describing a non-dynamic <see cref="RHIDeviceBuffer"/>.
  /// </summary>
  /// <param name="size">The desired capacity, in bytes.</param>
  /// <param name="usage">Indicates how the <see cref="RHIDeviceBuffer"/> will be used.</param>
  RHIBufferDescription(ezUInt32 size, ezBitflags<RHIBufferUsage> usage)
  {
    Size = size;
    Usage = usage;
    StructureByteStride = 0;
    RawBuffer = false;
  }

  /// <summary>
  /// Constructs a new <see cref="BufferDescription"/>.
  /// </summary>
  /// <param name="sizeInBytes">The desired capacity, in bytes.</param>
  /// <param name="usage">Indicates how the <see cref="RHIDeviceBuffer"/> will be used.</param>
  /// <param name="structureByteStride">For structured buffers, this value indicates the size in bytes of a single
  /// structure element, and must be non-zero. For all other buffer types, this value must be zero.</param>
  RHIBufferDescription(ezUInt32 size, ezBitflags<RHIBufferUsage> usage, ezUInt32 structureByteStride)
  {
    Size = size;
    Usage = usage;
    StructureByteStride = structureByteStride;
    RawBuffer = false;
  }

  /// <summary>
  /// Constructs a new <see cref="RHIBufferDescription"/>.
  /// </summary>
  /// <param name="sizeInBytes">The desired capacity, in bytes.</param>
  /// <param name="usage">Indicates how the <see cref="RHIDeviceBuffer"/> will be used.</param>
  /// <param name="structureByteStride">For structured buffers, this value indicates the size in bytes of a single
  /// structure element, and must be non-zero. For all other buffer types, this value must be zero.</param>
  /// <param name="rawBuffer">Indicates that this is a raw buffer. This should be combined with
  /// <see cref="RHIBufferUsage.StructuredBufferReadWrite"/>. This affects how the buffer is bound in the D3D11 backend.
  /// </param>
  RHIBufferDescription(ezUInt32 size, ezBitflags<RHIBufferUsage> usage, ezUInt32 structureByteStride, bool rawBuffer)
  {
    Size = size;
    Usage = usage;
    StructureByteStride = structureByteStride;
    RawBuffer = rawBuffer;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIBufferDescription& other) const
  {
    return Size == other.Size && Usage == other.Usage && StructureByteStride == other.StructureByteStride && RawBuffer == other.RawBuffer;
  }
};

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
