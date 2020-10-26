#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// Describes an individual resource element in a <see cref="RHIResourceLayout"/>.
/// </summary>
struct EZ_RHI_DLL RHIResourceLayoutElementDescription : public ezHashableStruct<RHIResourceLayoutElementDescription>
{
  /// <summary>
  /// The name of the element.
  /// </summary>
  ezString Name;

  /// <summary>
  /// The kind of resource.
  /// </summary>
  ezEnum<RHIResourceKind> Kind;

  /// <summary>
  /// The <see cref="RHIShaderStages"/> in which this element is used.
  /// </summary>
  ezBitflags<RHIShaderStages> Stages;

  /// <summary>
  /// Miscellaneous resource options for this element.
  /// </summary>
  ezBitflags<RHIResourceLayoutElementOptions> Options;

  /// <summary>
  /// Constructs a new ResourceLayoutElementDescription.
  /// </summary>
  /// <param name="name">The name of the element.</param>
  /// <param name="kind">The kind of resource.</param>
  /// <param name="stages">The <see cref="RHIShaderStages"/> in which this element is used.</param>
  RHIResourceLayoutElementDescription(ezString name, ezEnum<RHIResourceKind> kind, ezBitflags<RHIShaderStages> stages)
  {
    Name = name;
    Kind = kind;
    Stages = stages;
    Options = RHIResourceLayoutElementOptions::None;
  }

  /// <summary>
  /// Constructs a new ResourceLayoutElementDescription.
  /// </summary>
  /// <param name="name">The name of the element.</param>
  /// <param name="kind">The kind of resource.</param>
  /// <param name="stages">The <see cref="RHIShaderStages"/> in which this element is used.</param>
  /// <param name="options">Miscellaneous resource options for this element.</param>
  RHIResourceLayoutElementDescription(
    ezString name,
    ezEnum<RHIResourceKind> kind,
    ezBitflags<RHIShaderStages> stages,
    ezBitflags<RHIResourceLayoutElementOptions> options)
  {
    Name = name;
    Kind = kind;
    Stages = stages;
    Options = options;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIResourceLayoutElementDescription& other) const
  {
    return Name.IsEqual(other.Name) && Kind == other.Kind && Stages == other.Stages && Options == other.Options;
  }
};