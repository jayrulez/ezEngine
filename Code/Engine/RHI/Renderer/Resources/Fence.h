
#pragma once

#include <RHI/Renderer/Basics.h>

class EZ_RHI_DLL ezRHIFence : public ezRefCounted
{
protected:
  friend class ezRHIRenderDeviceImpl;

  ezRHIFence();

  virtual ~ezRHIFence();

  virtual ezResult InitPlatform(ezRHIRenderDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIRenderDevice* pDevice) = 0;
};
