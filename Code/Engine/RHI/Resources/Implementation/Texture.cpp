#include <RHI/Resources/Texture.h>
#include <RHI/Device/GraphicsDevice.h>

RHITextureView* RHITexture::CreateFullTextureView(RHIGraphicsDevice* graphicsDevice)
{
  return graphicsDevice->GetResourceFactory()->CreateTextureView(this);
}
