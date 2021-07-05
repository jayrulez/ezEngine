#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class ezRHIMemory : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIMemory() = default;
  virtual ezRHIMemoryType GetMemoryType() const = 0;
};
