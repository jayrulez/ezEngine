#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// A <see cref="RHIPipeline"/> component describing the blend behavior for an individual color attachment.
/// </summary>
struct EZ_RHI_DLL RHIBlendAttachmentDescription : public ezHashableStruct<RHIBlendAttachmentDescription>
{
  /// <summary>
  /// Controls whether blending is enabled for the color attachment.
  /// </summary>
  bool BlendEnabled;
  /// <summary>
  /// Controls the source color's influence on the blend result.
  /// </summary>
  ezEnum<RHIBlendFactor> SourceColorFactor;
  /// <summary>
  /// Controls the destination color's influence on the blend result.
  /// </summary>
  ezEnum<RHIBlendFactor> DestinationColorFactor;
  /// <summary>
  /// Controls the function used to combine the source and destination color factors.
  /// </summary>
  ezEnum<RHIBlendFunction> ColorFunction;
  /// <summary>
  /// Controls the source alpha's influence on the blend result.
  /// </summary>
  ezEnum<RHIBlendFactor> SourceAlphaFactor;
  /// <summary>
  /// Controls the destination alpha's influence on the blend result.
  /// </summary>
  ezEnum<RHIBlendFactor> DestinationAlphaFactor;
  /// <summary>
  /// Controls the function used to combine the source and destination alpha factors.
  /// </summary>
  ezEnum<RHIBlendFunction> AlphaFunction;

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
    return BlendEnabled == other.BlendEnabled && SourceColorFactor == other.SourceColorFactor && DestinationColorFactor == other.DestinationColorFactor && ColorFunction == other.ColorFunction && SourceAlphaFactor == other.SourceAlphaFactor && DestinationAlphaFactor == other.DestinationAlphaFactor && AlphaFunction == other.AlphaFunction;
  }
};

// TODO: Move to cpp file
const RHIBlendAttachmentDescription RHIBlendAttachmentDescription::OverrideBlend = RHIBlendAttachmentDescription{
  true,                  // BlendEnabled
  RHIBlendFactor::One,   // SourceColorFactor
  RHIBlendFactor::Zero,  // DestinationColorFactor
  RHIBlendFunction::Add, // ColorFunction
  RHIBlendFactor::One,   // SourceAlphaFactor
  RHIBlendFactor::Zero,  // DestinationAlphaFactor
  RHIBlendFunction::Add  // AlphaFunction
};

const RHIBlendAttachmentDescription RHIBlendAttachmentDescription::AlphaBlend = RHIBlendAttachmentDescription{
  true,                               // BlendEnabled
  RHIBlendFactor::SourceAlpha,        // SourceColorFactor
  RHIBlendFactor::InverseSourceAlpha, // DestinationColorFactor
  RHIBlendFunction::Add,              // ColorFunction
  RHIBlendFactor::SourceAlpha,        // SourceAlphaFactor
  RHIBlendFactor::InverseSourceAlpha, // DestinationAlphaFactor
  RHIBlendFunction::Add               // AlphaFunction
};

const RHIBlendAttachmentDescription RHIBlendAttachmentDescription::AdditiveBlend = RHIBlendAttachmentDescription{
  true,                        // BlendEnabled
  RHIBlendFactor::SourceAlpha, // SourceColorFactor
  RHIBlendFactor::One,         // DestinationColorFactor
  RHIBlendFunction::Add,       // ColorFunction
  RHIBlendFactor::SourceAlpha, // SourceAlphaFactor
  RHIBlendFactor::One,         // DestinationAlphaFactor
  RHIBlendFunction::Add        // AlphaFunction
};

const RHIBlendAttachmentDescription RHIBlendAttachmentDescription::Disabled = RHIBlendAttachmentDescription{
  false,                 // BlendEnabled
  RHIBlendFactor::One,   // SourceColorFactor
  RHIBlendFactor::Zero,  // DestinationColorFactor
  RHIBlendFunction::Add, // ColorFunction
  RHIBlendFactor::One,   // SourceAlphaFactor
  RHIBlendFactor::Zero,  // DestinationAlphaFactor
  RHIBlendFunction::Add  // AlphaFunction
};
// TODO end

/// <summary>
/// A <see cref="Pipeline"/> component describing how values are blended into each individual color target.
/// </summary>
struct EZ_RHI_DLL RHIBlendStateDescription : public ezHashableStruct<RHIBlendStateDescription>
{
  /// <summary>
  /// A constant blend color used in <see cref="RHIBlendFactor.BlendFactor"/> and <see cref="RHIBlendFactor.InverseBlendFactor"/>,
  /// or otherwise ignored.
  /// </summary>
  ezColor BlendFactor;

  /// <summary>
  /// An array of <see cref="RHIBlendAttachmentDescription"/> describing how blending is performed for each color target
  /// used in the <see cref="RHIPipeline"/>.
  /// </summary>
  ezDynamicArray<RHIBlendAttachmentDescription> AttachmentStates;

  /// <summary>
  /// Enables alpha-to-coverage, which causes a fragment's alpha value to be used when determining multi-sample coverage.
  /// </summary>
  bool AlphaToCoverageEnabled = false;

  RHIBlendStateDescription() = default;

  /// <summary>
  /// Constructs a new <see cref="BlendStateDescription"/>,
  /// </summary>
  /// <param name="blendFactor">The constant blend color.</param>
  /// <param name="attachmentStates">The blend attachment states.</param>
  RHIBlendStateDescription(ezColor blendFactor, ezDynamicArray<RHIBlendAttachmentDescription>& attachmentStates)
  {
    BlendFactor = blendFactor;
    AttachmentStates = attachmentStates;
    AlphaToCoverageEnabled = false;
  }

  /// <summary>
  /// Constructs a new <see cref="RHIBlendStateDescription"/>,
  /// </summary>
  /// <param name="blendFactor">The constant blend color.</param>
  /// <param name="alphaToCoverageEnabled">Enables alpha-to-coverage, which causes a fragment's alpha value to be
  /// used when determining multi-sample coverage.</param>
  /// <param name="attachmentStates">The blend attachment states.</param>
  RHIBlendStateDescription(
    ezColor blendFactor,
    bool alphaToCoverageEnabled,
    ezDynamicArray<RHIBlendAttachmentDescription>& attachmentStates)
  {
    BlendFactor = blendFactor;
    AttachmentStates = attachmentStates;
    AlphaToCoverageEnabled = alphaToCoverageEnabled;
  }

  /// <summary>
  /// Describes a blend state in which a single color target is blended with <see cref="RHIBlendAttachmentDescription.OverrideBlend"/>.
  /// </summary>
  static const RHIBlendStateDescription SingleOverrideBlend()
  {
    RHIBlendStateDescription desc;
    desc.AttachmentStates.PushBack(RHIBlendAttachmentDescription::OverrideBlend);
    return desc;
  }

  /// <summary>
  /// Describes a blend state in which a single color target is blended with <see cref="RHIBlendAttachmentDescription.AlphaBlend"/>.
  /// </summary>
  static const RHIBlendStateDescription SingleAlphaBlend()
  {
    RHIBlendStateDescription desc;
    desc.AttachmentStates.PushBack(RHIBlendAttachmentDescription::AlphaBlend);
    return desc;
  }

  /// <summary>
  /// Describes a blend state in which a single color target is blended with <see cref="RHIBlendAttachmentDescription.AdditiveBlend"/>.
  /// </summary>
  static const RHIBlendStateDescription SingleAdditiveBlend()
  {
    RHIBlendStateDescription desc;
    desc.AttachmentStates.PushBack(RHIBlendAttachmentDescription::AdditiveBlend);
    return desc;
  }

  /// <summary>
  /// Describes a blend state in which a single color target is blended with <see cref="RHIBlendAttachmentDescription.Disabled"/>.
  /// </summary>
  static const RHIBlendStateDescription SingleDisabled()
  {
    RHIBlendStateDescription desc;
    desc.AttachmentStates.PushBack(RHIBlendAttachmentDescription::Disabled);
    return desc;
  }

  /// <summary>
  /// Describes an empty blend state in which no color targets are used.
  /// </summary>
  static const RHIBlendStateDescription Empty()
  {
    RHIBlendStateDescription desc;
    return desc;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIBlendStateDescription& other) const
  {
    if (BlendFactor == other.BlendFactor && AlphaToCoverageEnabled == other.AlphaToCoverageEnabled)
    {
      if (AttachmentStates.GetCount() != other.AttachmentStates.GetCount())
      {
        return false;
      }

      for (ezUInt32 i = 0; i < AttachmentStates.GetCount(); i++)
      {
        if (!(AttachmentStates[i] == other.AttachmentStates[i]))
        {
          return false;
        }
      }
    }
    return true;
  }

  RHIBlendStateDescription Clone()
  {
    ezDynamicArray<RHIBlendAttachmentDescription> attachmentStates;
    attachmentStates.SetCountUninitialized(this->AttachmentStates.GetCount());
    attachmentStates.GetArrayPtr().CopyFrom(this->AttachmentStates.GetArrayPtr());

    RHIBlendStateDescription result(this->BlendFactor, this->AlphaToCoverageEnabled, attachmentStates);

    return result;
  }
};
