#pragma once

#include <RHI/Descriptors/OutputAttachmentDescription.h>
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <RHI/Util.h>

#include <optional>

class RHIFramebuffer;

/// <summary>
/// Describes a set of output attachments and their formats.
/// </summary>
struct EZ_RHI_DLL RHIOutputDescription : public ezHashableStruct<RHIOutputDescription>
{
  EZ_DECLARE_POD_TYPE();
  /// <summary>
  /// A description of the depth attachment, or null if none exists.
  /// </summary>
  std::optional<RHIOutputAttachmentDescription> DepthAttachment;

  /// <summary>
  /// An array of attachment descriptions, one for each color attachment. May be empty.
  /// </summary>
  ezDynamicArray<RHIOutputAttachmentDescription> ColorAttachments;

  /// <summary>
  /// The number of samples in each target attachment.
  /// </summary>
  ezEnum<RHITextureSampleCount> SampleCount;

  RHIOutputDescription() = default;

  RHIOutputDescription& operator=(const RHIOutputDescription& other)
  {
    DepthAttachment = other.DepthAttachment;

    ColorAttachments.SetCountUninitialized(other.ColorAttachments.GetCount());
    for (ezUInt32 i = 0; i < other.ColorAttachments.GetCount(); i++)
    {
      ColorAttachments[i] = other.ColorAttachments[i];
    }
    SampleCount = other.SampleCount;

    return *this;
  }

  /// <summary>
  /// Constructs a new <see cref="RHIOutputDescription"/>.
  /// </summary>
  /// <param name="depthAttachment">A description of the depth attachment.</param>
  /// <param name="colorAttachments">An array of descriptions of each color attachment.</param>
  RHIOutputDescription(std::optional<RHIOutputAttachmentDescription> depthAttachment, ezDynamicArray<RHIOutputAttachmentDescription>& colorAttachments)
  {
    DepthAttachment = depthAttachment;
    ColorAttachments = colorAttachments;
    SampleCount = RHITextureSampleCount::Count1;
  }

  /// <summary>
  /// Constructs a new <see cref="OutputDescription"/>.
  /// </summary>
  /// <param name="depthAttachment">A description of the depth attachment.</param>
  /// <param name="colorAttachments">An array of descriptions of each color attachment.</param>
  /// <param name="sampleCount">The number of samples in each target attachment.</param>
  RHIOutputDescription(
    std::optional<RHIOutputAttachmentDescription> depthAttachment,
    ezDynamicArray<RHIOutputAttachmentDescription>& colorAttachments,
    ezEnum<RHITextureSampleCount> sampleCount)
  {
    DepthAttachment = depthAttachment;
    ColorAttachments = colorAttachments;
    SampleCount = sampleCount;
  }

  static RHIOutputDescription CreateFromFramebuffer(RHIFramebuffer* framebuffer);

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIOutputDescription& other) const
  {
    return DepthAttachment == other.DepthAttachment &&
           Util::AreEquatable(ColorAttachments, other.ColorAttachments) &&
           SampleCount == other.SampleCount;
  }
};
