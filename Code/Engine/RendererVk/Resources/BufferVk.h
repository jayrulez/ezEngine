
#pragma once

#include <RendererFoundation/Resources/Buffer.h>

class EZ_RENDERERVK_DLL ezGALBufferVk : public ezGALBuffer
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALBufferVk(const ezGALBufferCreationDescription& Description);

  virtual ~ezGALBufferVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice, ezArrayPtr<const ezUInt8> pInitialData) override;
  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;

  virtual void SetDebugNamePlatform(const char* szName) const override;
};

#include <RendererVk/Resources/Implementation/BufferVk_inl.h>
