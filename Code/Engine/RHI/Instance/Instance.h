#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class ezRHIInstance : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIInstance() = default;
  virtual ezDynamicArray<ezRHIAdapter*> EnumerateAdapters() = 0;
};
