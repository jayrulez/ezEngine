#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Shader/VertexDeclarationVk.h>
#include <RendererFoundation/Shader/Shader.h>

ezGALVertexDeclarationVk::ezGALVertexDeclarationVk(const ezGALVertexDeclarationCreationDescription& Description)
  : ezGALVertexDeclaration(Description)
{
}

ezGALVertexDeclarationVk::~ezGALVertexDeclarationVk() = default;

ezResult ezGALVertexDeclarationVk::InitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezResult ezGALVertexDeclarationVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}



EZ_STATICLINK_FILE(RendererVk, RendererVk_Shader_Implementation_VertexDeclarationVk);
