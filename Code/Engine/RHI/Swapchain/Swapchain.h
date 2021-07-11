#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Fence/Fence.h>
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Resource/Resource.h>

using Window = void*;

class EZ_RHI_DLL Swapchain : public QueryInterface
{
public:
  virtual ~Swapchain() = default;
  virtual ezRHIResourceFormat::Enum GetFormat() const = 0;
  virtual std::shared_ptr<Resource> GetBackBuffer(uint32_t buffer) = 0;
  virtual uint32_t NextImage(const std::shared_ptr<Fence>& fence, uint64_t signal_value) = 0;
  virtual void Present(const std::shared_ptr<Fence>& fence, uint64_t wait_value) = 0;
};
