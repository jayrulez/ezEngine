
#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>

class EZ_RHI_DLL ezRHIBlendState : public ezRHIObject<ezRHIBlendStateCreationDescription>
{
public:
protected:
  ezRHIBlendState(const ezRHIBlendStateCreationDescription& Description);

  virtual ~ezRHIBlendState();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};

class EZ_RHI_DLL ezRHIDepthStencilState : public ezRHIObject<ezRHIDepthStencilStateCreationDescription>
{
public:
protected:
  ezRHIDepthStencilState(const ezRHIDepthStencilStateCreationDescription& Description);

  virtual ~ezRHIDepthStencilState();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};

class EZ_RHI_DLL ezRHIRasterizerState : public ezRHIObject<ezRHIRasterizerStateCreationDescription>
{
public:
protected:
  ezRHIRasterizerState(const ezRHIRasterizerStateCreationDescription& Description);

  virtual ~ezRHIRasterizerState();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};

class EZ_RHI_DLL ezRHISamplerState : public ezRHIObject<ezRHISamplerStateCreationDescription>
{
public:
protected:
  ezRHISamplerState(const ezRHISamplerStateCreationDescription& Description);

  virtual ~ezRHISamplerState();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};
