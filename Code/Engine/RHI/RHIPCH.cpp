#include <RHIPCH.h>

EZ_STATICLINK_LIBRARY(RHI)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(RHI_Implementation_Utils);
  EZ_STATICLINK_REFERENCE(RHI_Descriptors_Implementation_BlendAttachmentDescription);
  EZ_STATICLINK_REFERENCE(RHI_Descriptors_Implementation_BlendStencilStateDescription);
  EZ_STATICLINK_REFERENCE(RHI_Descriptors_Implementation_FramebufferAttachmentDescription);
  EZ_STATICLINK_REFERENCE(RHI_Descriptors_Implementation_OutputDescription);
  EZ_STATICLINK_REFERENCE(RHI_Descriptors_Implementation_RasterizerStateDescription);
  EZ_STATICLINK_REFERENCE(RHI_Descriptors_Implementation_SamplerDescription);
  EZ_STATICLINK_REFERENCE(RHI_Descriptors_Implementation_TextureViewDescription);
  EZ_STATICLINK_REFERENCE(RHI_Resources_Implementation_Texture);
}
