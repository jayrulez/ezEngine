#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>
#include <RHI/Framebuffer/Framebuffer.h>

class EZ_RHI_DLL ezRHIFramebufferBase : public ezRHIFramebuffer
{
public:
  ezRHIFramebufferBase(const ezRHIFramebufferCreationDescription& desc);
  const ezRHIFramebufferCreationDescription& GetDescription() const;

private:
  ezRHIFramebufferCreationDescription m_Description;
};
