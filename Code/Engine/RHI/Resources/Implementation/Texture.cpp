#include <RHI/Resources/Texture.h>
#include <RHI/Resources/TextureView.h>
#include <RHI/Device/GraphicsDevice.h>

void RHITexture::Dispose()
{
  ezLock lock(_fullTextureViewLock);
  if (_fullTextureView != nullptr)
  {
    _fullTextureView->Dispose();
    _fullTextureView = nullptr;
  }

  DisposeCore();
}

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
