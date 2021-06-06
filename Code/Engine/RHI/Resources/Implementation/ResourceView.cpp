#include <RHI/Resources/ResourceView.h>

ezRHIResourceView::ezRHIResourceView(const ezRHIResourceViewDesc& desc)
  : ezRHIResource(desc, ezRHIGPUResourceType::ResourceView)
{
}

ezRHIResourceView::~ezRHIResourceView()
{
}
