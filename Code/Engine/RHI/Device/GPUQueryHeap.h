#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIGPUQueryHeap : public ezRHIObject
{
public:
  const ezRHIGPUQueryHeapDesc& GetDesc() const
  {
    return m_Desc;
  }

protected:
  ezRHIGPUQueryHeap(const ezRHIGPUQueryHeapDesc& desc);

  virtual ~ezRHIGPUQueryHeap();

private:
  ezRHIGPUQueryHeapDesc m_Desc;
};
