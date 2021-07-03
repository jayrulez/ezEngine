
#pragma once

#include <RHI/Renderer/Basics.h>

class EZ_RHI_DLL ezRHIFence : public ezRefCounted
{
protected:
  friend class ezRHIDevice;

  ezRHIFence();

  virtual ~ezRHIFence();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};
