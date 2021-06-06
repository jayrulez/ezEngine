#include <RHI/Resources/Texture.h>

ezRHITexture::ezRHITexture(const ezRHITextureDesc& desc)
  : ezRHIResource(desc, ezRHIGPUResourceType::Texture)
{
}

ezRHITexture::~ezRHITexture()
{
}
