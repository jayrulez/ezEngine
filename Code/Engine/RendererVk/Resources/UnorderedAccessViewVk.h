
#pragma once

#include <RendererFoundation/Resources/UnorderedAccesView.h>

class ezGALUnorderedAccessViewVk : public ezGALUnorderedAccessView
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALUnorderedAccessViewVk(ezGALResourceBase* pResource, const ezGALUnorderedAccessViewCreationDescription& Description);

  ~ezGALUnorderedAccessViewVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
};

#include <RendererVk/Resources/Implementation/UnorderedAccessViewVk_inl.h>
