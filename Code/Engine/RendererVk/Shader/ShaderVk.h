
#pragma once

#include <RendererVk/RendererVkDLL.h>
#include <RendererFoundation/RendererFoundationDLL.h>
#include <RendererFoundation/Shader/Shader.h>

class EZ_RENDERERVK_DLL ezGALShaderVk : public ezGALShader
{
public:
  void SetDebugName(const char* szName) const override;

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALShaderVk(const ezGALShaderCreationDescription& description);

  virtual ~ezGALShaderVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;
};

#include <RendererVk/Shader/Implementation/ShaderVk_inl.h>
