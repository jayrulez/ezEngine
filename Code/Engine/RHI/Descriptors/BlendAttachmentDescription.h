#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// A <see cref="RHIPipeline"/> component describing the blend behavior for an individual color attachment.
/// </summary>
struct EZ_RHI_DLL RHIBlendAttachmentDescription : public ezHashableStruct<RHIBlendAttachmentDescription>
{
  EZ_DECLARE_POD_TYPE();

  /// <summary>
  /// Controls whether blending is enabled for the color attachment.
  /// </summary>
  bool BlendEnabled = false;
  /// <summary>
  /// Controls the source color's influence on the blend result.
  /// </summary>
  ezEnum<RHIBlendFactor> SourceColorFactor = RHIBlendFactor::Default;
  /// <summary>
  /// Controls the destination color's influence on the blend result.
  /// </summary>
  ezEnum<RHIBlendFactor> DestinationColorFactor = RHIBlendFactor::Default;
  /// <summary>
  /// Controls the function used to combine the source and destination color factors.
  /// </summary>
  ezEnum<RHIBlendFunction> ColorFunction = RHIBlendFunction::Default;
  /// <summary>
  /// Controls the source alpha's influence on the blend result.
  /// </summary>
  ezEnum<RHIBlendFactor> SourceAlphaFactor = RHIBlendFactor::Default;
  /// <summary>
  /// Controls the destination alpha's influence on the blend result.
  /// </summary>
  ezEnum<RHIBlendFactor> DestinationAlphaFactor = RHIBlendFactor::Default;
  /// <summary>
  /// Controls the function used to combine the source and destination alpha factors.
  /// </summary>
  ezEnum<RHIBlendFunction> AlphaFunction = RHIBlendFunction::Default;

  RHIBlendAttachmentDescription() = default;

  /// <summary>
  /// Constructs a new <see cref="RHIBlendAttachmentDescription"/>.
  /// </summary>
  /// <param name="blendEnabled">Controls whether blending is enabled for the color attachment.</param>
  /// <param name="sourceColorFactor">Controls the source color's influence on the blend result.</param>
  /// <param name="destinationColorFactor">Controls the destination color's influence on the blend result.</param>
  /// <param name="colorFunction">Controls the function used to combine the source and destination color factors.</param>
  /// <param name="sourceAlphaFactor">Controls the source alpha's influence on the blend result.</param>
  /// <param name="destinationAlphaFactor">Controls the destination alpha's influence on the blend result.</param>
  /// <param name="alphaFunction">Controls the function used to combine the source and destination alpha factors.</param>
  RHIBlendAttachmentDescription(
    bool blendEnabled,
    ezEnum<RHIBlendFactor> sourceColorFactor,
    ezEnum<RHIBlendFactor> destinationColorFactor,
    ezEnum<RHIBlendFunction> colorFunction,
    ezEnum<RHIBlendFactor> sourceAlphaFactor,
    ezEnum<RHIBlendFactor> destinationAlphaFactor,
    ezEnum<RHIBlendFunction> alphaFunction)
  {
    BlendEnabled = blendEnabled;
    SourceColorFactor = sourceColorFactor;
    DestinationColorFactor = destinationColorFactor;
    ColorFunction = colorFunction;
    SourceAlphaFactor = sourceAlphaFactor;
    DestinationAlphaFactor = destinationAlphaFactor;
    AlphaFunction = alphaFunction;
  }

  /// <summary>
  /// Describes a blend attachment state in which the source completely overrides the destination.
  /// Settings:
  ///     BlendEnabled = true
  ///     SourceColorFactor = RHIBlendFactor.One
  ///     DestinationColorFactor = RHIBlendFactor.Zero
  ///     ColorFunction = RHIBlendFunction.Add
  ///     SourceAlphaFactor = RHIBlendFactor.One
  ///     DestinationAlphaFactor = RHIBlendFactor.Zero
  ///     AlphaFunction = RHIBlendFunction.Add
  /// </summary>
  static const RHIBlendAttachmentDescription OverrideBlend;

  /// <summary>
  /// Describes a blend attachment state in which the source and destination are blended in an inverse relationship.
  /// Settings:
  ///     BlendEnabled = true
  ///     SourceColorFactor = RHIBlendFactor.SourceAlpha
  ///     DestinationColorFactor = RHIBlendFactor.InverseSourceAlpha
  ///     ColorFunction = RHIBlendFunction.Add
  ///     SourceAlphaFactor = RHIBlendFactor.SourceAlpha
  ///     DestinationAlphaFactor = RHIBlendFactor.InverseSourceAlpha
  ///     AlphaFunction = RHIBlendFunction.Add
  /// </summary>
  static const RHIBlendAttachmentDescription AlphaBlend;

  /// <summary>
  /// Describes a blend attachment state in which the source is added to the destination based on its alpha channel.
  /// Settings:
  ///     BlendEnabled = true
  ///     SourceColorFactor = RHIBlendFactor.SourceAlpha
  ///     DestinationColorFactor = RHIBlendFactor.One
  ///     ColorFunction = RHIBlendFunction.Add
  ///     SourceAlphaFactor = RHIBlendFactor.SourceAlpha
  ///     DestinationAlphaFactor = RHIBlendFactor.One
  ///     AlphaFunction = RHIBlendFunction.Add
  /// </summary>
  static const RHIBlendAttachmentDescription AdditiveBlend;

  /// <summary>
  /// Describes a blend attachment state in which blending is disabled.
  /// Settings:
  ///     BlendEnabled = false
  ///     SourceColorFactor = RHIBlendFactor.One
  ///     DestinationColorFactor = RHIBlendFactor.Zero
  ///     ColorFunction = RHIBlendFunction.Add
  ///     SourceAlphaFactor = RHIBlendFactor.One
  ///     DestinationAlphaFactor = RHIBlendFactor.Zero
  ///     AlphaFunction = RHIBlendFunction.Add
  /// </summary>
  static const RHIBlendAttachmentDescription Disabled;

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIBlendAttachmentDescription& other) const
  {
    return BlendEnabled == other.BlendEnabled &&
      SourceColorFactor == other.SourceColorFactor &&
      DestinationColorFactor == other.DestinationColorFactor &&
      ColorFunction == other.ColorFunction &&
      SourceAlphaFactor == other.SourceAlphaFactor &&
      DestinationAlphaFactor == other.DestinationAlphaFactor &&
      AlphaFunction == other.AlphaFunction;
  }
};
