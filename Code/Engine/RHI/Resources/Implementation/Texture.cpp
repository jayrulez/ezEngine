#include <RHI/Resources/Texture.h>
#include <RHI/Resources/TextureView.h>
#include <RHI/Device/GraphicsDevice.h>

void RHITexture::Dispose()
{
  ezLock lock(_fullTextureViewLock);
  if (_fullTextureView != nullptr)
  {
    _fullTextureView->Dispose();
    EZ_DEFAULT_DELETE(_fullTextureView);
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


EZ_STATICLINK_FILE(RHI, RHI_Resources_Implementation_Texture);

