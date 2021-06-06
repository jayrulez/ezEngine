#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIRenderPass: public ezRHIObject
{
public:
  ezUInt64 GetHash() const { return m_Hash; }

  const ezRHIRenderPassDesc& GetDesc() const
  {
    return m_Desc;
  }

protected:
  ezRHIRenderPass(const ezRHIRenderPassDesc& desc);

  virtual ~ezRHIRenderPass();

private:
  ezUInt64 m_Hash;
  ezRHIRenderPassDesc m_Desc;
};
