#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Resources/Resource.h>

#include <Foundation/Algorithm/HashableStruct.h>

struct RHIMappedResourceCacheKey : public ezHashableStruct<RHIMappedResourceCacheKey>
{
  RHIResource* GetResource() const
  {
    return Resource;
  }

  ezUInt32 GetSubresource() const
  {
    return Subresource;
  }

  RHIMappedResourceCacheKey()
    : RHIMappedResourceCacheKey(nullptr, 0)
  {
  }

  RHIMappedResourceCacheKey(RHIResource* resource, ezUInt32 subresource)
  {
    Resource = resource;
    Subresource = subresource;
  }

  bool operator==(const RHIMappedResourceCacheKey& other) const
  {
    return Resource == other.Resource && Subresource == other.Subresource;
  }

private:
  RHIResource* Resource = nullptr;
  ezUInt32 Subresource = 0;
};

template <>
struct ezHashHelper<RHIMappedResourceCacheKey>
{
  EZ_ALWAYS_INLINE static ezUInt32 Hash(const RHIMappedResourceCacheKey& value)
  {
    ezUInt32 hash = 0;
    RHIResource* resource = value.GetResource();
    if (resource)
    {
      resource->GetHash();
    }

    ezUInt32 subresource = value.GetSubresource();
    hash = ezHashingUtils::xxHash32(&subresource, sizeof(subresource), hash);
    return hash;
  }

  EZ_ALWAYS_INLINE static bool Equal(const RHIMappedResourceCacheKey& a, const RHIMappedResourceCacheKey& b) { return a == b; }
};
