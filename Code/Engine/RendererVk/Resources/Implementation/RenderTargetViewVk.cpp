#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Resources/RenderTargetViewVk.h>
#include <RendererVk/Resources/TextureVk.h>

ezGALRenderTargetViewVk::ezGALRenderTargetViewVk(ezGALTexture* pTexture, const ezGALRenderTargetViewCreationDescription& Description)
  : ezGALRenderTargetView(pTexture, Description)
{
}

ezGALRenderTargetViewVk::~ezGALRenderTargetViewVk() {}

ezResult ezGALRenderTargetViewVk::InitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezResult ezGALRenderTargetViewVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

EZ_STATICLINK_FILE(RendererVk, RendererVk_Resources_Implementation_RenderTargetViewVk);
