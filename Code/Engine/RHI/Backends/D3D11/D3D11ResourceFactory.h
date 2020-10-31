#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Backends/D3D11/D3D11GraphicsDevice.h>
#include <RHI/Backends/D3D11/D3D11ResourceCache.h>
#include <RHI/Resources/ResourceFactory.h>


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

  D3D11ResourceFactory(D3D11GraphicsDevice* graphicsDevice);

  void Dispose();

public:
  virtual RHICommandList* CreateCommandListCore(const RHICommandListDescription& description) override;

  virtual RHIFramebuffer* CreateFramebufferCore(const RHIFramebufferDescription& description) override;

  virtual RHIPipeline* CreateGraphicsPipelineCore(const RHIGraphicsPipelineDescription& description) override;

  virtual RHIPipeline* CreateComputePipelineCore(const RHIComputePipelineDescription& description) override;

  virtual RHITexture* CreateTextureCore(ezUInt64 nativeTexture, const RHITextureDescription& description) override;
  virtual RHITexture* CreateTextureCore(const RHITextureDescription& description) override;

  virtual RHITextureView* CreateTextureViewCore(const RHITextureViewDescription& description) override;

  virtual RHIBuffer* CreateBufferCore(const RHIBufferDescription& description) override;

  virtual RHISampler* CreateSamplerCore(const RHISamplerDescription& description) override { return new D3D11Sampler(Device, description); }

  virtual RHIShader* CreateShaderCore(const RHIShaderDescription& description) override { return new D3D11Shader(Device, description); }

  virtual RHIResourceLayout* CreateResourceLayoutCore(const RHIResourceLayoutDescription& description) override { return new D3D11ResourceLayout(description); }

  virtual RHIResourceSet* CreateResourceSetCore(const RHIResourceSetDescription& description) override;
  virtual RHIFence* CreateFenceCore(bool signaled) override { return new D3D11Fence(signaled); }

  virtual RHISwapchain* CreateSwapchainCore(const RHISwapchainDescription& description) override { return new D3D11Swapchain(GraphicsDevice, description); }

private:
  D3D11GraphicsDevice* GraphicsDevice = nullptr;
  ID3D11Device* Device = nullptr;
  D3D11ResourceCache* Cache = nullptr;
};
