#include <RHIPCH.h>

#include <RHI/Renderer/Resources/Texture.h>

ezRHITexture::ezRHITexture(const ezRHITextureCreationDescription& Description)
  : ezRHIResource(Description)
{
}

ezRHITexture::~ezRHITexture() {}



EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Resources_Implementation_Texture);
