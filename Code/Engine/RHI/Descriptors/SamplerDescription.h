#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

#include <optional>

/// <summary>
/// Describes a <see cref="Sampler"/>, for creation using a <see cref="ResourceFactory"/>.
/// </summary>
struct EZ_RHI_DLL RHISamplerDescription : public ezHashableStruct<RHISamplerDescription>
{
  EZ_DECLARE_POD_TYPE();

  /// <summary>
  /// The <see cref="SamplerAddressMode"/> mode to use for the U (or S) coordinate.
  /// </summary>
  ezEnum<RHISamplerAddressMode> AddressModeU;

  /// <summary>
  /// The <see cref="SamplerAddressMode"/> mode to use for the V (or T) coordinate.
  /// </summary>
  ezEnum<RHISamplerAddressMode> AddressModeV;

  /// <summary>
  /// The <see cref="SamplerAddressMode"/> mode to use for the W (or R) coordinate.
  /// </summary>
  ezEnum<RHISamplerAddressMode> AddressModeW;

  /// <summary>
  /// The filter used when sampling.
  /// </summary>
  ezEnum<RHISamplerFilter> Filter;

  /// <summary>
  /// An optional value controlling the kind of comparison to use when sampling. If null, comparison sampling is not used.
  /// </summary>
  std::optional<ezEnum<RHIComparisonKind>> ComparisonKind;

  /// <summary>
  /// The maximum anisotropy of the filter, when <see cref="SamplerFilter.Anisotropic"/> is used, or otherwise ignored.
  /// </summary>
  ezUInt32 MaximumAnisotropy;

  /// <summary>
  /// The minimum level of detail.
  /// </summary>
  ezUInt32 MinimumLod;

  /// <summary>
  /// The maximum level of detail.
  /// </summary>
  ezUInt32 MaximumLod;

  /// <summary>
  /// The level of detail bias.
  /// </summary>
  ezInt32 LodBias;

  /// <summary>
  /// The constant color that is sampled when <see cref="SamplerAddressMode.Border"/> is used, or otherwise ignored.
  /// </summary>
  ezEnum<RHISamplerBorderColor> BorderColor;

  /// <summary>
  /// Constructs a new SamplerDescription.
  /// </summary>
  /// <param name="addressModeU">The <see cref="SamplerAddressMode"/> mode to use for the U (or R) coordinate.</param>
  /// <param name="addressModeV">The <see cref="SamplerAddressMode"/> mode to use for the V (or S) coordinate.</param>
  /// <param name="addressModeW">The <see cref="SamplerAddressMode"/> mode to use for the W (or T) coordinate.</param>
  /// <param name="filter">The filter used when sampling.</param>
  /// <param name="comparisonKind">An optional value controlling the kind of comparison to use when sampling. If null,
  /// comparison sampling is not used.</param>
  /// <param name="maximumAnisotropy">The maximum anisotropy of the filter, when <see cref="SamplerFilter.Anisotropic"/> is
  /// used, or otherwise ignored.</param>
  /// <param name="minimumLod">The minimum level of detail.</param>
  /// <param name="maximumLod">The maximum level of detail.</param>
  /// <param name="lodBias">The level of detail bias.</param>
  /// <param name="borderColor">The constant color that is sampled when <see cref="SamplerAddressMode.Border"/> is used, or
  /// otherwise ignored.</param>
  RHISamplerDescription(
    ezEnum<RHISamplerAddressMode> addressModeU,
    ezEnum<RHISamplerAddressMode> addressModeV,
    ezEnum<RHISamplerAddressMode> addressModeW,
    ezEnum<RHISamplerFilter> filter,
    std::optional<ezEnum<RHIComparisonKind>> comparisonKind,
    ezUInt32 maximumAnisotropy,
    ezUInt32 minimumLod,
    ezUInt32 maximumLod,
    ezInt32 lodBias,
    ezEnum<RHISamplerBorderColor> borderColor)
  {
    AddressModeU = addressModeU;
    AddressModeV = addressModeV;
    AddressModeW = addressModeW;
    Filter = filter;
    ComparisonKind = comparisonKind;
    MaximumAnisotropy = maximumAnisotropy;
    MinimumLod = minimumLod;
    MaximumLod = maximumLod;
    LodBias = lodBias;
    BorderColor = borderColor;
  }

  /// <summary>
  /// Describes a common point-filter sampler, with wrapping address mode.
  /// Settings:
  ///     AddressModeU = SamplerAddressMode.Wrap
  ///     AddressModeV = SamplerAddressMode.Wrap
  ///     AddressModeW = SamplerAddressMode.Wrap
  ///     Filter = SamplerFilter.MinPoint_MagPoint_MipPoint
  ///     LodBias = 0
  ///     MinimumLod = 0
  ///     MaximumLod = uint.MaxValue
  ///     MaximumAnisotropy = 0
  /// </summary>
  static const RHISamplerDescription Point;

  /// <summary>
  /// Describes a common linear-filter sampler, with wrapping address mode.
  /// Settings:
  ///     AddressModeU = SamplerAddressMode.Wrap
  ///     AddressModeV = SamplerAddressMode.Wrap
  ///     AddressModeW = SamplerAddressMode.Wrap
  ///     Filter = SamplerFilter.MinLinear_MagLinear_MipLinear
  ///     LodBias = 0
  ///     MinimumLod = 0
  ///     MaximumLod = uint.MaxValue
  ///     MaximumAnisotropy = 0
  /// </summary>
  static const RHISamplerDescription Linear;

  /// <summary>
  /// Describes a common 4x-anisotropic-filter sampler, with wrapping address mode.
  /// Settings:
  ///     AddressModeU = SamplerAddressMode.Wrap
  ///     AddressModeV = SamplerAddressMode.Wrap
  ///     AddressModeW = SamplerAddressMode.Wrap
  ///     Filter = SamplerFilter.Anisotropic
  ///     LodBias = 0
  ///     MinimumLod = 0
  ///     MaximumLod = uint.MaxValue
  ///     MaximumAnisotropy = 4
  /// </summary>
  static const RHISamplerDescription Aniso4x;

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHISamplerDescription& other) const
  {
    return AddressModeU == other.AddressModeU &&
           AddressModeV == other.AddressModeV &&
           AddressModeW == other.AddressModeW &&
           Filter == other.Filter &&
           ComparisonKind == other.ComparisonKind &&
           MaximumAnisotropy == other.MaximumAnisotropy &&
           MinimumLod == other.MinimumLod &&
           MaximumLod == other.MaximumLod &&
           LodBias == other.LodBias &&
           BorderColor == other.BorderColor;
  }
};
