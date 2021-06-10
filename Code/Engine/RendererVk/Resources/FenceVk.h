
#pragma once

#include <RendererFoundation/Resources/Fence.h>

class EZ_RENDERERVK_DLL ezGALFenceVk : public ezGALFence
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALFenceVk();

  virtual ~ezGALFenceVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
};
