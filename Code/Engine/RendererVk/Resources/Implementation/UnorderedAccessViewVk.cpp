#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Resources/BufferVk.h>
#include <RendererVk/Resources/TextureVk.h>
#include <RendererVk/Resources/UnorderedAccessViewVk.h>

ezGALUnorderedAccessViewVk::ezGALUnorderedAccessViewVk(
  ezGALResourceBase* pResource, const ezGALUnorderedAccessViewCreationDescription& Description)
  : ezGALUnorderedAccessView(pResource, Description)
{
}

ezGALUnorderedAccessViewVk::~ezGALUnorderedAccessViewVk() {}

ezResult ezGALUnorderedAccessViewVk::InitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezResult ezGALUnorderedAccessViewVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

EZ_STATICLINK_FILE(RendererVk, RendererVk_Resources_Implementation_UnorderedAccessViewVk);
