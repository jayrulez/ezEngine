#pragma once

#include <RendererFoundation/Resources/Query.h>

class ezGALQueryVk : public ezGALQuery
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALQueryVk(const ezGALQueryCreationDescription& Description);
  ~ezGALQueryVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;
  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;

  virtual void SetDebugNamePlatform(const char* szName) const override;
};

#include <RendererVk/Resources/Implementation/QueryVk_inl.h>
