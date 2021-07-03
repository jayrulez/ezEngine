#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Resource/Resource.h>
#include <RHI/Fence/Fence.h>
#include <RHI/RHIDLL.h>

using Window = void*;

class Swapchain : public QueryInterface
{
public:
    virtual ~Swapchain() = default;
  virtual ezRHIResourceFormat::Enum GetFormat() const = 0;
    virtual ezSharedPtr<Resource> GetBackBuffer(uint32_t buffer) = 0;
    virtual uint32_t NextImage(const ezSharedPtr<Fence>& fence, uint64_t signal_value) = 0;
    virtual void Present(const ezSharedPtr<Fence>& fence, uint64_t wait_value) = 0;
};
