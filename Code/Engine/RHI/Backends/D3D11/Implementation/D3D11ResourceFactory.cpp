#include <RHI/Backends/D3D11/D3D11ResourceFactory.h>
#include <RHI/ValidationHelpers.h>

ezEnum<RHIGraphicsBackend> D3D11ResourceFactory::GetBackendType() const
{
  return GraphicsDevice->GetBackendType();
}

D3D11ResourceFactory::D3D11ResourceFactory(D3D11GraphicsDevice* graphicsDevice)
  : RHIResourceFactory(graphicsDevice->GetFeatures())
{
  GraphicsDevice = graphicsDevice;
  Device = graphicsDevice->GetDevice();
  //Cache = new D3D11ResourceCache(Device);
  Cache = EZ_DEFAULT_NEW(D3D11ResourceCache, Device);
}

void D3D11ResourceFactory::Dispose()
{
  if (!Cache->IsDisposed())
  {
    Cache->Dispose();
    EZ_DEFAULT_DELETE(Cache);
  }
}

RHICommandList* D3D11ResourceFactory::CreateCommandListCore(const RHICommandListDescription& description)
{
  //return new D3D11CommandList(GraphicsDevice, description);
  return EZ_DEFAULT_NEW(D3D11CommandList, GraphicsDevice, description);
}

RHIFramebuffer* D3D11ResourceFactory::CreateFramebufferCore(const RHIFramebufferDescription& description)
{
  //return new D3D11Framebuffer(Device, description);
  return EZ_DEFAULT_NEW(D3D11Framebuffer, Device, description);
}

RHIPipeline* D3D11ResourceFactory::CreateGraphicsPipelineCore(const RHIGraphicsPipelineDescription& description)
{
  //return new D3D11Pipeline(Cache, description);
  return EZ_DEFAULT_NEW(D3D11Pipeline, Cache, description);
}

RHIPipeline* D3D11ResourceFactory::CreateComputePipelineCore(const RHIComputePipelineDescription& description)
{
  //return new D3D11Pipeline(Cache, description);
  return EZ_DEFAULT_NEW(D3D11Pipeline, Cache, description);
}

RHITexture* D3D11ResourceFactory::CreateTextureCore(ezUInt64 nativeTexture, const RHITextureDescription& description)
{
  // TODO: this is sus, figure out what to do here
  ID3D11Texture2D* existingTexture = reinterpret_cast<ID3D11Texture2D*>(&nativeTexture);
  //return new D3D11Texture(existingTexture, description.Type, description.Format);
  return EZ_DEFAULT_NEW(D3D11Texture, existingTexture, description.Type, description.Format);
}

RHITexture* D3D11ResourceFactory::CreateTextureCore(const RHITextureDescription& description)
{
  //return new D3D11Texture(Device, description);
  return EZ_DEFAULT_NEW(D3D11Texture, Device, description);
}

RHITextureView* D3D11ResourceFactory::CreateTextureViewCore(const RHITextureViewDescription& description)
{
  //return new D3D11TextureView(GraphicsDevice, description);
  return EZ_DEFAULT_NEW(D3D11TextureView, GraphicsDevice, description);
}

RHIBuffer* D3D11ResourceFactory::CreateBufferCore(const RHIBufferDescription& description)
{
  //return new D3D11DeviceBuffer(
  //  Device,
  //  description.Size,
  //  description.Usage,
  //  description.StructureByteStride,
  //  description.RawBuffer);

  return EZ_DEFAULT_NEW(
    D3D11DeviceBuffer, 
    Device,
    description.Size,
    description.Usage,
    description.StructureByteStride,
    description.RawBuffer);
}

RHISampler* D3D11ResourceFactory::CreateSamplerCore(const RHISamplerDescription& description)
{
  //return new D3D11Sampler(Device, description);
  return EZ_DEFAULT_NEW(D3D11Sampler, Device, description);
}

RHIShader* D3D11ResourceFactory::CreateShaderCore(const RHIShaderDescription& description)
{
  //return new D3D11Shader(Device, description);
  return EZ_DEFAULT_NEW(D3D11Shader, Device, description);
}

RHIResourceLayout* D3D11ResourceFactory::CreateResourceLayoutCore(const RHIResourceLayoutDescription& description)
{
  //return new D3D11ResourceLayout(description);
  return EZ_DEFAULT_NEW(D3D11ResourceLayout, description);
}

RHIResourceSet* D3D11ResourceFactory::CreateResourceSetCore(const RHIResourceSetDescription& description)
{
  ValidationHelpers::ValidateResourceSet(GraphicsDevice, description);
  //return new D3D11ResourceSet(description);
  return EZ_DEFAULT_NEW(D3D11ResourceSet, description);
}

RHIFence* D3D11ResourceFactory::CreateFenceCore(bool signaled)
{
  //return new D3D11Fence(signaled);
  return EZ_DEFAULT_NEW(D3D11Fence, signaled);
}

RHISwapchain* D3D11ResourceFactory::CreateSwapchainCore(const RHISwapchainDescription& description)
{
  //return new D3D11Swapchain(GraphicsDevice, description);
  return EZ_DEFAULT_NEW(D3D11Swapchain, GraphicsDevice, description);
}


EZ_STATICLINK_FILE(RHI, RHI_Backends_D3D11_Implementation_D3D11ResourceFactory);
