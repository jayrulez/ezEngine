#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/GraphicsPipelineDescription.h>
#include <RHI/Descriptors/ComputePipelineDescription.h>
#include <RHI/Descriptors/VertexLayoutDescription.h>
#include <RHI/Descriptors/FramebufferDescription.h>
#include <RHI/Descriptors/TextureDescription.h>
#include <RHI/Descriptors/TextureViewDescription.h>
#include <RHI/Descriptors/BufferDescription.h>
#include <RHI/Descriptors/SamplerDescription.h>
#include <RHI/Descriptors/ShaderDescription.h>
#include <RHI/Descriptors/CommandListDescription.h>
#include <RHI/Descriptors/ResourceLayoutDescription.h>
#include <RHI/Descriptors/ResourceSetDescription.h>
#include <RHI/Descriptors/SwapchainDescription.h>

#include <RHI/Resources/Pipeline.h>
#include <RHI/Resources/Framebuffer.h>
#include <RHI/Resources/Texture.h>
#include <RHI/Resources/TextureView.h>
#include <RHI/Resources/Buffer.h>
#include <RHI/Resources/Sampler.h>
#include <RHI/Resources/Shader.h>
#include <RHI/Resources/CommandList.h>
#include <RHI/Resources/ResourceLayout.h>
#include <RHI/Resources/ResourceSet.h>
#include <RHI/Resources/Fence.h>
#include <RHI/Resources/Swapchain.h>

/// <summary>
/// A device object responsible for the creation of graphics resources.
/// </summary>
class RHIResourceFactory
{
public:
  /// <summary>
  /// Gets the <see cref="GraphicsDeviceFeatures"/> this instance was created with.
  /// </summary>
  RHIGraphicsDeviceFeatures GetFeatures() const;

  /// <summary>
  /// Gets the <see cref="GraphicsBackend"/> of this instance.
  /// </summary>
  virtual ezEnum<RHIGraphicsBackend> GetBackendType() const = 0;

  /// <summary>
  /// Creates a new <see cref="Pipeline"/> object.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="Pipeline"/> which, when bound to a CommandList, is used to dispatch draw commands.</returns>
  RHIPipeline* CreateGraphicsPipeline(const RHIGraphicsPipelineDescription& description);

  /// <summary>
  /// Creates a new compute <see cref="Pipeline"/> object.
  /// </summary>
  /// <param name="description">The desirede properties of the created object.</param>
  /// <returns>A new <see cref="Pipeline"/> which, when bound to a CommandList, is used to dispatch compute commands.</returns>
  virtual RHIPipeline* CreateComputePipeline(const RHIComputePipelineDescription& description);

  /// <summary>
  /// Creates a new <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="Framebuffer"/>.</returns>
  virtual RHIFramebuffer* CreateFramebuffer(const RHIFramebufferDescription& description);

  /// <summary>
  /// Creates a new <see cref="Texture"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="Texture"/>.</returns>
  RHITexture* CreateTexture(const RHITextureDescription& description);

  /// <summary>
  /// Creates a new <see cref="Texture"/> from an existing native texture.
  /// </summary>
  /// <param name="nativeTexture">A backend-specific handle identifying an existing native texture. See remarks.</param>
  /// <param name="description">The properties of the existing Texture.</param>
  /// <returns>A new <see cref="Texture"/> wrapping the existing native texture.</returns>
  /// <remarks>
  /// The nativeTexture parameter is backend-specific, and the type of data passed in depends on which graphics API is
  /// being used.
  /// When using the Vulkan backend, nativeTexture must be a valid VkImage handle.
  /// When using the Metal backend, nativeTexture must be a valid MTLTexture pointer.
  /// When using the D3D11 backend, nativeTexture must be a valid pointer to an ID3D11Texture1D, ID3D11Texture2D, or
  /// ID3D11Texture3D.
  /// When using the OpenGL backend, nativeTexture must be a valid OpenGL texture name.
  /// The properties of the Texture will be determined from the <see cref="TextureDescription"/> passed in. These
  /// properties must match the true properties of the existing native texture.
  /// </remarks>
  RHITexture* CreateTexture(ezUInt64 nativeTexture, const RHITextureDescription& description);

  /// <summary>
  /// Creates a new <see cref="TextureView"/>.
  /// </summary>
  /// <param name="target">The target <see cref="Texture"/> used in the new view.</param>
  /// <returns>A new <see cref="TextureView"/>.</returns>
  RHITextureView* CreateTextureView(RHITexture* target);

  /// <summary>
  /// Creates a new <see cref="TextureView"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="TextureView"/>.</returns>
  RHITextureView* CreateTextureView(const RHITextureViewDescription& description);

  /// <summary>
  /// Creates a new <see cref="DeviceBuffer"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="DeviceBuffer"/>.</returns>
  RHIBuffer* CreateBuffer(const RHIBufferDescription& description);

  /// <summary>
  /// Creates a new <see cref="Sampler"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="Sampler"/>.</returns>
  RHISampler* CreateSampler(const RHISamplerDescription& description);

  /// <summary>
  /// Creates a new <see cref="Shader"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="Shader"/>.</returns>
  RHIShader* CreateShader(const RHIShaderDescription& description);

  /// <summary>
  /// Creates a new <see cref="CommandList"/>.
  /// </summary>
  /// <returns>A new <see cref="CommandList"/>.</returns>
  RHICommandList* CreateCommandList();

  /// <summary>
  /// Creates a new <see cref="CommandList"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="CommandList"/>.</returns>
  virtual RHICommandList* CreateCommandList(const RHICommandListDescription& description);

  /// <summary>
  /// Creates a new <see cref="ResourceLayout"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="ResourceLayout"/>.</returns>
  RHIResourceLayout* CreateResourceLayout(const RHIResourceLayoutDescription& description);
  /// <summary>
  /// Creates a new <see cref="ResourceSet"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="ResourceSet"/>.</returns>
  RHIResourceSet* CreateResourceSet(const RHIResourceSetDescription& description);

  /// <summary>
  /// Creates a new <see cref="Fence"/> in the given state.
  /// </summary>
  /// <param name="signaled">A value indicating whether the Fence should be in the signaled state when created.</param>
  /// <returns>A new <see cref="Fence"/>.</returns>
  RHIFence* CreateFence(bool signaled);

  /// <summary>
  /// Creates a new <see cref="Swapchain"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="Swapchain"/>.</returns>
  RHISwapchain* CreateSwapchain(const RHISwapchainDescription& description);

protected:
  /// <summary></summary>
  /// <param name="features"></param>
  RHIResourceFactory(const RHIGraphicsDeviceFeatures& features);

  virtual RHIPipeline* CreateGraphicsPipelineCore(const RHIGraphicsPipelineDescription& description) = 0;
  virtual RHITexture* CreateTextureCore(ezUInt64 nativeTexture, const RHITextureDescription& description) = 0;
  virtual RHITexture* CreateTextureCore(const RHITextureDescription& description) = 0;
  virtual RHITextureView* CreateTextureViewCore(const RHITextureViewDescription& description) = 0;
  virtual RHIBuffer* CreateBufferCore(const RHIBufferDescription& description) = 0;
  virtual RHISampler* CreateSamplerCore(const RHISamplerDescription& description) = 0;
  virtual RHIShader* CreateShaderCore(const RHIShaderDescription& description) = 0;
  virtual RHICommandList* CreateCommandListCore(const RHICommandListDescription& description) = 0;
  virtual RHIResourceLayout* CreateResourceLayoutCore(const RHIResourceLayoutDescription& description) = 0;
  virtual RHIResourceSet* CreateResourceSetCore(const RHIResourceSetDescription& description) = 0;
  virtual RHIFence* CreateFenceCore(bool signaled) = 0;
  virtual RHISwapchain* CreateSwapchainCore(const RHISwapchainDescription& description) = 0;
  virtual RHIPipeline* CreateComputePipelineCore(const RHIComputePipelineDescription& description) = 0;
  virtual RHIFramebuffer* CreateFramebufferCore(const RHIFramebufferDescription& description) = 0;

private:
  RHIGraphicsDeviceFeatures Features;
};
