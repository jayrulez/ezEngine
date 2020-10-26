#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

class RHITexture;

/// <summary>
/// Describes a single attachment (color or depth) for a <see cref="RHIFramebuffer"/>.
/// </summary>
struct EZ_RHI_DLL RHIFramebufferAttachmentDescription : public ezHashableStruct<RHIFramebufferAttachmentDescription>
{
  EZ_DECLARE_POD_TYPE();
  /// <summary>
  /// The target texture to render into. For color attachments, this resource must have been created with the
  /// <see cref="RHITextureUsage.RenderTarget"/> flag. For depth attachments, this resource must have been created with the
  /// <see cref="RHITextureUsage.DepthStencil"/> flag.
  /// </summary>
  RHITexture* Target = nullptr;

  /// <summary>
  /// The array layer to render to. This value must be less than <see cref="RHITexture.ArrayLayers"/> in the target
  /// <see cref="Texture"/>.
  /// </summary>
  ezUInt32 ArrayLayer = 0;

  /// <summary>
  /// The mip level to render to. This value must be less than <see cref="RHITexture.MipLevels"/> in the target
  /// <see cref="RHITexture"/>.
  /// </summary>
  ezUInt32 MipLevel = 0;

  RHIFramebufferAttachmentDescription() = default;

  /// <summary>
  /// Constructs a new RHIFramebufferAttachmentDescription.
  /// </summary>
  /// <param name="target">The target texture to render into. For color attachments, this resource must have been created
  /// with the <see cref="TextureUsage.RenderTarget"/> flag. For depth attachments, this resource must have been created
  /// with the <see cref="TextureUsage.DepthStencil"/> flag.</param>
  /// <param name="arrayLayer">The array layer to render to. This value must be less than <see cref="Texture.ArrayLayers"/>
  /// in the target <see cref="Texture"/>.</param>
  RHIFramebufferAttachmentDescription(RHITexture* target, ezUInt32 arrayLayer)
    : Target(target)
    , ArrayLayer(arrayLayer)
    , MipLevel(0)
  {
  }

  /// <summary>
  /// Constructs a new FramebufferAttachmentDescription.
  /// </summary>
  /// <param name="target">The target texture to render into. For color attachments, this resource must have been created
  /// with the <see cref="RHITextureUsage.RenderTarget"/> flag. For depth attachments, this resource must have been created
  /// with the <see cref="RHITextureUsage.DepthStencil"/> flag.</param>
  /// <param name="arrayLayer">The array layer to render to. This value must be less than <see cref="RHITexture.ArrayLayers"/>
  /// in the target <see cref="RHITexture"/>.</param>
  /// <param name="mipLevel">The mip level to render to. This value must be less than <see cref="RHITexture.MipLevels"/> in
  /// the target <see cref="RHITexture"/>.</param>
  RHIFramebufferAttachmentDescription(RHITexture* target, ezUInt32 arrayLayer, ezUInt32 mipLevel);

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIFramebufferAttachmentDescription& other) const
  {
    return Target == other.Target && ArrayLayer == other.ArrayLayer && MipLevel == other.MipLevel;
  }
};
