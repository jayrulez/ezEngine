
#pragma once

#include <RendererFoundation/State/State.h>

class EZ_RENDERERVK_DLL ezGALBlendStateVk : public ezGALBlendState
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALBlendStateVk(const ezGALBlendStateCreationDescription& Description);

  ~ezGALBlendStateVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
};

class EZ_RENDERERVK_DLL ezGALDepthStencilStateVk : public ezGALDepthStencilState
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALDepthStencilStateVk(const ezGALDepthStencilStateCreationDescription& Description);

  ~ezGALDepthStencilStateVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
};

class EZ_RENDERERVK_DLL ezGALRasterizerStateVk : public ezGALRasterizerState
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALRasterizerStateVk(const ezGALRasterizerStateCreationDescription& Description);

  ~ezGALRasterizerStateVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
};

class EZ_RENDERERVK_DLL ezGALSamplerStateVk : public ezGALSamplerState
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALSamplerStateVk(const ezGALSamplerStateCreationDescription& Description);

  ~ezGALSamplerStateVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
};


#include <RendererVk/State/Implementation/StateVk_inl.h>
