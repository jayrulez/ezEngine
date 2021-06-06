#include <RHIPCH.h>

#include <RHI/RHIDLL.h>
#include <RHI/State/DepthStencilState.h>

ezRHIDepthStencilState::ezRHIDepthStencilState(const ezRHIDepthStencilStateCreationDescription& desc)
  : m_Desc(desc)
{
}

ezRHIDepthStencilState::~ezRHIDepthStencilState()
{
}
