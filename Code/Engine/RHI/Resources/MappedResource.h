#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <optional>

#include <RHI/Resources/DeviceResource.h>

/// <summary>
/// A structure describing the layout of a mapped <see cref="RHIDeviceResource"/> object.
/// </summary>
struct RHIMappedResource
{
  /// <summary>
  /// The resource which has been mapped.
  /// </summary>
  RHIDeviceResource* Resource = nullptr;

  /// <summary>
  /// Identifies the <see cref="MapMode"/> that was used to map the resource.
  /// </summary>
  ezEnum<RHIMapMode> Mode = RHIMapMode::Default;

  /// <summary>
  /// A pointer to the start of the mapped data region.
  /// </summary>
  ezUInt64* Data = nullptr; /*std::intptr_t*/

  /// <summary>
  /// The total size, in bytes, of the mapped data region.
  /// </summary>
  ezUInt32 Size = 0;

  /// <summary>
  /// For mapped <see cref="Texture"/> resources, this is the subresource which is mapped.
  /// For <see cref="DeviceBuffer"/> resources, this field has no meaning.
  /// </summary>
  ezUInt32 Subresource = 0;

  /// <summary>
  /// For mapped <see cref="Texture"/> resources, this is the number of bytes between each row of texels.
  /// For <see cref="DeviceBuffer"/> resources, this field has no meaning.
  /// </summary>
  ezUInt32 RowPitch = 0;

  /// <summary>
  /// For mapped <see cref="Texture"/> resources, this is the number of bytes between each depth slice of a 3D Texture.
  /// For <see cref="DeviceBuffer"/> resources or 2D Textures, this field has no meaning.
  /// </summary>
  ezUInt32 DepthPitch = 0;

  RHIMappedResource(
    RHIDeviceResource* resource,
    ezEnum<RHIMapMode> mode,
    ezUInt64* data,
    ezUInt32 size,
    ezUInt32 subresource,
    ezUInt32 rowPitch,
    ezUInt32 depthPitch)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    EZ_VERIFY(resource != nullptr, "Resource is null.");
    EZ_VERIFY(resource->GetFlags().IsSet(RHIDeviceResourceFlags::Mappable), "Resource is not mappable.");
#endif

    Resource = resource;
    Mode = mode;
    Data = data;
    Size = size;
    Subresource = subresource;
    RowPitch = rowPitch;
    DepthPitch = depthPitch;
  }

  RHIMappedResource(RHIDeviceResource* resource, ezEnum<const RHIMapMode> mode, ezUInt64* data, ezUInt32 size)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    EZ_VERIFY(resource != nullptr, "Resource is null.");
    EZ_VERIFY(resource->GetFlags().IsSet(RHIDeviceResourceFlags::Mappable), "Resource is not mappable.");
#endif

    Resource = resource;
    Mode = mode;
    Data = data;
    Size = size;
    Subresource = 0;
    RowPitch = 0;
    DepthPitch = 0;
  }

  RHIMappedResource() {}
};

struct RHIMappedResourceInfo
{
  ezInt32 RefCount = 0;
  ezEnum<RHIMapMode> Mode = RHIMapMode::Default;
  RHIMappedResource* Resource = nullptr;
};

struct RHIMappedResourceCacheKey : public ezHashableStruct<RHIMappedResourceCacheKey>
{
  RHIDeviceResource* Resource = nullptr;
  ezUInt32 Subresource = 0;

  RHIMappedResourceCacheKey()
    : Resource(nullptr)
    , Subresource(0)
  {
  }

  RHIMappedResourceCacheKey(RHIDeviceResource* resource, ezUInt32 subresource)
  {
    Resource = resource;
    Subresource = subresource;
  }

  bool operator==(const RHIMappedResourceCacheKey& other) const
  {
    return Resource == other.Resource && Subresource == other.Subresource;
  }
};

/// <summary>
/// A typed view of a <see cref="RHIMappedResource"/>. Provides by-reference structured access to individual elements in the
/// mapped resource.
/// </summary>
/// <typeparam name="T">The blittable value type which mapped data is viewed as.</typeparam>
template <typename T>
class EZ_RHI_DLL RHIMappedResourceView
{
private:
  static const TSize = sizeof(T);

public:
  RHIMappedResourceView(RHIMappedResource* rawResource)
  {
    Resource = rawResource;
    Size = rawResource->Size;
    Count = Size / TSize;
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

    ezUInt8* ptr = Resource->Data + (index * TSize);

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

    ezUInt8* ptr = Resource->Data + (index * TSize);

    return reinterpret_cast<T*>(ptr);
  }

  /// <summary>
  /// Gets a reference to the structure at the given 2-dimensional texture coordinates.
  /// </summary>
  /// <param name="coord">ezVec2I32 coordinate vector.</param>
  /// <returns>A reference to the value at the given coordinates.</returns>
  T* operator[](ezVec2I32 coord)
  {
    ezUInt8* ptr = Resource->Data + (coord.y * Resource->RowPitch) + (coord.x * TSize);

    return reinterpret_cast<T*>(ptr);
  }

  /// <summary>
  /// Gets a reference to the structure at the given 2-dimensional texture coordinates.
  /// </summary>
  /// <param name="coord">ezVec2U32 coordinate vector.</param>
  /// <returns>A reference to the value at the given coordinates.</returns>
  T* operator[](ezVec2U32 coord)
  {
    ezUInt8* ptr = Resource->Data + (coord.y * Resource->RowPitch) + (coord.x * TSize);

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
                   (coord.x * TSize);

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
                   (coord.x * TSize);

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
