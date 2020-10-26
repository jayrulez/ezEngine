#include <RHI/Descriptors/TextureViewDescription.h>
#include <RHI/Resources/Texture.h>

RHITextureViewDescription::RHITextureViewDescription(RHITexture* target)
{
  Target = target;
  BaseMipLevel = 0;
  MipLevels = target->GetMipLevels();
  BaseArrayLayer = 0;
  ArrayLayers = target->GetArrayLayers();
  Format = std::make_optional(target->GetFormat());
}

RHITextureViewDescription::RHITextureViewDescription(RHITexture* target, ezEnum<RHIPixelFormat> format)
{
  Target = target;
  BaseMipLevel = 0;
  MipLevels = target->GetMipLevels();
  BaseArrayLayer = 0;
  ArrayLayers = target->GetArrayLayers();
  Format = std::make_optional(format);
}

RHITextureViewDescription::RHITextureViewDescription(RHITexture* target, ezUInt32 baseMipLevel, ezUInt32 mipLevels, ezUInt32 baseArrayLayer, ezUInt32 arrayLayers)
{
  Target = target;
  BaseMipLevel = baseMipLevel;
  MipLevels = mipLevels;
  BaseArrayLayer = baseArrayLayer;
  ArrayLayers = arrayLayers;
  Format = std::make_optional(target->GetFormat());
}

RHITextureViewDescription::RHITextureViewDescription(RHITexture* target, ezEnum<RHIPixelFormat> format, ezUInt32 baseMipLevel, ezUInt32 mipLevels, ezUInt32 baseArrayLayer, ezUInt32 arrayLayers)
{
  Target = target;
  BaseMipLevel = baseMipLevel;
  MipLevels = mipLevels;
  BaseArrayLayer = baseArrayLayer;
  ArrayLayers = arrayLayers;
  Format = std::make_optional(target->GetFormat());
}
