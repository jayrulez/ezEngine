#include <RHI/Backends/D3D11/D3D11ResourceFactory.h>

D3D11ResourceFactory::D3D11ResourceFactory(D3D11GraphicsDevice* graphicsDevice)
  : RHIResourceFactory(graphicsDevice->GetFeatures())
{
  GraphicsDevice = graphicsDevice;
  Device = graphicsDevice->GetDevice();
  Cache = new D3D11ResourceCache(Device);
}

void D3D11ResourceFactory::Dispose()
{
  if (!Cache->IsDisposed())
  {
    Cache->Dispose();
  }
}

RHICommandList* D3D11ResourceFactory::CreateCommandListCore(const RHICommandListDescription& description)
{
  return new D3D11CommandList(GraphicsDevice, description);
}

RHIFramebuffer* D3D11ResourceFactory::CreateFramebufferCore(const RHIFramebufferDescription& description)
{
  new D3D11Framebuffer(Device, description);
}

RHIPipeline* D3D11ResourceFactory::CreateGraphicsPipelineCore(const RHIGraphicsPipelineDescription& description)
{
  return new D3D11Pipeline(Cache, description);
}

RHIPipeline* D3D11ResourceFactory::CreateComputePipelineCore(const RHIComputePipelineDescription& description)
{
  return new D3D11Pipeline(Cache, description);
}

RHITexture* D3D11ResourceFactory::CreateTextureCore(ezUInt64 nativeTexture, const RHITextureDescription& description)
{
  // TODO: this is sus, figure out what to do here
  ID3D11Texture2D* existingTexture = reinterpret_cast<ID3D11Texture2D*>(&nativeTexture);
  return new D3D11Texture(existingTexture, description.Type, description.Format);
}

 RHITexture* D3D11ResourceFactory::CreateTextureCore(const RHITextureDescription& description)
{
  return new D3D11Texture(Device, description);
}

RHITextureView* D3D11ResourceFactory::CreateTextureViewCore(const RHITextureViewDescription& description)
{
  return new D3D11TextureView(GraphicsDevice, description);
}

RHIDeviceBuffer* D3D11ResourceFactory::CreateBufferCore(const RHIBufferDescription& description)
{
  return new D3D11DeviceBuffer(
    Device,
    description.Size,
    description.Usage,
    description.StructureByteStride,
    description.RawBuffer);
}
