
#pragma once

#include <RendererFoundation/Resources/RenderTargetView.h>

class ezGALRenderTargetViewVk : public ezGALRenderTargetView
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALRenderTargetViewVk(ezGALTexture* pTexture, const ezGALRenderTargetViewCreationDescription& Description);

  virtual ~ezGALRenderTargetViewVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
};

#include <RendererVk/Resources/Implementation/RenderTargetViewVk_inl.h>
