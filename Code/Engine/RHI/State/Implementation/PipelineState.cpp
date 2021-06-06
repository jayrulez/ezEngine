#include <RHIPCH.h>

#include <RHI/RHIDLL.h>
#include <RHI/State/PipelineState.h>

ezRHIPipelineState::ezRHIPipelineState(const ezRHIPipelineStateDesc& desc)
  : m_Desc{desc}
  , m_Hash{0}
{
}

ezRHIPipelineState::~ezRHIPipelineState() {}
