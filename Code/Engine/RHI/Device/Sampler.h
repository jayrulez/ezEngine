#pragma once

#include <RHI/State/SamplerState.h>

class EZ_RHI_DLL ezRHIStaticSampler
{
public:
  ezRHIStaticSampler();
private:
  ezRHISamplerState m_Sampler;
  ezUInt32 m_Slot;
};

