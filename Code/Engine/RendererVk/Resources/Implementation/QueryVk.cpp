#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Resources/QueryVk.h>

ezGALQueryVk::ezGALQueryVk(const ezGALQueryCreationDescription& Description)
  : ezGALQuery(Description)
{
}

ezGALQueryVk::~ezGALQueryVk() {}

ezResult ezGALQueryVk::InitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezResult ezGALQueryVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

void ezGALQueryVk::SetDebugNamePlatform(const char* szName) const
{
}

EZ_STATICLINK_FILE(RendererVk, RendererVk_Resources_Implementation_QueryVk);
