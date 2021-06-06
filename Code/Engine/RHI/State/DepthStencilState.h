#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIDepthStencilState : public ezRHIObject
{
public:
  const ezRHIDepthStencilStateCreationDescription& GetDesc() const
  {
    return m_Desc;
  }

protected:
  ezRHIDepthStencilState(const ezRHIDepthStencilStateCreationDescription& desc);

  virtual ~ezRHIDepthStencilState();

private:
  ezRHIDepthStencilStateCreationDescription m_Desc;
};
