#include <RHI/Resources/MappedResourceCacheKey.h>

ezUInt32 ezHashHelper<RHIMappedResourceCacheKey>::Hash(const RHIMappedResourceCacheKey& value)
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

EZ_STATICLINK_FILE(RHI, RHI_Resources_Implementation_MappedResourceCacheKey);

