#include <RHI/Framebuffer/FramebufferBase.h>
#include <RHI/RenderPass/RenderPass.h>
#include <RHI/Resource/Resource.h>
#include <RHI/Shader/Shader.h>
#include <RHI/Framebuffer/Framebuffer.h>
#include <RHI/Program/Program.h>

FramebufferBase::FramebufferBase(const FramebufferDesc& desc)
    : m_desc(desc)
{
}

const FramebufferDesc& FramebufferBase::GetDesc() const
{
    return m_desc;
}


EZ_STATICLINK_FILE(RHI, RHI_Framebuffer_FramebufferBase);

