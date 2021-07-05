#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class ezRHISwapchain : public ezRHIQueryInterface
{
public:
  virtual ~ezRHISwapchain() = default;
  virtual ezRHIResourceFormat GetFormat() const = 0;
  virtual ezRHIResource* GetBackBuffer(ezUInt32 buffer) = 0;
  virtual ezUInt32 NextImage(const ezRHIFence* fence, ezUInt64 signalValue) = 0;
  virtual void Present(const ezRHIFence* fence, ezUInt64 waitValue) = 0;
};
