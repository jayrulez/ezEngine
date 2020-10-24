#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <optional>

#include <RHI/Resources/DeviceResource.h>
#include <RHI/Resources/Texture.h>

class RHIFramebuffer;

/// <summary>
/// Describes an individual output attachment and its format.
/// </summary>
struct EZ_RHI_DLL RHIOutputAttachmentDescription : public ezHashableStruct<RHIOutputAttachmentDescription>
{
  EZ_DECLARE_POD_TYPE();

  /// <summary>
  /// The format of the <see cref="RHITexture"/> attachment.
  /// </summary>
  ezEnum<RHIPixelFormat> Format;

  RHIOutputAttachmentDescription() = default;

  /// <summary>
  /// Constructs a new <see cref="RHIOutputAttachmentDescription"/>.
  /// </summary>
  /// <param name="format">THe format of the <see cref="Texture"/> attachment.</param>
  RHIOutputAttachmentDescription(ezEnum<RHIPixelFormat> format)
  {
    Format = format;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIOutputAttachmentDescription& other) const
  {
    return Format == other.Format;
  }
};

/// <summary>
/// Describes a set of output attachments and their formats.
/// </summary>
struct EZ_RHI_DLL RHIOutputDescription : public ezHashableStruct<RHIOutputDescription>
{
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
    return DepthAttachment == other.DepthAttachment && ColorAttachments == other.ColorAttachments && SampleCount == other.SampleCount;
  }
};

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
  RHIFramebufferAttachmentDescription(RHITexture* target, ezUInt32 arrayLayer, ezUInt32 mipLevel)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    ezUInt32 effectiveArrayLayers = target->GetArrayLayers();
    if ((target->GetUsage() & RHITextureUsage::Cubemap) != 0)
    {
      effectiveArrayLayers *= 6;
    }

    if (arrayLayer >= effectiveArrayLayers)
    {
      EZ_REPORT_FAILURE("arrayLayer must be less then target.ArrayLayers");
    }
    if (mipLevel >= target->GetMipLevels())
    {
      EZ_REPORT_FAILURE("mipLevel must be less than target.MipLevels");
    }
#endif
    Target = target;
    ArrayLayer = arrayLayer;
    MipLevel = mipLevel;
  }

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

/// <summary>
/// Represents a single output of a <see cref="RHIFramebuffer"/>. May be a color or depth attachment.
/// </summary>
class EZ_RHI_DLL RHIFramebufferAttachment : public ezHashableStruct<RHIFramebufferAttachment>
{
public:
  /// <summary>
  /// The target <see cref="Texture"/> which will be rendered to.
  /// </summary>
  virtual RHITexture* GetTarget() const
  {
    return Target;
  }

  /// <summary>
  /// The target array layer.
  /// </summary>
  virtual ezUInt32 GetArrayLayer() const
  {
    return ArrayLayer;
  }

  /// <summary>
  /// The target mip level.
  /// </summary>
  virtual ezUInt32 GetMipLevel() const
  {
    return MipLevel;
  }

  /// <summary>
  /// Constructs a new FramebufferAttachment.
  /// </summary>
  /// <param name="target">The target <see cref="RHITexture"/> which will be rendered to.</param>
  /// <param name="arrayLayer">The target array layer.</param>
  RHIFramebufferAttachment(RHITexture* target, ezUInt32 arrayLayer)
  {
    Target = target;
    ArrayLayer = arrayLayer;
    MipLevel = 0;
  }

  /// <summary>
  /// Constructs a new FramebufferAttachment.
  /// </summary>
  /// <param name="target">The target <see cref="RHITexture"/> which will be rendered to.</param>
  /// <param name="arrayLayer">The target array layer.</param>
  /// <param name="mipLevel">The target mip level.</param>
  RHIFramebufferAttachment(RHITexture* target, ezUInt32 arrayLayer, ezUInt32 mipLevel)
  {
    Target = target;
    ArrayLayer = arrayLayer;
    MipLevel = mipLevel;
  }

private:
  RHITexture* Target = nullptr;
  ezUInt32 ArrayLayer = 0;
  ezUInt32 MipLevel = 0;
};

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

/// <summary>
/// A device resource used to control which color and depth textures are rendered to.
/// See <see cref="RHIFramebufferDescription"/>.
/// </summary>
class RHIFramebuffer : public RHIDeviceResource
{
public:
  RHIFramebuffer() = default;

  RHIFramebuffer(std::optional<RHIFramebufferAttachmentDescription>& depthTargetDesc, ezDynamicArray<RHIFramebufferAttachmentDescription>& colorTargetDescs)
  {
    if (depthTargetDesc)
    {
      RHIFramebufferAttachmentDescription depthAttachment = depthTargetDesc.value();
      DepthTarget = RHIFramebufferAttachment(depthAttachment.Target, depthAttachment.ArrayLayer, depthAttachment.MipLevel);
    }

    for (ezUInt32 i = 0; i < colorTargetDescs.GetCount(); i++)
    {
      ColorTargets.PushBack(RHIFramebufferAttachment(colorTargetDescs[i].Target, colorTargetDescs[i].ArrayLayer, colorTargetDescs[i].MipLevel));
    }

    RHITexture* dimTex = nullptr;
    ezUInt32 mipLevel;

    if (ColorTargets.GetCount() > 0)
    {
      dimTex = ColorTargets[0].GetTarget();
      mipLevel = ColorTargets[0].GetMipLevel();
    }
    else
    {
      EZ_VERIFY(DepthTarget.has_value(), "DepthTarget is not set.");
      dimTex = DepthTarget.value().GetTarget();
      dimTex = DepthTarget.value().GetTarget();
    }

    ezUInt32 mipWidth = 0;
    ezUInt32 mipHeight = 0;

    // TODO: implement this
    //Util.GetMipDimensions(dimTex, mipLevel, out uint mipWidth, out uint mipHeight, out _);

    Width = mipWidth;
    Height = mipHeight;

    OutputDescription = RHIOutputDescription::CreateFromFramebuffer(this);
  }

  /// <summary>
  /// Gets the depth attachment associated with this instance. May be null if no depth texture is used.
  /// </summary>
  virtual std::optional<RHIFramebufferAttachment> GetDepthTarget() const
  {
    return DepthTarget;
  }

  /// <summary>
  /// Gets the collection of color attachments associated with this instance. May be empty.
  /// </summary>
  virtual ezDynamicArray<RHIFramebufferAttachment> GetColorTargets() const
  {
    return ColorTargets;
  }

  /// <summary>
  /// Gets an <see cref="RHIOutputDescription"/> which describes the number and formats of the depth and color targets
  /// in this instance.
  /// </summary>
  virtual RHIOutputDescription GetOutputDescription() const
  {
    return OutputDescription;
  }

  /// <summary>
  /// Gets the width of the <see cref="RHIFramebuffer"/>.
  /// </summary>
  virtual ezUInt32 GetWidth() const
  {
    return Width;
  }

  /// <summary>
  /// Gets the height of the <see cref="RHIFramebuffer"/>.
  /// </summary>
  virtual ezUInt32 GetHeight() const
  {
    return Height;
  }

private:
  std::optional<RHIFramebufferAttachment> DepthTarget;
  ezDynamicArray<RHIFramebufferAttachment> ColorTargets;
  RHIOutputDescription OutputDescription;
  ezUInt32 Width = 0;
  ezUInt32 Height = 0;
};
