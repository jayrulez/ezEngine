#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Resources/TextureVk.h>

ezGALTextureVk::ezGALTextureVk(const ezGALTextureCreationDescription& Description)
  : ezGALTexture(Description)
  , m_pExisitingNativeObject(Description.m_pExisitingNativeObject)
{
}

ezGALTextureVk::~ezGALTextureVk() {}

ezResult ezGALTextureVk::InitPlatform(ezGALDevice* pDevice, ezArrayPtr<ezGALSystemMemoryDescription> pInitialData)
{
  return EZ_SUCCESS;
}

ezResult ezGALTextureVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezResult ezGALTextureVk::ReplaceExisitingNativeObject(void* pExisitingNativeObject)
{
  return EZ_SUCCESS;
}

void ezGALTextureVk::SetDebugNamePlatform(const char* szName) const
{
}

EZ_STATICLINK_FILE(RendererVk, RendererVk_Resources_Implementation_TextureVk);
