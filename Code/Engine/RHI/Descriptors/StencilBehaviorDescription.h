#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>


/// <summary>
/// Describes how stencil tests are performed in a <see cref="RHIPipeline"/>'s depth-stencil state.
/// </summary>
struct EZ_RHI_DLL RHIStencilBehaviorDescription : public ezHashableStruct<RHIStencilBehaviorDescription>
{
  /// <summary>
  /// The operation performed on samples that fail the stencil test.
  /// </summary>
  ezEnum<RHIStencilOperation> Fail;
  /// <summary>
  /// The operation performed on samples that pass the stencil test.
  /// </summary>
  ezEnum<RHIStencilOperation> Pass;
  /// <summary>
  /// The operation performed on samples that pass the stencil test but fail the depth test.
  /// </summary>
  ezEnum<RHIStencilOperation> DepthFail;
  /// <summary>
  /// The comparison operator used in the stencil test.
  /// </summary>
  ezEnum<RHIComparisonKind> Comparison;

  RHIStencilBehaviorDescription() = default;

  /// <summary>
  /// Constructs a new StencilBehaviorDescription.
  /// </summary>
  /// <param name="fail">The operation performed on samples that fail the stencil test.</param>
  /// <param name="pass">The operation performed on samples that pass the stencil test.</param>
  /// <param name="depthFail">The operation performed on samples that pass the stencil test but fail the depth test.</param>
  /// <param name="comparison">The comparison operator used in the stencil test.</param>
  RHIStencilBehaviorDescription(
    ezEnum<RHIStencilOperation> fail,
    ezEnum<RHIStencilOperation> pass,
    ezEnum<RHIStencilOperation> depthFail,
    ezEnum<RHIComparisonKind> comparison)
  {
    Fail = fail;
    Pass = pass;
    DepthFail = depthFail;
    Comparison = comparison;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIStencilBehaviorDescription& other) const
  {
    return Fail == other.Fail &&
           Pass == other.Pass &&
           DepthFail == other.DepthFail &&
           Comparison == other.Comparison;
  }
};
