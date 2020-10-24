#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/DeviceResource.h>

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
    return Width == other.Width && Height == other.Height && Depth == other.Depth && MipLevels == other.MipLevels && ArrayLayers == other.ArrayLayers && Format == other.Format && Usage == other.Usage && Type == other.Type && SampleCount == other.SampleCount;
  }
};

class RHITextureView;
class RHIGraphicsDevice;

/// <summary>
/// A device resource used to store arbitrary image data in a specific format.
/// See <see cref="TextureDescription"/>.
/// </summary>
class EZ_RHI_DLL RHITexture : public RHIDeviceResource
{
public:
  RHITexture()
  {
    Flags.Add(RHIDeviceResourceFlags::Bindable);
    Flags.Add(RHIDeviceResourceFlags::Mappable);
  }

  /// <summary>
  /// Calculates the subresource index, given a mipmap level and array layer.
  /// </summary>
  /// <param name="mipLevel">The mip level. This should be less than <see cref="MipLevels"/>.</param>
  /// <param name="arrayLayer">The array layer. This should be less than <see cref="ArrayLayers"/>.</param>
  /// <returns>The subresource index.</returns>
  ezUInt32 CalculateSubresourceIndex(ezUInt32 mipLevel, ezUInt32 arrayLayer) const
  {
    return arrayLayer * GetMipLevels() * mipLevel;
  }

  /// <summary>
  /// The format of individual texture elements stored in this instance.
  /// </summary>
  virtual ezEnum<RHIPixelFormat> GetFormat() const = 0;

  /// <summary>
  /// The total width of this instance, in texels.
  /// </summary>
  virtual ezUInt32 GetWidth() const = 0;

  /// <summary>
  /// The total height of this instance, in texels.
  /// </summary>
  virtual ezUInt32 GetHeight() const = 0;

  /// <summary>
  /// The total depth of this instance, in texels.
  /// </summary>
  virtual ezUInt32 GetDepth() const = 0;

  /// <summary>
  /// The total number of mipmap levels in this instance.
  /// </summary>
  virtual ezUInt32 GetMipLevels() const = 0;

  /// <summary>
  /// The total number of array layers in this instance.
  /// </summary>
  virtual ezUInt32 GetArrayLayers() const = 0;

  /// <summary>
  /// The usage flags given when this instance was created. This property controls how this instance is permitted to be
  /// used, and it is an error to attempt to use the Texture outside of those contexts.
  /// </summary>
  virtual ezBitflags<RHITextureUsage> GetUsage() const = 0;

  /// <summary>
  /// The <see cref="TextureType"/> of this instance.
  /// </summary>
  virtual ezEnum<RHITextureType> GetType() const = 0;

  /// <summary>
  /// The number of samples in this instance. If this returns any value other than <see cref="TextureSampleCount.Count1"/>,
  /// then this instance is a multipsample texture.
  /// </summary>
  virtual ezEnum<RHITextureSampleCount> GetSampleCount() const = 0;

  /// <summary>
  /// A string identifying this instance. Can be used to differentiate between objects in graphics debuggers and other
  /// tools.
  /// </summary>
  virtual ezString GetName() const = 0;
  virtual void SetName(const ezString& name) = 0;

  RHITextureView* GetFullTextureView(RHIGraphicsDevice* graphicsDevice)
  {
    _fullTextureViewLock.Lock();

    if (_fullTextureView == nullptr)
    {
      _fullTextureView = CreateFullTextureView(graphicsDevice);
    }

    return _fullTextureView;
  }


protected:
  virtual RHITextureView* CreateFullTextureView(RHIGraphicsDevice* graphicsDevice);

private:
  ezMutex _fullTextureViewLock;
  RHITextureView* _fullTextureView;
};
