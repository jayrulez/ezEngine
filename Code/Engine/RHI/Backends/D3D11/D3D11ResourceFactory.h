#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Backends/D3D11/D3D11GraphicsDevice.h>
#include <RHI/Backends/D3D11/D3D11ResourceCache.h>
#include <RHI/Device/ResourceFactory.h>


#include <RHI/Backends/D3D11/D3D11Buffer.h>
#include <RHI/Backends/D3D11/D3D11CommandList.h>
#include <RHI/Backends/D3D11/D3D11Fence.h>
#include <RHI/Backends/D3D11/D3D11Framebuffer.h>
#include <RHI/Backends/D3D11/D3D11Pipeline.h>
#include <RHI/Backends/D3D11/D3D11ResourceLayout.h>
#include <RHI/Backends/D3D11/D3D11ResourceSet.h>
#include <RHI/Backends/D3D11/D3D11Sampler.h>
#include <RHI/Backends/D3D11/D3D11Shader.h>
#include <RHI/Backends/D3D11/D3D11Swapchain.h>
#include <RHI/Backends/D3D11/D3D11Texture.h>
#include <RHI/Backends/D3D11/D3D11TextureView.h>

class D3D11ResourceFactory : public RHIResourceFactory
{
public:
  virtual ezEnum<RHIGraphicsBackend> GetBackendType() const override
  {
    return RHIGraphicsBackend::Direct3D11;
  }

  D3D11ResourceFactory(D3D11GraphicsDevice* graphicsDevice)
    : RHIResourceFactory(graphicsDevice->GetFeatures())
  {
    GraphicsDevice = graphicsDevice;
    Device = graphicsDevice->GetDevice();
    Cache = new D3D11ResourceCache(Device);
  }

  void Dispose()
  {
    if (!Cache->IsDisposed())
    {
      Cache->Dispose();
    }
  }

public:
  virtual RHICommandList* CreateCommandListCore(const RHICommandListDescription& description) override { return new D3D11CommandList(GraphicsDevice, description); }

  virtual RHIFramebuffer* CreateFramebufferCore(const RHIFramebufferDescription& description) override { new D3D11Framebuffer(Device, description); }

  virtual RHIPipeline* CreateGraphicsPipelineCore(const RHIGraphicsPipelineDescription& description) override { return new D3D11Pipeline(Cache, description); }

  virtual RHIPipeline* CreateComputePipelineCore(const RHIComputePipelineDescription& description) override { return new D3D11Pipeline(Cache, description); }

  virtual RHITexture* CreateTextureCore(ezUInt64 nativeTexture, const RHITextureDescription& description) override
  {
    // TODO: this is sus, figure out what to do here
    ID3D11Texture2D* existingTexture = reinterpret_cast<ID3D11Texture2D*>(&nativeTexture);
    return new D3D11Texture(existingTexture, description.Type, description.Format);
  }
  virtual RHITexture* CreateTextureCore(const RHITextureDescription& description) override { return new D3D11Texture(Device, description); }

  virtual RHITextureView* CreateTextureViewCore(const RHITextureViewDescription& description) override { return new D3D11TextureView(GraphicsDevice, description); }

  virtual RHIDeviceBuffer* CreateBufferCore(const RHIBufferDescription& description) override
  {
    return new D3D11DeviceBuffer(
      Device,
      description.Size,
      description.Usage,
      description.StructureByteStride,
      description.RawBuffer);
  }

  virtual RHISampler* CreateSamplerCore(const RHISamplerDescription& description) override { return new D3D11Sampler(Device, description); }

  virtual RHIShader* CreateShaderCore(const RHIShaderDescription& description) override { return new D3D11Shader(Device, description); }

  virtual RHIResourceLayout* CreateResourceLayoutCore(const RHIResourceLayoutDescription& description) override { return new D3D11ResourceLayout(description); }

  virtual RHIResourceSet* CreateResourceSetCore(const RHIResourceSetDescription& description) override
  {
    RHIValidationUtils::ValidateResourceSet(GraphicsDevice, description);
    return new D3D11ResourceSet(description);
  }
  virtual RHIFence* CreateFenceCore(bool signaled) override { return new D3D11Fence(signaled); }

  virtual RHISwapchain* CreateSwapchainCore(const RHISwapchainDescription& description) override { return new D3D11Swapchain(GraphicsDevice, description); }

private:
  D3D11GraphicsDevice* GraphicsDevice = nullptr;
  ID3D11Device* Device = nullptr;
  D3D11ResourceCache* Cache = nullptr;
};
