#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Descriptors/ResourceSetDescription.h>

class RHIResource;
class RHIGraphicsDevice;

namespace ValidationHelpers
{
  void ValidateResourceSet(RHIGraphicsDevice* graphicsDevice, const RHIResourceSetDescription& description);

  void ValidateResourceKind(ezEnum<RHIResourceKind> kind, RHIResource* resource, ezUInt32 slot);
} // namespace ValidationHelpers
