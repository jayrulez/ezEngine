#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/ResourceSetDescription.h>

#include <RHI/Resources/Resource.h>
#include <RHI/Resources/ResourceLayout.h>

/// <summary>
/// A device resource used to bind a particular set of <see cref="BindableResource"/> objects to a <see cref="CommandList"/>.
/// See <see cref="ResourceSetDescription"/>.
/// </summary>
class EZ_RHI_DLL RHIResourceSet : public RHIResource
{
public:
  RHIResourceSet(const RHIResourceSetDescription& description)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    Layout = description.Layout;
    Resources = description.BoundResources;
#endif
  }

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  RHIResourceLayout* GetLayout() const
  {
    return Layout;
  }

  ezDynamicArray<RHIResource*> GetResources() const
  {
    return Resources;
  }
#endif

private:
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  RHIResourceLayout* Layout;
  ezDynamicArray<RHIResource*> Resources;
#endif
};


