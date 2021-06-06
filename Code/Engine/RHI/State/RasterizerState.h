#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIRasterizerState : public ezRHIObject
{
public:
  const ezRHIRasterizerStateCreationDescription& GetDesc() const
  {
    return m_Desc;
  }

protected:
  ezRHIRasterizerState(const ezRHIRasterizerStateCreationDescription& desc);

  virtual ~ezRHIRasterizerState();

private:
  ezRHIRasterizerStateCreationDescription m_Desc;
};
