#include <RHI/RHIPCH.h>

EZ_STATICLINK_LIBRARY(RHI)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(RHI_Implementation_DefaultShaderSourceStreamFactory);
  EZ_STATICLINK_REFERENCE(RHI_Implementation_FrameBuffer);
  EZ_STATICLINK_REFERENCE(RHI_Implementation_RenderPass);
  EZ_STATICLINK_REFERENCE(RHI_Implementation_ResourceMapping);
  EZ_STATICLINK_REFERENCE(RHI_Implementation_RHIMemory);
  EZ_STATICLINK_REFERENCE(RHI_Implementation_Texture);
}
