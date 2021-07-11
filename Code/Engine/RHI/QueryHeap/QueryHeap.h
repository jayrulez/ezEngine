#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/QueryInterface.h>

class EZ_RHI_DLL QueryHeap : public QueryInterface
{
public:
  virtual ~QueryHeap() = default;
  virtual QueryHeapType GetType() const = 0;
};
