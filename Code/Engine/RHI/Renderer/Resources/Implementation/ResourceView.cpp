#include <RHIPCH.h>

#include <RHI/Renderer/Resources/ResourceView.h>


ezRHIResourceView::ezRHIResourceView(ezRHIResourceBase* pResource, const ezRHIResourceViewCreationDescription& description)
  : ezRHIObject(description)
  , m_pResource(pResource)
{
  EZ_ASSERT_DEV(m_pResource != nullptr, "Resource must not be null");
}

ezRHIResourceView::~ezRHIResourceView() {}



EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Resources_Implementation_ResourceView);
