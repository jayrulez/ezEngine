#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIRaytracingPipelineState : public ezRHIObject
{
public:
  const ezRHIRaytracingPipelineStateDesc& GetDesc() const
  {
    return m_Desc;
  }

protected:
  ezRHIRaytracingPipelineState(const ezRHIRaytracingPipelineStateDesc& desc);

  virtual ~ezRHIRaytracingPipelineState();

private:
  ezRHIRaytracingPipelineStateDesc m_Desc;
};
