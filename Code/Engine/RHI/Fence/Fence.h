#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/QueryInterface.h>
#include <cstdint>

class EZ_RHI_DLL Fence : public QueryInterface
{
public:
  virtual ~Fence() = default;
  virtual uint64_t GetCompletedValue() = 0;
  virtual void Wait(uint64_t value) = 0;
  virtual void Signal(uint64_t value) = 0;
};
