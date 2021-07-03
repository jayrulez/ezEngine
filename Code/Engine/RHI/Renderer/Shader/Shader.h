#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Basics.h>

class EZ_RHI_DLL ezRHIShader : public ezRHIObject<ezRHIShaderCreationDescription>
{
public:
  virtual void SetDebugName(const char* szName) const = 0;

protected:
  friend class ezRHIRenderDeviceImpl;

  virtual ezResult InitPlatform(ezRHIRenderDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIRenderDevice* pDevice) = 0;

  ezRHIShader(const ezRHIShaderCreationDescription& Description);

  virtual ~ezRHIShader();
};
