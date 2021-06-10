#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/RendererVkDLL.h>
#include <RendererVk/State/StateVk.h>

// Mapping tables to map ezGAL constants to Vk constants
#include <RendererVk/State/Implementation/StateVk_MappingTables.inl>

// Blend state

ezGALBlendStateVk::ezGALBlendStateVk(const ezGALBlendStateCreationDescription& Description)
  : ezGALBlendState(Description)
{
}

ezGALBlendStateVk::~ezGALBlendStateVk() {}

ezResult ezGALBlendStateVk::InitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezResult ezGALBlendStateVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

// Depth Stencil state

ezGALDepthStencilStateVk::ezGALDepthStencilStateVk(const ezGALDepthStencilStateCreationDescription& Description)
  : ezGALDepthStencilState(Description)
{
}

ezGALDepthStencilStateVk::~ezGALDepthStencilStateVk() {}

ezResult ezGALDepthStencilStateVk::InitPlatform(ezGALDevice* pDevice)
{
    return EZ_SUCCESS;
}

ezResult ezGALDepthStencilStateVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}


// Rasterizer state

ezGALRasterizerStateVk::ezGALRasterizerStateVk(const ezGALRasterizerStateCreationDescription& Description)
  : ezGALRasterizerState(Description)
{
}

ezGALRasterizerStateVk::~ezGALRasterizerStateVk() {}



ezResult ezGALRasterizerStateVk::InitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}


ezResult ezGALRasterizerStateVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}


// Sampler state

ezGALSamplerStateVk::ezGALSamplerStateVk(const ezGALSamplerStateCreationDescription& Description)
  : ezGALSamplerState(Description)
{
}

ezGALSamplerStateVk::~ezGALSamplerStateVk() {}

/*
 */

ezResult ezGALSamplerStateVk::InitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}


ezResult ezGALSamplerStateVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}



EZ_STATICLINK_FILE(RendererVk, RendererVk_State_Implementation_StateVk);
