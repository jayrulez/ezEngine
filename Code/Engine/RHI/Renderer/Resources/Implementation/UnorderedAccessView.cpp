#include <RHIPCH.h>

#include <RHI/Renderer/Resources/UnorderedAccesView.h>

ezRHIUnorderedAccessView::ezRHIUnorderedAccessView(ezRHIResourceBase* pResource, const ezRHIUnorderedAccessViewCreationDescription& description)
  : ezRHIObject(description)
  , m_pResource(pResource)
{
  EZ_ASSERT_DEV(m_pResource != nullptr, "Resource must not be null");
}

ezRHIUnorderedAccessView::~ezRHIUnorderedAccessView() {}

EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Resources_Implementation_UnorderedAccessView);
