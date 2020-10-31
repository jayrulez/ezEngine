#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// Describes a <see cref="RHITexture"/>, for creation using a <see cref="RHIResourceFactory"/>.
/// </summary>
struct EZ_RHI_DLL RHITextureDescription : public ezHashableStruct<RHITextureDescription>
{
  /// <summary>
  /// The total width, in texels.
  /// </summary>
  ezUInt32 Width;

  /// <summary>
  /// The total height, in texels.
  /// </summary>
  ezUInt32 Height;

  /// <summary>
  /// The total depth, in texels.
  /// </summary>
  ezUInt32 Depth;

  /// <summary>
  /// The number of mipmap levels.
  /// </summary>
  ezUInt32 MipLevels;

  /// <summary>
  /// The number of array layers.
  /// </summary>
  ezUInt32 ArrayLayers;

  /// <summary>
  /// The format of individual texture elements.
  /// </summary>
  ezEnum<RHIPixelFormat> Format;

  /// <summary>
  /// Controls how the Texture is permitted to be used. If the Texture will be sampled from a shader, then
  /// <see cref="RHITextureUsage.Sampled"/> must be included. If the Texture will be used as a depth target in a
  /// <see cref="RHIFramebuffer"/>, then <see cref="RHITextureUsage.DepthStencil"/> must be included. If the Texture will be used
  /// as a color target in a <see cref="Framebuffer"/>, then <see cref="RHITextureUsage.RenderTarget"/> must be included.
  /// If the Texture will be used as a 2D cubemap, then <see cref="RHITextureUsage.Cubemap"/> must be included.
  /// </summary>
  ezBitflags<RHITextureUsage> Usage;

  /// <summary>
  /// The type of Texture to create.
  /// </summary>
  ezEnum<RHITextureType> Type;

  /// <summary>
  /// The number of samples. If equal to <see cref="RHITextureSampleCount.Count1"/>, this instance does not describe a
  /// multisample <see cref="Texture"/>.
  /// </summary>
  ezEnum<RHITextureSampleCount> SampleCount;

  /// <summary>
  /// Contsructs a new TextureDescription describing a non-multisampled <see cref="RHITexture"/>.
  /// </summary>
  /// <param name="width">The total width, in texels.</param>
  /// <param name="height">The total height, in texels.</param>
  /// <param name="depth">The total depth, in texels.</param>
  /// <param name="mipLevels">The number of mipmap levels.</param>
  /// <param name="arrayLayers">The number of array layers.</param>
  /// <param name="format">The format of individual texture elements.</param>
  /// <param name="usage">Controls how the Texture is permitted to be used. If the Texture will be sampled from a shader,
  /// then <see cref="RHITextureUsage.Sampled"/> must be included. If the Texture will be used as a depth target in a
  /// <see cref="RHIFramebuffer"/>, then <see cref="RHITextureUsage.DepthStencil"/> must be included. If the Texture will be used
  /// as a color target in a <see cref="RHIFramebuffer"/>, then <see cref="RHITextureUsage.RenderTarget"/> must be included.
  /// If the Texture will be used as a 2D cubemap, then <see cref="RHITextureUsage.Cubemap"/> must be included.</param>
  /// <param name="type">The type of Texture to create.</param>
  RHITextureDescription(
    ezUInt32 width,
    ezUInt32 height,
    ezUInt32 depth,
    ezUInt32 mipLevels,
    ezUInt32 arrayLayers,
    ezEnum<RHIPixelFormat> format,
    ezBitflags<RHITextureUsage> usage,
    ezEnum<RHITextureType> type)
  {
    Width = width;
    Height = height;
    Depth = depth;
    MipLevels = mipLevels;
    ArrayLayers = arrayLayers;
    Format = format;
    Usage = usage;
    SampleCount = RHITextureSampleCount::Count1;
    Type = type;
  }

  /// <summary>
  /// Contsructs a new TextureDescription.
  /// </summary>
  /// <param name="width">The total width, in texels.</param>
  /// <param name="height">The total height, in texels.</param>
  /// <param name="depth">The total depth, in texels.</param>
  /// <param name="mipLevels">The number of mipmap levels.</param>
  /// <param name="arrayLayers">The number of array layers.</param>
  /// <param name="format">The format of individual texture elements.</param>
  /// <param name="usage">Controls how the Texture is permitted to be used. If the Texture will be sampled from a shader,
  /// then <see cref="RHITextureUsage.Sampled"/> must be included. If the Texture will be used as a depth target in a
  /// <see cref="RHIFramebuffer"/>, then <see cref="RHITextureUsage.DepthStencil"/> must be included. If the Texture will be used
  /// as a color target in a <see cref="RHIFramebuffer"/>, then <see cref="RHITextureUsage.RenderTarget"/> must be included.
  /// If the Texture will be used as a 2D cubemap, then <see cref="RHITextureUsage.Cubemap"/> must be included.</param>
  /// <param name="type">The type of Texture to create.</param>
  /// <param name="sampleCount">The number of samples. If any other value than <see cref="RHITextureSampleCount.Count1"/> is
  /// provided, then this describes a multisample texture.</param>
  RHITextureDescription(
    ezUInt32 width,
    ezUInt32 height,
    ezUInt32 depth,
    ezUInt32 mipLevels,
    ezUInt32 arrayLayers,
    ezEnum<RHIPixelFormat> format,
    ezEnum<RHITextureUsage> usage,
    ezEnum<RHITextureType> type,
    ezEnum<RHITextureSampleCount> sampleCount)
  {
    Width = width;
    Height = height;
    Depth = depth;
    MipLevels = mipLevels;
    ArrayLayers = arrayLayers;
    Format = format;
    Usage = usage;
    Type = type;
    SampleCount = sampleCount;
  }

  /// <summary>
  /// Creates a description for a non-multisampled 1D Texture.
  /// </summary>
  /// <param name="width">The total width, in texels.</param>
  /// <param name="mipLevels">The number of mipmap levels.</param>
  /// <param name="arrayLayers">The number of array layers.</param>
  /// <param name="format">The format of individual texture elements.</param>
  /// <param name="usage">Controls how the Texture is permitted to be used. If the Texture will be sampled from a shader,
  /// then <see cref="TextureUsage.Sampled"/> must be included. If the Texture will be used as a depth target in a
  /// <see cref="Framebuffer"/>, then <see cref="TextureUsage.DepthStencil"/> must be included. If the Texture will be used
  /// as a color target in a <see cref="Framebuffer"/>, then <see cref="TextureUsage.RenderTarget"/> must be included.
  /// </param>
  /// <returns>A new TextureDescription for a non-multisampled 1D Texture.</returns>
  static RHITextureDescription Texture1D(
    ezUInt32 width,
    ezUInt32 mipLevels,
    ezUInt32 arrayLayers,
    ezEnum<RHIPixelFormat> format,
    ezEnum<RHITextureUsage> usage)
  {
    return RHITextureDescription(
      width,
      1,
      1,
      mipLevels,
      arrayLayers,
      format,
      usage,
      RHITextureType::Texture1D,
      RHITextureSampleCount::Count1);
  }

  /// <summary>
  /// Creates a description for a non-multisampled 2D Texture.
  /// </summary>
  /// <param name="width">The total width, in texels.</param>
  /// <param name="height">The total height, in texels.</param>
  /// <param name="mipLevels">The number of mipmap levels.</param>
  /// <param name="arrayLayers">The number of array layers.</param>
  /// <param name="format">The format of individual texture elements.</param>
  /// <param name="usage">Controls how the Texture is permitted to be used. If the Texture will be sampled from a shader,
  /// then <see cref="TextureUsage.Sampled"/> must be included. If the Texture will be used as a depth target in a
  /// <see cref="Framebuffer"/>, then <see cref="TextureUsage.DepthStencil"/> must be included. If the Texture will be used
  /// as a color target in a <see cref="Framebuffer"/>, then <see cref="TextureUsage.RenderTarget"/> must be included.
  /// If the Texture will be used as a 2D cubemap, then <see cref="TextureUsage.Cubemap"/> must be included.</param>
  /// <returns>A new TextureDescription for a non-multisampled 2D Texture.</returns>
  static RHITextureDescription Texture2D(
    ezUInt32 width,
    ezUInt32 height,
    ezUInt32 mipLevels,
    ezUInt32 arrayLayers,
    ezEnum<RHIPixelFormat> format,
    ezEnum<RHITextureUsage> usage)
  {
    return RHITextureDescription(
      width,
      height,
      1,
      mipLevels,
      arrayLayers,
      format,
      usage,
      RHITextureType::Texture2D,
      RHITextureSampleCount::Count1);
  }

  /// <summary>
  /// Creates a description for a 2D Texture.
  /// </summary>
  /// <param name="width">The total width, in texels.</param>
  /// <param name="height">The total height, in texels.</param>
  /// <param name="mipLevels">The number of mipmap levels.</param>
  /// <param name="arrayLayers">The number of array layers.</param>
  /// <param name="format">The format of individual texture elements.</param>
  /// <param name="usage">Controls how the Texture is permitted to be used. If the Texture will be sampled from a shader,
  /// then <see cref="TextureUsage.Sampled"/> must be included. If the Texture will be used as a depth target in a
  /// <see cref="Framebuffer"/>, then <see cref="TextureUsage.DepthStencil"/> must be included. If the Texture will be used
  /// as a color target in a <see cref="Framebuffer"/>, then <see cref="TextureUsage.RenderTarget"/> must be included.
  /// If the Texture will be used as a 2D cubemap, then <see cref="TextureUsage.Cubemap"/> must be included.</param>
  /// <param name="sampleCount">The number of samples. If any other value than <see cref="TextureSampleCount.Count1"/> is
  /// provided, then this describes a multisample texture.</param>
  /// <returns>A new TextureDescription for a 2D Texture.</returns>
  static RHITextureDescription Texture2D(
    ezUInt32 width,
    ezUInt32 height,
    ezUInt32 mipLevels,
    ezUInt32 arrayLayers,
    ezEnum<RHIPixelFormat> format,
    ezEnum<RHITextureUsage> usage,
    ezEnum<RHITextureSampleCount> sampleCount)
  {
    return RHITextureDescription(
      width,
      height,
      1,
      mipLevels,
      arrayLayers,
      format,
      usage,
      RHITextureType::Texture2D,
      sampleCount);
  }

  /// <summary>
  /// Creates a description for a 3D Texture.
  /// </summary>
  /// <param name="width">The total width, in texels.</param>
  /// <param name="height">The total height, in texels.</param>
  /// <param name="depth">The total depth, in texels.</param>
  /// <param name="mipLevels">The number of mipmap levels.</param>
  /// <param name="format">The format of individual texture elements.</param>
  /// <param name="usage">Controls how the Texture is permitted to be used. If the Texture will be sampled from a shader,
  /// then <see cref="TextureUsage.Sampled"/> must be included. If the Texture will be used as a depth target in a
  /// <see cref="Framebuffer"/>, then <see cref="TextureUsage.DepthStencil"/> must be included. If the Texture will be used
  /// as a color target in a <see cref="Framebuffer"/>, then <see cref="TextureUsage.RenderTarget"/> must be included.</param>
  /// <returns>A new TextureDescription for a 3D Texture.</returns>
  static RHITextureDescription Texture3D(
    ezUInt32 width,
    ezUInt32 height,
    ezUInt32 depth,
    ezUInt32 mipLevels,
    ezEnum<RHIPixelFormat> format,
    ezEnum<RHITextureUsage> usage)
  {
    return RHITextureDescription(
      width,
      height,
      depth,
      mipLevels,
      1,
      format,
      usage,
      RHITextureType::Texture3D,
      RHITextureSampleCount::Count1);
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHITextureDescription& other) const
  {
    return Width == other.Width &&
           Height == other.Height &&
           Depth == other.Depth &&
           MipLevels == other.MipLevels &&
           ArrayLayers == other.ArrayLayers &&
           Format == other.Format &&
           Usage == other.Usage &&
           Type == other.Type &&
           SampleCount == other.SampleCount;
  }
};