#include <RHIPCH.h>

#include <RHI/Framebuffer/FramebufferBase.h>

ezRHIFramebufferBase::ezRHIFramebufferBase(const ezRHIFramebufferCreationDescription& desc)
  :m_Description{desc}
{
}

const ezRHIFramebufferCreationDescription& ezRHIFramebufferBase::GetDescription() const
{
  return m_Description;
}
