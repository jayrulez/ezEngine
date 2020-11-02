#include <RHIPCH.h>

EZ_STATICLINK_LIBRARY(RHI)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(RHI_Implementation_GraphicsDevice);
  EZ_STATICLINK_REFERENCE(RHI_DX11_Implementation_GraphicsDevice_DX11);
  EZ_STATICLINK_REFERENCE(RHI_DX12_Implementation_GraphicsDevice_DX12);
  EZ_STATICLINK_REFERENCE(RHI_Vulkan_Implementation_GraphicsDevice_Vulkan);
}
