#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/RendererVkDLL.h>
#include <RendererVk/Resources/BufferVk.h>

ezGALBufferVk::ezGALBufferVk(const ezGALBufferCreationDescription& Description)
  : ezGALBuffer(Description)
{
}

ezGALBufferVk::~ezGALBufferVk() {}

ezResult ezGALBufferVk::InitPlatform(ezGALDevice* pDevice, ezArrayPtr<const ezUInt8> pInitialData)
{
  return EZ_FAILURE;
}

ezResult ezGALBufferVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

void ezGALBufferVk::SetDebugNamePlatform(const char* szName) const
{
}

EZ_STATICLINK_FILE(RendererVk, RendererVk_Resources_Implementation_BufferVk);
