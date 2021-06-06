#include <RHIPCH.h>

#include <RHI/RHIDLL.h>
#include <RHI/State/RaytracingPipelineState.h>

ezRHIRaytracingPipelineState::ezRHIRaytracingPipelineState(const ezRHIRaytracingPipelineStateDesc& desc)
  : m_Desc{desc}
{
}

ezRHIRaytracingPipelineState::~ezRHIRaytracingPipelineState() {}
