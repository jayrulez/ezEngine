#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/BlendAttachmentDescription.h>

#include <Foundation/Algorithm/HashableStruct.h>

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
    return BlendFactor == other.BlendFactor && AttachmentStates == other.AttachmentStates && AlphaToCoverageEnabled == other.AlphaToCoverageEnabled;
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
