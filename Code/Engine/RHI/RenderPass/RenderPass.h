#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class ezRHIRenderPass : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIRenderPass() = default;
  virtual const ezRHIRenderPassCreationDescription& GetDescription() const = 0;
};
