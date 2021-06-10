#include <RendererVkPCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Device/SwapChainVk.h>

#include <Foundation/Basics/Platform/Win/HResultUtils.h>

ezGALSwapChainVk::ezGALSwapChainVk(const ezGALSwapChainCreationDescription& Description)
  : ezGALSwapChain(Description)
{
}

ezGALSwapChainVk::~ezGALSwapChainVk() {}


ezResult ezGALSwapChainVk::InitPlatform(ezGALDevice* pDevice)
{  
  return EZ_SUCCESS;
}

ezResult ezGALSwapChainVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}



EZ_STATICLINK_FILE(RendererVk, RendererVk_Device_Implementation_SwapChainVk);
