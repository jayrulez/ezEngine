#pragma once

#include <RHI/Descriptors/ResourceLayoutDescription.h>
#include <RHI/Descriptors/ResourceLayoutElementDescription.h>
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Resource.h>
#include <RHI/ValidationHelpers.h>

/// <summary>
/// A device resource which describes the layout and kind of <see cref="RHIBindableResource"/> objects available
/// to a shader set.
/// See <see cref="RHIResourceLayoutDescription"/>.
/// </summary>
class RHIResourceLayout : public RHIResource
{
private:
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  RHIResourceLayoutDescription Description;
  ezUInt32 DynamicBufferCount = 0;
#endif

protected:
  friend class RHICommandList;

  friend void ValidationHelpers::ValidateResourceSet(RHIGraphicsDevice* graphicsDevice, const RHIResourceSetDescription& description);

  RHIResourceLayout(const RHIResourceLayoutDescription& description)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    Description = description;
    for (RHIResourceLayoutElementDescription element : description.Elements)
    {
      if ((element.Options & RHIResourceLayoutElementOptions::DynamicBinding) != 0)
      {
        DynamicBufferCount += 1;
      }
    }
#endif
  }

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  const RHIResourceLayoutDescription& GetDescription() const
  {
    return Description;
  }

  ezUInt32 GetDynamicBufferCount() const
  {
    return DynamicBufferCount;
  }
#endif
};
