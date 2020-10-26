#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Resources/DeviceResource.h>

#include <Foundation/Algorithm/HashableStruct.h>

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
