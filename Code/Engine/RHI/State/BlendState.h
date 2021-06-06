#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIBlendState : public ezRHIObject
{
public:
  const ezRHIBlendStateCreationDescription& GetDesc() const
  {
    return m_Desc;
  }

protected:
  ezRHIBlendState(const ezRHIBlendStateCreationDescription& desc);

  virtual ~ezRHIBlendState();

private:
  ezRHIBlendStateCreationDescription m_Desc;
};
