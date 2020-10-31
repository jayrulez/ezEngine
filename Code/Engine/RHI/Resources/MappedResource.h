#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Resource.h>

/// <summary>
/// A structure describing the layout of a mapped <see cref="RHIResource"/> object.
/// </summary>
struct RHIMappedResource
{
  /// <summary>
  /// The resource which has been mapped.
  /// </summary>
  RHIResource* Resource = nullptr;

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
    RHIResource* resource,
    ezEnum<RHIMapMode> mode,
    ezUInt64* data,
    ezUInt32 size,
    ezUInt32 subresource,
    ezUInt32 rowPitch,
    ezUInt32 depthPitch)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    EZ_VERIFY(resource != nullptr, "Resource is null.");
    EZ_VERIFY(resource->GetFlags().IsSet(RHIResourceFlags::Mappable), "Resource is not mappable.");
#endif

    Resource = resource;
    Mode = mode;
    Data = data;
    Size = size;
    Subresource = subresource;
    RowPitch = rowPitch;
    DepthPitch = depthPitch;
  }

  RHIMappedResource(RHIResource* resource, ezEnum<const RHIMapMode> mode, ezUInt64* data, ezUInt32 size)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    EZ_VERIFY(resource != nullptr, "Resource is null.");
    EZ_VERIFY(resource->GetFlags().IsSet(RHIResourceFlags::Mappable), "Resource is not mappable.");
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
