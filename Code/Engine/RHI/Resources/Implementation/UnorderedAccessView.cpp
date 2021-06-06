#include <RHI/Resources/UnorderedAccessView.h>

ezRHIUnorderedAccessView::ezRHIUnorderedAccessView(const ezRHIUnorderedAccessViewDesc& desc)
  : ezRHIResource(desc, ezRHIGPUResourceType::UnorderedAccessView)
{
}

ezRHIUnorderedAccessView::~ezRHIUnorderedAccessView()
{
}
