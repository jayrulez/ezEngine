#pragma once

#include <RHI/Device/Sampler.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIShader : ezRHIObject
{
public:
  virtual void SetDebugName(const char* name) const = 0;

protected:
  friend class ezRHIDevice;

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;
  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;

private:
  ezRHIShaderStage::Enum m_Stage = ezRHIShaderStage::ENUM_COUNT;
  ezDynamicArray<ezRHIStaticSampler> m_AutoSamplers; //ability to set static samplers without explicit root signature
};
