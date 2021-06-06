#include <RHI/Device/RenderPass.h>

ezRHIRenderPass::ezRHIRenderPass(const ezRHIRenderPassDesc& desc)
  : m_Desc(desc)
  , m_Hash{0}
{
}

ezRHIRenderPass::~ezRHIRenderPass()
{
}
