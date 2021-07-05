#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class EZ_RHI_DLL ezRHIFence : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIFence() = default;
  virtual ezUInt64 GetCompletedValue() = 0;
  virtual void Wait(ezUInt64 value) = 0;
  virtual void Signal(ezUInt64 value) = 0;
};
