#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/FramebufferAttachmentDescription.h>

#include <Foundation/Algorithm/HashableStruct.h>

#include <optional>

/// <summary>
/// Describes a <see cref="RHIFramebuffer"/>, for creation using a <see cref="RHIResourceFactory"/>.
/// </summary>
struct EZ_RHI_DLL RHIFramebufferDescription : public ezHashableStruct<RHIFramebufferDescription>
{
  /// <summary>
  /// The depth texture, which must have been created with <see cref="RHITextureUsage.DepthStencil"/> usage flags.
  /// May be null.
  /// </summary>
  std::optional<RHIFramebufferAttachmentDescription> DepthTarget;

  /// <summary>
  /// An array of color textures, all of which must have been created with <see cref="RHITextureUsage.RenderTarget"/>
  /// usage flags. May be null or empty.
  /// </summary>
  ezDynamicArray<RHIFramebufferAttachmentDescription> ColorTargets;

  /// <summary>
  /// Constructs a new <see cref="RHIFramebufferDescription"/>.
  /// </summary>
  /// <param name="depthTarget">The depth texture, which must have been created with
  /// <see cref="RHITextureUsage.DepthStencil"/> usage flags. May be null.</param>
  /// <param name="colorTargets">An array of color textures, all of which must have been created with
  /// <see cref="RHITextureUsage.RenderTarget"/> usage flags. May be null or empty.</param>
  RHIFramebufferDescription(RHITexture* depthTarget, ezDynamicArray<RHITexture*>& colorTargets)
  {
    if (depthTarget != nullptr)
    {
      DepthTarget = std::make_optional(RHIFramebufferAttachmentDescription(depthTarget, 0));
    }
    else
    {
      DepthTarget = std::nullopt;
    }
    ColorTargets.Clear();
    ColorTargets.SetCountUninitialized(colorTargets.GetCount());

    for (ezUInt32 i = 0; i < colorTargets.GetCount(); i++)
    {
      ColorTargets[i] = RHIFramebufferAttachmentDescription(colorTargets[i], 0);
    }
  }

  /// <summary>
  /// Constructs a new <see cref="RHIFramebufferDescription"/>.
  /// </summary>
  /// <param name="depthTarget">A description of the depth attachment. May be null if no depth attachment will be used.</param>
  /// <param name="colorTargets">An array of descriptions of color attachments. May be empty if no color attachments will
  /// be used.</param>
  RHIFramebufferDescription(
    std::optional<RHIFramebufferAttachmentDescription>& depthTarget,
    ezDynamicArray<RHIFramebufferAttachmentDescription>& colorTargets)
  {
    DepthTarget = depthTarget;
    ColorTargets = colorTargets;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIFramebufferDescription& other) const
  {
    return DepthTarget == other.DepthTarget && ColorTargets == other.ColorTargets;
  }
};
