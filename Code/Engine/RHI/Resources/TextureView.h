#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <optional>

#include <RHI/Resources/DeviceResource.h>
#include <RHI/Resources/Texture.h>

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
  RHITextureViewDescription(RHITexture* target)
  {
    Target = target;
    BaseMipLevel = 0;
    MipLevels = target->GetMipLevels();
    BaseArrayLayer = 0;
    ArrayLayers = target->GetArrayLayers();
    Format = std::make_optional(target->GetFormat());
  }

  /// <summary>
  /// Constructs a new TextureViewDescription.
  /// </summary>
  /// <param name="target">The desired target <see cref="Texture"/>. This <see cref="Texture"/> must have been created
  /// with the <see cref="TextureUsage.Sampled"/> usage flag.</param>
  /// <param name="format">Specifies how the data within the target Texture will be viewed.
  /// This format must be "compatible" with the target Texture's. For uncompressed formats, the overall size and number of
  /// components in this format must be equal to the underlying format. For compressed formats, it is only possible to use
  /// the same PixelFormat or its sRGB/non-sRGB counterpart.</param>
  RHITextureViewDescription(RHITexture* target, ezEnum<RHIPixelFormat> format)
  {
    Target = target;
    BaseMipLevel = 0;
    MipLevels = target->GetMipLevels();
    BaseArrayLayer = 0;
    ArrayLayers = target->GetArrayLayers();
    Format = std::make_optional(format);
  }

  /// <summary>
  /// Constructs a new TextureViewDescription.
  /// </summary>
  /// <param name="target">The desired target <see cref="Texture"/>.</param>
  /// <param name="baseMipLevel">The base mip level visible in the view. Must be less than <see cref="Texture.MipLevels"/>.
  /// </param>
  /// <param name="mipLevels">The number of mip levels visible in the view.</param>
  /// <param name="baseArrayLayer">The base array layer visible in the view.</param>
  /// <param name="arrayLayers">The number of array layers visible in the view.</param>
  RHITextureViewDescription(RHITexture* target, ezUInt32 baseMipLevel, ezUInt32 mipLevels, ezUInt32 baseArrayLayer, ezUInt32 arrayLayers)
  {
    Target = target;
    BaseMipLevel = baseMipLevel;
    MipLevels = mipLevels;
    BaseArrayLayer = baseArrayLayer;
    ArrayLayers = arrayLayers;
    Format = std::make_optional(target->GetFormat());
  }

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
  RHITextureViewDescription(RHITexture* target, ezEnum<RHIPixelFormat> format, ezUInt32 baseMipLevel, ezUInt32 mipLevels, ezUInt32 baseArrayLayer, ezUInt32 arrayLayers)
  {
    Target = target;
    BaseMipLevel = baseMipLevel;
    MipLevels = mipLevels;
    BaseArrayLayer = baseArrayLayer;
    ArrayLayers = arrayLayers;
    Format = std::make_optional(target->GetFormat());
  }

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

/// <summary>
/// A bindable device resource which provides a shader with access to a sampled <see cref="RHITexture"/> object.
/// See <see cref="RHITextureViewDescription"/>.
/// </summary>
class EZ_RHI_DLL RHITextureView : public RHIDeviceResource
{
public:
  RHITextureView()
  {
    Flags.Add(RHIDeviceResourceFlags::Bindable);
  }

  /// <summary>
  /// The target <see cref="Texture"/> object to be sampled via this instance.
  /// </summary>
  RHITexture* GetTarget() const { return Target; }
  /// <summary>
  /// The base mip level visible in the view.
  /// </summary>
  unsigned int GetBaseMipLevel() const { return BaseMipLevel; }
  /// <summary>
  /// The number of mip levels visible in the view.
  /// </summary>
  unsigned int GetMipLevels() const { return MipLevels; }
  /// <summary>
  /// The base array layer visible in the view.
  /// </summary>
  unsigned int GetBaseArrayLayer() const { return BaseArrayLayer; }
  /// <summary>
  /// The number of array layers visible in the view.
  /// </summary>
  unsigned int GetArrayLayers() const { return ArrayLayers; }
  /// <summary>
  /// The format used to interpret the contents of the target Texture. This may be different from the target Texture's
  /// true storage format, but it will be the same size.
  /// </summary>
  ezEnum<RHIPixelFormat> GetFormat() const { return Format; }

  /// <summary>
  /// A string identifying this instance. Can be used to differentiate between objects in graphics debuggers and other
  /// tools.
  /// </summary>
  virtual ezString GetName() const = 0;
  virtual void SetName(const ezString& name) = 0;

private:
  RHITextureView(const RHITextureViewDescription& description)
  {
    Target = description.Target;
    BaseMipLevel = description.BaseMipLevel;
    MipLevels = description.MipLevels;
    BaseArrayLayer = description.BaseArrayLayer;
    ArrayLayers = description.ArrayLayers;
    Format = description.Format.has_value() ? description.Format.value() : description.Target->GetFormat();
  }

private:
  RHITexture* Target;
  unsigned int BaseMipLevel = 0;
  unsigned int MipLevels = 0;
  unsigned int BaseArrayLayer = 0;
  unsigned int ArrayLayers = 0;
  ezEnum<RHIPixelFormat> Format;
};
