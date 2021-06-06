#include <RHI/Resources/RenderTargetView.h>

ezRHIRenderTargetView::ezRHIRenderTargetView(const ezRHIRenderTargetViewDesc& desc)
  : ezRHIResource(desc, ezRHIGPUResourceType::RenderTargetView)
{
}

ezRHIRenderTargetView::~ezRHIRenderTargetView()
{
}
