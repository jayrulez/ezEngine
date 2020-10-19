#include <RHIPCH.h>

EZ_STATICLINK_LIBRARY(RHI)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(RHI_Implementation_GraphicsDevice);
  EZ_STATICLINK_REFERENCE(RHI_Implementation_DX11_GraphicsDevice_DX11);
  EZ_STATICLINK_REFERENCE(RHI_Implementation_DX12_GraphicsDevice_DX12);
  EZ_STATICLINK_REFERENCE(RHI_Implementation_Vulkan_GraphicsDevice_Vulkan);
}
