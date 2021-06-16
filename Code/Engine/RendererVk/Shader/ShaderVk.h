
#pragma once

#include <RendererFoundation/RendererFoundationDLL.h>
#include <RendererFoundation/Shader/Shader.h>
#include <RendererVk/RendererVkDLL.h>

class EZ_RENDERERVK_DLL ezGALShaderVk : public ezGALShader
{
public:
  void SetDebugName(const char* szName) const override;

  EZ_ALWAYS_INLINE VkShaderModule GetVkShaderModule() const;
  EZ_ALWAYS_INLINE VkShaderStageFlagBits GetVkShaderStage() const;

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALShaderVk(const ezGALShaderCreationDescription& description);

  virtual ~ezGALShaderVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;

private:
  VkShaderModule m_pVkShaderModule;
  VkShaderStageFlagBits m_VkShaderStage;
  ezGALDeviceVk* m_pDeviceVk;
};

#include <RendererVk/Shader/Implementation/ShaderVk_inl.h>
