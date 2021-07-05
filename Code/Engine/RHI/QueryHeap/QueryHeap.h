#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class ezRHIQueryHeap : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIQueryHeap() = default;
  virtual ezRHIQueryHeapType GetType() const = 0;
};
