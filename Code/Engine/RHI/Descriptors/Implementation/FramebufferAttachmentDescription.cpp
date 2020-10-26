#include <RHI/Descriptors/FramebufferAttachmentDescription.h>
#include <RHI/Resources/Texture.h>

RHIFramebufferAttachmentDescription::RHIFramebufferAttachmentDescription(RHITexture* target, ezUInt32 arrayLayer, ezUInt32 mipLevel)
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
