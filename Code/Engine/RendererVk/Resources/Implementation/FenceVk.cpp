#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/RendererVkDLL.h>
#include <RendererVk/Resources/FenceVk.h>

ezGALFenceVk::ezGALFenceVk()
{
}

ezGALFenceVk::~ezGALFenceVk() {}


ezResult ezGALFenceVk::InitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezResult ezGALFenceVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}



EZ_STATICLINK_FILE(RendererVk, RendererVk_Resources_Implementation_FenceVk);
