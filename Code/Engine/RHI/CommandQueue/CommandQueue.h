#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class EZ_RHI_DLL ezRHICommandQueue : public ezRHIQueryInterface
{
public:
  virtual ~ezRHICommandQueue() = default;
  virtual void Wait(const ezRHIFence* fence, ezUInt64 value) = 0;
  virtual void Signal(const ezRHIFence* fence, ezUInt64 value) = 0;
  virtual void ExecuteCommandLists(const ezDynamicArray<ezRHICommandList*>& commandLists) = 0;
};
