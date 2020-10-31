#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/StencilBehaviorDescription.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// A <see cref="RHIPipeline"/> component describing the properties of the depth stencil state.
/// </summary>
struct EZ_RHI_DLL RHIDepthStencilStateDescription : public ezHashableStruct<RHIDepthStencilStateDescription>
{
  /// <summary>
  /// Controls whether depth testing is enabled.
  /// </summary>
  bool DepthTestEnabled = false;
  /// <summary>
  /// Controls whether new depth values are written to the depth buffer.
  /// </summary>
  bool DepthWriteEnabled = false;
  /// <summary>
  /// The <see cref="RHIComparisonKind"/> used when considering new depth values.
  /// </summary>
  ezEnum<RHIComparisonKind> DepthComparison = RHIComparisonKind::Never;

  /// <summary>
  /// Controls whether the stencil test is enabled.
  /// </summary>
  bool StencilTestEnabled = false;
  /// <summary>
  /// Controls how stencil tests are handled for pixels whose surface faces towards the camera.
  /// </summary>
  RHIStencilBehaviorDescription StencilFront;
  /// <summary>
  /// Controls how stencil tests are handled for pixels whose surface faces away from the camera.
  /// </summary>
  RHIStencilBehaviorDescription StencilBack;
  /// <summary>
  /// Controls the portion of the stencil buffer used for reading.
  /// </summary>
  ezUInt8 StencilReadMask = 0;
  /// <summary>
  /// Controls the portion of the stencil buffer used for writing.
  /// </summary>
  ezUInt8 StencilWriteMask = 0;
  /// <summary>
  /// The reference value to use when doing a stencil test.
  /// </summary>
  ezUInt32 StencilReference = 0;

  RHIDepthStencilStateDescription() = default;

  /// <summary>
  /// Constructs a new <see cref="DepthStencilStateDescription"/>. This describes a depth-stencil state with no stencil
  /// testing enabled.
  /// </summary>
  /// <param name="depthTestEnabled">Controls whether depth testing is enabled.</param>
  /// <param name="depthWriteEnabled">Controls whether new depth values are written to the depth buffer.</param>
  /// <param name="comparisonKind">The <see cref="RHIComparisonKind"/> used when considering new depth values.</param>
  RHIDepthStencilStateDescription(bool depthTestEnabled, bool depthWriteEnabled, ezEnum<RHIComparisonKind> comparisonKind)
  {
    DepthTestEnabled = depthTestEnabled;
    DepthWriteEnabled = depthWriteEnabled;
    DepthComparison = comparisonKind;

    StencilTestEnabled = false;
    StencilFront = RHIStencilBehaviorDescription();
    StencilBack = RHIStencilBehaviorDescription();
    StencilReadMask = 0;
    StencilWriteMask = 0;
    StencilReference = 0;
  }

  /// <summary>
  /// Constructs a new <see cref="RHIDepthStencilStateDescription"/>. This describes a depth-stencil state with no stencil
  /// testing enabled.
  /// </summary>
  /// <param name="depthTestEnabled">Controls whether depth testing is enabled.</param>
  /// <param name="depthWriteEnabled">Controls whether new depth values are written to the depth buffer.</param>
  /// <param name="comparisonKind">The <see cref="RHIComparisonKind"/> used when considering new depth values.</param>
  /// <param name="stencilTestEnabled">Controls whether the stencil test is enabled.</param>
  /// <param name="stencilFront">Controls how stencil tests are handled for pixels whose surface faces towards the camera.</param>
  /// <param name="stencilBack">Controls how stencil tests are handled for pixels whose surface faces away from the camera.</param>
  /// <param name="stencilReadMask">Controls the portion of the stencil buffer used for reading.</param>
  /// <param name="stencilWriteMask">Controls the portion of the stencil buffer used for writing.</param>
  /// <param name="stencilReference">The reference value to use when doing a stencil test.</param>
  RHIDepthStencilStateDescription(
    bool depthTestEnabled,
    bool depthWriteEnabled,
    ezEnum<RHIComparisonKind> comparisonKind,
    bool stencilTestEnabled,
    RHIStencilBehaviorDescription stencilFront,
    RHIStencilBehaviorDescription stencilBack,
    ezUInt8 stencilReadMask,
    ezUInt8 stencilWriteMask,
    ezUInt32 stencilReference)
  {
    DepthTestEnabled = depthTestEnabled;
    DepthWriteEnabled = depthWriteEnabled;
    DepthComparison = comparisonKind;

    StencilTestEnabled = stencilTestEnabled;
    StencilFront = stencilFront;
    StencilBack = stencilBack;
    StencilReadMask = stencilReadMask;
    StencilWriteMask = stencilWriteMask;
    StencilReference = stencilReference;
  }

  /// <summary>
  /// Describes a depth-only depth stencil state which uses a <see cref="RHIComparisonKind.LessEqual"/> comparison.
  /// The stencil test is disabled.
  /// Settings:
  ///     DepthTestEnabled = true
  ///     DepthWriteEnabled = true
  ///     ComparisonKind = DepthComparisonKind.LessEqual
  /// </summary>
  static const RHIDepthStencilStateDescription DepthOnlyLessEqual;

  /// <summary>
  /// Describes a depth-only depth stencil state which uses a <see cref="RHIComparisonKind.LessEqual"/> comparison, and disables writing to the depth buffer.
  /// The stencil test is disabled.
  /// Settings:
  ///     DepthTestEnabled = true
  ///     DepthWriteEnabled = false
  ///     ComparisonKind = DepthComparisonKind.LessEqual
  /// </summary>
  static const RHIDepthStencilStateDescription DepthOnlyLessEqualRead;

  /// <summary>
  /// Describes a depth-only depth stencil state which uses a <see cref="RHIComparisonKind.GreaterEqual"/> comparison.
  /// The stencil test is disabled.
  /// Settings:
  ///     DepthTestEnabled = true
  ///     DepthWriteEnabled = true
  ///     ComparisonKind = DepthComparisonKind.GreaterEqual
  /// </summary>
  static const RHIDepthStencilStateDescription DepthOnlyGreaterEqual;

  /// <summary>
  /// Describes a depth-only depth stencil state which uses a <see cref="RHIComparisonKind.GreaterEqual"/> comparison, and
  /// disables writing to the depth buffer. The stencil test is disabled.
  /// Settings:
  ///     DepthTestEnabled = true
  ///     DepthWriteEnabled = false
  ///     ComparisonKind = DepthComparisonKind.GreaterEqual
  /// </summary>
  static const RHIDepthStencilStateDescription DepthOnlyGreaterEqualRead;

  /// <summary>
  /// Describes a depth-only depth stencil state in which depth testing and writing is disabled.
  /// The stencil test is disabled.
  /// Settings:
  ///     DepthTestEnabled = false
  ///     DepthWriteEnabled = false
  ///     ComparisonKind = DepthComparisonKind.LessEqual
  /// </summary>
  static const RHIDepthStencilStateDescription Disabled;

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIDepthStencilStateDescription& other) const
  {
    return DepthTestEnabled == other.DepthTestEnabled && DepthWriteEnabled == other.DepthWriteEnabled && DepthComparison == other.DepthComparison && StencilTestEnabled == other.StencilTestEnabled && StencilFront == other.StencilFront && StencilBack == other.StencilBack && StencilReadMask == other.StencilReadMask && StencilWriteMask == other.StencilWriteMask && StencilReference == other.StencilReference;
  }
};

template <>
struct ezHashHelper<RHIDepthStencilStateDescription>
{
  EZ_ALWAYS_INLINE static ezUInt32 Hash(const RHIDepthStencilStateDescription& value) { return ezHashHelper<ezUInt64>::Hash(0); }

  EZ_ALWAYS_INLINE static bool Equal(const RHIDepthStencilStateDescription& a, const RHIDepthStencilStateDescription& b) { return a == b; }
};
