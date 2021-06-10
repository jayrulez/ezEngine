#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Resources/BufferVk.h>
#include <RendererVk/Resources/ResourceViewVk.h>
#include <RendererVk/Resources/TextureVk.h>

ezGALResourceViewVk::ezGALResourceViewVk(ezGALResourceBase* pResource, const ezGALResourceViewCreationDescription& Description)
  : ezGALResourceView(pResource, Description)
{
}

ezGALResourceViewVk::~ezGALResourceViewVk() {}

ezResult ezGALResourceViewVk::InitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezResult ezGALResourceViewVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

EZ_STATICLINK_FILE(RendererVk, RendererVk_Resources_Implementation_ResourceViewVk);
