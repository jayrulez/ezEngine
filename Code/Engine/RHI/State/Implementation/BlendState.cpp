#include <RHIPCH.h>

#include <RHI/RHIDLL.h>
#include <RHI/State/BlendState.h>

ezRHIBlendState::ezRHIBlendState(const ezRHIBlendStateCreationDescription& desc)
  : m_Desc(desc)
{
}

ezRHIBlendState::~ezRHIBlendState()
{
}
