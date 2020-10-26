#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/ResourceLayoutElementDescription.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// Describes the layout of <see cref="RHIResourseFlags::Bindable"/> objects for a <see cref="RHIPipeline"/>.
/// </summary>
struct EZ_RHI_DLL RHIResourceLayoutDescription : public ezHashableStruct<RHIResourceLayoutDescription>
{
  /// <summary>
  /// An array of <see cref="RHIResourceLayoutElementDescription"/> objects, describing the properties of each resource
  /// element in the <see cref="RHIResourceLayout"/>.
  /// </summary>
  ezDynamicArray<RHIResourceLayoutElementDescription> Elements;

  RHIResourceLayoutDescription() = default;

  /// <summary>
  /// Constructs a new ResourceLayoutDescription.
  /// </summary>
  /// <param name="elements">An array of <see cref="RHIResourceLayoutElementDescription"/> objects, describing the properties
  /// of each resource element in the <see cref="RHIResourceLayout"/>.</param>
  RHIResourceLayoutDescription(ezDynamicArray<RHIResourceLayoutElementDescription>& elements)
  {
    Elements = elements;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIResourceLayoutDescription& other) const
  {
    return Elements == other.Elements;
  }
};
