#include <RHIPCH.h>

#include <RHI/Renderer/Resources/RenderTargetView.h>


ezRHIRenderTargetView::ezRHIRenderTargetView(ezRHITexture* pTexture, const ezRHIRenderTargetViewCreationDescription& description)
  : ezRHIObject(description)
  , m_pTexture(pTexture)
{
  EZ_ASSERT_DEV(m_pTexture != nullptr, "Texture must not be null");
}

ezRHIRenderTargetView::~ezRHIRenderTargetView() {}



EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Resources_Implementation_RenderTargetView);
