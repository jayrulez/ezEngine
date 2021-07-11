#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Framebuffer/Framebuffer.h>
#include <RHI/View/View.h>
#include <vector>

class EZ_RHI_DLL FramebufferBase
  : public Framebuffer
{
public:
  FramebufferBase(const FramebufferDesc& desc);
  const FramebufferDesc& GetDesc() const;

private:
  FramebufferDesc m_desc;
};
