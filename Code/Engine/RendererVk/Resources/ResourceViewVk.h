
#pragma once

#include <RendererFoundation/Resources/ResourceView.h>

class ezGALResourceViewVk : public ezGALResourceView
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALResourceViewVk(ezGALResourceBase* pResource, const ezGALResourceViewCreationDescription& Description);

  ~ezGALResourceViewVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
};

#include <RendererVk/Resources/Implementation/ResourceViewVk_inl.h>
