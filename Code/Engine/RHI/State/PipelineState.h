#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIPipelineState : public ezRHIObject
{
public:
  const ezRHIPipelineStateDesc& GetDesc() const
  {
    return m_Desc;
  }

  ezUInt64 GetHash() const { return m_Hash; }

protected:
  ezRHIPipelineState(const ezRHIPipelineStateDesc& desc);

  virtual ~ezRHIPipelineState();

private:
  ezUInt64 m_Hash;
  ezRHIPipelineStateDesc m_Desc;
};
