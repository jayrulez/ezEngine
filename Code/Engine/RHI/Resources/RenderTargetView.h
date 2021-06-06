#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Resources/Resource.h>

class EZ_RHI_DLL ezRHIRenderTargetView : public ezRHIResource<ezRHIRenderTargetViewDesc>
{
protected:
  ezRHIRenderTargetView(const ezRHIRenderTargetViewDesc& desc);

  virtual ~ezRHIRenderTargetView();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;
  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};
