#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Shader/ShaderVk.h>

ezGALShaderVk::ezGALShaderVk(const ezGALShaderCreationDescription& Description)
  : ezGALShader(Description)
{
}

ezGALShaderVk::~ezGALShaderVk() {}

void ezGALShaderVk::SetDebugName(const char* szName) const
{
}

ezResult ezGALShaderVk::InitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezResult ezGALShaderVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}



EZ_STATICLINK_FILE(RendererVk, RendererVk_Shader_Implementation_ShaderVk);
