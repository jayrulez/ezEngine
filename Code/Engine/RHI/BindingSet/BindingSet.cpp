#include <RHIPCH.h>

#include <RHI/BindingSet/BindingSet.h>

void ezRHIBindingSet::WriteBindings(const ezDynamicArray<ezRHIBindingDescription>& bindings)
{
  WriteBindingsPlatform(bindings);
}
