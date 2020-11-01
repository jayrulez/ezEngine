#pragma once

#include <RHI/Descriptors/TextureViewDescription.h>
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Resource.h>
#include <RHI/Resources/texture.h>

/// <summary>
/// A bindable device resource which provides a shader with access to a sampled <see cref="RHITexture"/> object.
/// See <see cref="RHITextureViewDescription"/>.
/// </summary>
class EZ_RHI_DLL RHITextureView : public RHIResource
{
public:
  /// <summary>
  /// The target <see cref="Texture"/> object to be sampled via this instance.
  /// </summary>
  RHITexture* GetTarget() const { return Target; }

  /// <summary>
  /// The base mip level visible in the view.
  /// </summary>
  ezUInt32 GetBaseMipLevel() const { return BaseMipLevel; }

  /// <summary>
  /// The number of mip levels visible in the view.
  /// </summary>
  ezUInt32 GetMipLevels() const { return MipLevels; }

  /// <summary>
  /// The base array layer visible in the view.
  /// </summary>
  ezUInt32 GetBaseArrayLayer() const { return BaseArrayLayer; }

  /// <summary>
  /// The number of array layers visible in the view.
  /// </summary>
  ezUInt32 GetArrayLayers() const { return ArrayLayers; }

  /// <summary>
  /// The format used to interpret the contents of the target Texture. This may be different from the target Texture's
  /// true storage format, but it will be the same size.
  /// </summary>
  ezEnum<RHIPixelFormat> GetFormat() const { return Format; }

protected:
  RHITextureView()
  {
    Flags.Add(RHIResourceFlags::Bindable);
  }
  RHITextureView(const RHITextureViewDescription& description)
    : RHITextureView()
  {
    Target = description.Target;
    BaseMipLevel = description.BaseMipLevel;
    MipLevels = description.MipLevels;
    BaseArrayLayer = description.BaseArrayLayer;
    ArrayLayers = description.ArrayLayers;
    Format = description.Format.has_value() ? description.Format.value() : description.Target->GetFormat();
  }

private:
  RHITexture* Target = nullptr;
  unsigned int BaseMipLevel = 0;
  unsigned int MipLevels = 0;
  unsigned int BaseArrayLayer = 0;
  unsigned int ArrayLayers = 0;
  ezEnum<RHIPixelFormat> Format;
};
