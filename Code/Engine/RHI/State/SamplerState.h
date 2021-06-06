#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHISamplerState : public ezRHIObject
{
public:
  ezRHISamplerState() = default;
  virtual ~ezRHISamplerState();

  const ezRHISamplerStateDesc& GetDesc() const
  {
    return m_Desc;
  }

protected:
  ezRHISamplerState(const ezRHISamplerStateDesc& desc);

private:
  ezRHISamplerStateDesc m_Desc;
};
