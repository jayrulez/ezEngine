#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIQueryHeap : public ezRHIObject
{
public:
  const ezRHIQueryHeapDesc& GetDesc() const
  {
    return m_Desc;
  }

protected:
  ezRHIQueryHeap(const ezRHIQueryHeapDesc& desc);

  virtual ~ezRHIQueryHeap();

private:
  ezRHIQueryHeapDesc m_Desc;
};
