
#pragma once

#include <RendererVk/RendererVkDLL.h>
#include <RendererFoundation/Descriptors/Descriptors.h>
#include <RendererFoundation/Device/SwapChain.h>

class ezGALSwapChainVk : public ezGALSwapChain
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALSwapChainVk(const ezGALSwapChainCreationDescription& Description);

  virtual ~ezGALSwapChainVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;

};

#include <RendererVk/Device/Implementation/SwapChainVk_inl.h>
