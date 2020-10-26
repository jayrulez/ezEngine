#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/MappedResource.h>

/// <summary>
/// A typed view of a <see cref="RHIMappedResource"/>. Provides by-reference structured access to individual elements in the
/// mapped resource.
/// </summary>
/// <typeparam name="T">The blittable value type which mapped data is viewed as.</typeparam>
template <typename T>
class EZ_RHI_DLL RHIMappedResourceView
{
public:
  RHIMappedResourceView(RHIMappedResource* rawResource)
  {
    Resource = rawResource;
    Size = rawResource->Size;
    Count = Size / sizeof(T);
  }

  RHIMappedResource* GetMappedResource() const
  {
    return Resource;
  }

  ezUInt32 GetSize() const
  {
    return Size;
  }

  ezUInt32 GetCount() const
  {
    return Count;
  }

  /// <summary>
  /// Gets a reference to the structure value at the given index.
  /// </summary>
  /// <param name="index">The index of the value.</param>
  /// <returns>A reference to the value at the given index.</returns>
  T* operator[](ezInt32 index)
  {
    if (index >= Count || index < 0)
    {
      EZ_REPORT_FAILURE("Given index {} must be non-negative and less than Count {}.", index, Count);
    }

    ezUInt8* ptr = Resource->Data + (index * sizeof(T));

    return reinterpret_cast<T*>(ptr);
  }

  /// <summary>
  /// Gets a reference to the structure value at the given index.
  /// </summary>
  /// <param name="index">The index of the value.</param>
  /// <returns>A reference to the value at the given index.</returns>
  T* operator[](ezUInt32 index)
  {
    if (index >= Count)
    {
      EZ_REPORT_FAILURE("Given index {} must be  less than Count {}.", Count);
    }

    ezUInt8* ptr = Resource->Data + (index * sizeof(T));

    return reinterpret_cast<T*>(ptr);
  }

  /// <summary>
  /// Gets a reference to the structure at the given 2-dimensional texture coordinates.
  /// </summary>
  /// <param name="coord">ezVec2I32 coordinate vector.</param>
  /// <returns>A reference to the value at the given coordinates.</returns>
  T* operator[](ezVec2I32 coord)
  {
    ezUInt8* ptr = Resource->Data + (coord.y * Resource->RowPitch) + (coord.x * sizeof(T));

    return reinterpret_cast<T*>(ptr);
  }

  /// <summary>
  /// Gets a reference to the structure at the given 2-dimensional texture coordinates.
  /// </summary>
  /// <param name="coord">ezVec2U32 coordinate vector.</param>
  /// <returns>A reference to the value at the given coordinates.</returns>
  T* operator[](ezVec2U32 coord)
  {
    ezUInt8* ptr = Resource->Data + (coord.y * Resource->RowPitch) + (coord.x * sizeof(T));

    return reinterpret_cast<T*>(ptr);
  }

  /// <summary>
  /// Gets a reference to the structure at the given 3-dimensional texture coordinates.
  /// </summary>
  /// <param name="coord">ezVec3I32 coordinate vector.</param>
  /// <returns>A reference to the value at the given coordinates.</returns>
  T* operator[](ezVec3I32 coord)
  {
    ezUInt8* ptr = Resource->Data +
                   (coord.z * Resource->DepthPitch) +
                   (coord.y * Resource->RowPitch) +
                   (coord.x * sizeof(T));

    return reinterpret_cast<T*>(ptr);
  }

  /// <summary>
  /// Gets a reference to the structure at the given 3-dimensional texture coordinates.
  /// </summary>
  /// <param name="coord">ezVec3U32 coordinate vector.</param>
  /// <returns>A reference to the value at the given coordinates.</returns>
  T* operator[](ezVec3U32 coord)
  {
    ezUInt8* ptr = Resource->Data +
                   (coord.z * Resource->DepthPitch) +
                   (coord.y * Resource->RowPitch) +
                   (coord.x * sizeof(T));

    return reinterpret_cast<T*>(ptr);
  }

private:
  /// <summary>
  /// The <see cref="MappedResource"/> that this instance views.
  /// </summary>
  RHIMappedResource* Resource = nullptr;

  /// <summary>
  /// The total size in bytes of the mapped resource.
  /// </summary>
  ezUInt32 Size = 0;

  /// <summary>
  /// Constructs a new MappedResourceView which wraps the given <see cref="MappedResource"/>.
  /// </summary>
  /// <param name="rawResource">The raw resource which has been mapped.</param>
  ezUInt32 Count = 0;
};
