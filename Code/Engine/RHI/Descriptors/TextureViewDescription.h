#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

#include <optional>

class RHITexture;

/// <summary>
/// Describes a <see cref="RHITextureView"/>, for creation using a <see cref="RHIResourceFactory"/>.
/// </summary>
struct EZ_RHI_DLL RHITextureViewDescription : public ezHashableStruct<RHITextureViewDescription>
{
  /// <summary>
  /// The desired target <see cref="RHITexture"/>.
  /// </summary>
  RHITexture* Target;

  /// <summary>
  /// The base mip level visible in the view. Must be less than <see cref="RHITexture.MipLevels"/>.
  /// </summary>
  ezUInt32 BaseMipLevel = 0;

  /// <summary>
  /// The number of mip levels visible in the view.
  /// </summary>
  ezUInt32 MipLevels = 0;

  /// <summary>
  /// The base array layer visible in the view.
  /// </summary>
  ezUInt32 BaseArrayLayer = 0;

  /// <summary>
  /// The number of array layers visible in the view.
  /// </summary>
  ezUInt32 ArrayLayers = 0;

  /// <summary>
  /// An optional <see cref="RHIPixelFormat"/> which specifies how the data within <see cref="Target"/> will be viewed.
  /// If this value is null, then the created TextureView will use the same <see cref="PixelFormat"/> as the target
  /// <see cref="RHITexture"/>. If not null, this format must be "compatible" with the target Texture's. For uncompressed
  /// formats, the overall size and number of components in this format must be equal to the underlying format. For
  /// compressed formats, it is only possible to use the same PixelFormat or its sRGB/non-sRGB counterpart.
  /// </summary>
  std::optional<ezEnum<RHIPixelFormat>> Format;

  /// <summary>
  /// Constructs a new TextureViewDescription.
  /// </summary>
  /// <param name="target">The desired target <see cref="Texture"/>. This <see cref="Texture"/> must have been created
  /// with the <see cref="TextureUsage.Sampled"/> usage flag.</param>
  RHITextureViewDescription(RHITexture* target);

  /// <summary>
  /// Constructs a new TextureViewDescription.
  /// </summary>
  /// <param name="target">The desired target <see cref="Texture"/>. This <see cref="Texture"/> must have been created
  /// with the <see cref="TextureUsage.Sampled"/> usage flag.</param>
  /// <param name="format">Specifies how the data within the target Texture will be viewed.
  /// This format must be "compatible" with the target Texture's. For uncompressed formats, the overall size and number of
  /// components in this format must be equal to the underlying format. For compressed formats, it is only possible to use
  /// the same PixelFormat or its sRGB/non-sRGB counterpart.</param>
  RHITextureViewDescription(RHITexture* target, ezEnum<RHIPixelFormat> format);

  /// <summary>
  /// Constructs a new TextureViewDescription.
  /// </summary>
  /// <param name="target">The desired target <see cref="Texture"/>.</param>
  /// <param name="baseMipLevel">The base mip level visible in the view. Must be less than <see cref="Texture.MipLevels"/>.
  /// </param>
  /// <param name="mipLevels">The number of mip levels visible in the view.</param>
  /// <param name="baseArrayLayer">The base array layer visible in the view.</param>
  /// <param name="arrayLayers">The number of array layers visible in the view.</param>
  RHITextureViewDescription(RHITexture* target, ezUInt32 baseMipLevel, ezUInt32 mipLevels, ezUInt32 baseArrayLayer, ezUInt32 arrayLayers);

  /// <summary>
  /// Constructs a new TextureViewDescription.
  /// </summary>
  /// <param name="target">The desired target <see cref="Texture"/>.</param>
  /// <param name="format">Specifies how the data within the target Texture will be viewed.
  /// This format must be "compatible" with the target Texture's. For uncompressed formats, the overall size and number of
  /// components in this format must be equal to the underlying format. For compressed formats, it is only possible to use
  /// the same PixelFormat or its sRGB/non-sRGB counterpart.</param>
  /// <param name="baseMipLevel">The base mip level visible in the view. Must be less than <see cref="Texture.MipLevels"/>.
  /// </param>
  /// <param name="mipLevels">The number of mip levels visible in the view.</param>
  /// <param name="baseArrayLayer">The base array layer visible in the view.</param>
  /// <param name="arrayLayers">The number of array layers visible in the view.</param>
  RHITextureViewDescription(RHITexture* target, ezEnum<RHIPixelFormat> format, ezUInt32 baseMipLevel, ezUInt32 mipLevels, ezUInt32 baseArrayLayer, ezUInt32 arrayLayers);

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHITextureViewDescription& other) const
  {
    return Target == other.Target && BaseMipLevel == other.BaseMipLevel && MipLevels == other.MipLevels && BaseArrayLayer == other.BaseArrayLayer && ArrayLayers == other.ArrayLayers && Format == other.Format;
  }
};
