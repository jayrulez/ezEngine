#include <RHIPCH.h>

#include <RHI/RHIDLL.h>
#include <RHI/State/SamplerState.h>

ezRHISamplerState::ezRHISamplerState(const ezRHISamplerStateDesc& desc)
  : m_Desc(desc)
{
}

ezRHISamplerState::~ezRHISamplerState()
{
}
