#include <RHI/Resources/Texture.h>
#include <RHI/Device/GraphicsDevice.h>

RHITextureView* RHITexture::GetFullTextureView(RHIGraphicsDevice* graphicsDevice)
{
  _fullTextureViewLock.Lock();

  if (_fullTextureView == nullptr)
  {
    _fullTextureView = CreateFullTextureView(graphicsDevice);
  }

  return _fullTextureView;
}

RHITextureView* RHITexture::CreateFullTextureView(RHIGraphicsDevice* graphicsDevice)
{
  return graphicsDevice->GetResourceFactory()->CreateTextureView(this);
}
