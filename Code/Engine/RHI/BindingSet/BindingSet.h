#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class EZ_RHI_DLL ezRHIBindingSet : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIBindingSet() = default;
  void WriteBindings(const ezDynamicArray<ezRHIBindingDescription>& bindings);

protected:
  virtual void WriteBindingsPlatform(const ezDynamicArray<ezRHIBindingDescription>& bindings) = 0;
};
