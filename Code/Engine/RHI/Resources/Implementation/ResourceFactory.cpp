#include <RHI/Resources/ResourceFactory.h>

/// <summary>
/// Gets the <see cref="GraphicsDeviceFeatures"/> this instance was created with.
/// </summary>

/// <summary>
/// Creates a new <see cref="Pipeline"/> object.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="Pipeline"/> which, when bound to a CommandList, is used to dispatch draw commands.</returns>

RHIPipeline* RHIResourceFactory::CreateGraphicsPipeline(const RHIGraphicsPipelineDescription& description)
{

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  if (!description.RasterizerState.DepthClipEnabled && !Features.DepthClipDisableSupported())
  {
    EZ_REPORT_FAILURE("RasterizerState.DepthClipEnabled must be true if GraphicsDeviceFeatures.DepthClipDisable is not supported.");
  }
  if (description.RasterizerState.FillMode == RHIPolygonFillMode::Wireframe && !Features.FillModeWireframeSupported())
  {
    EZ_REPORT_FAILURE("PolygonFillMode.Wireframe requires GraphicsDeviceFeatures.FillModeWireframe.");
  }
  if (!Features.IndependentBlendSupported())
  {
    if (description.BlendState.AttachmentStates.GetCount() > 0)
    {
      RHIBlendAttachmentDescription attachmentState = description.BlendState.AttachmentStates[0];
      for (ezUInt32 i = 1; i < description.BlendState.AttachmentStates.GetCount(); i++)
      {
        if (!(attachmentState == description.BlendState.AttachmentStates[i]))
        {
          EZ_REPORT_FAILURE("If GraphcsDeviceFeatures.IndependentBlend is false, then all members of BlendState.AttachmentStates must be equal.");
        }
      }
    }
  }
  for (const RHIVertexLayoutDescription& layoutDesc : description.ShaderSet.VertexLayouts)
  {
    bool hasExplicitLayout = false;
    ezUInt32 minOffset = 0;
    for (const RHIVertexElementDescription& elementDesc : layoutDesc.Elements)
    {
      if (hasExplicitLayout && elementDesc.Offset == 0)
      {
        EZ_REPORT_FAILURE("If any vertex element has an explicit offset, then all elements must have an explicit offset.");
      }

      if (elementDesc.Offset != 0 && elementDesc.Offset < minOffset)
      {
        EZ_REPORT_FAILURE("Vertex element \"{}\" has an explicit offset which overlaps with the previous element.", elementDesc.Name);
      }

      minOffset = elementDesc.Offset + FormatHelpers::GetSize(elementDesc.Format);
      hasExplicitLayout |= elementDesc.Offset != 0;
    }

    if (minOffset > layoutDesc.Stride)
    {
      EZ_REPORT_FAILURE("The vertex layout's stride ({}) is less than the full size of the vertex ({})", layoutDesc.Stride, minOffset);
    }
  }
#endif
  return CreateGraphicsPipelineCore(description);
}

/// <summary>
/// Creates a new compute <see cref="Pipeline"/> object.
/// </summary>
/// <param name="description">The desirede properties of the created object.</param>
/// <returns>A new <see cref="Pipeline"/> which, when bound to a CommandList, is used to dispatch compute commands.</returns>

RHIPipeline* RHIResourceFactory::CreateComputePipeline(const RHIComputePipelineDescription& description)
{
  return CreateComputePipelineCore(description);
}

/// <summary>
/// Creates a new <see cref="Framebuffer"/>.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="Framebuffer"/>.</returns>

RHIFramebuffer* RHIResourceFactory::CreateFramebuffer(const RHIFramebufferDescription& description)
{
  return CreateFramebufferCore(description);
}

/// <summary>
/// Creates a new <see cref="Texture"/>.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="Texture"/>.</returns>

RHITexture* RHIResourceFactory::CreateTexture(const RHITextureDescription& description)
{

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  if (description.Width == 0 || description.Height == 0 || description.Depth == 0)
  {
    EZ_REPORT_FAILURE("Width, Height, and Depth must be non-zero.");
  }
  if ((description.Format == RHIPixelFormat::D24_UNorm_S8_UInt || description.Format == RHIPixelFormat::D32_Float_S8_UInt) && (description.Usage & RHITextureUsage::DepthStencil) == 0)
  {
    EZ_REPORT_FAILURE("The givel PixelFormat can only be used in a Texture with DepthStencil usage.");
  }
  if ((description.Type == RHITextureType::Texture1D || description.Type == RHITextureType::Texture3D) && description.SampleCount != RHITextureSampleCount::Count1)
  {
    EZ_REPORT_FAILURE("1D and 3D Textures must use RHITextureSampleCount.Count1.");
  }
  if (description.Type == RHITextureType::Texture1D && !Features.Texture1DSupported())
  {
    EZ_REPORT_FAILURE("1D Textures are not supported by this device.");
  }
  if ((description.Usage & RHITextureUsage::Staging) != 0 && description.Usage != RHITextureUsage::Staging)
  {
    EZ_REPORT_FAILURE("TextureUsage::Staging cannot be combined with any other flags.");
  }
  if ((description.Usage & RHITextureUsage::DepthStencil) != 0 && (description.Usage & RHITextureUsage::GenerateMipmaps) != 0)
  {
    EZ_REPORT_FAILURE("RHITextureUsage.DepthStencil and RHITextureUsage.GenerateMipmaps cannot be combined.");
  }
#endif
  return CreateTextureCore(description);
}

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

RHITexture* RHIResourceFactory::CreateTexture(ezUInt64 nativeTexture, const RHITextureDescription& description)
{
  return CreateTextureCore(nativeTexture, description);
}

/// <summary>
/// Creates a new <see cref="TextureView"/>.
/// </summary>
/// <param name="target">The target <see cref="Texture"/> used in the new view.</param>
/// <returns>A new <see cref="TextureView"/>.</returns>

RHITextureView* RHIResourceFactory::CreateTextureView(RHITexture* target)
{
  return CreateTextureView(RHITextureViewDescription(target));
}

/// <summary>
/// Creates a new <see cref="TextureView"/>.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="TextureView"/>.</returns>

RHITextureView* RHIResourceFactory::CreateTextureView(const RHITextureViewDescription& description)
{

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  if (description.MipLevels == 0 || description.ArrayLayers == 0 || (description.BaseMipLevel + description.MipLevels) > description.Target->GetMipLevels() || (description.BaseArrayLayer + description.ArrayLayers) > description.Target->GetArrayLayers())
  {
    EZ_REPORT_FAILURE("TextureView mip level and array layer range must be contained in the target Texture.");
  }
  if ((description.Target->GetUsage() & RHITextureUsage::Sampled) == 0 && (description.Target->GetUsage() & RHITextureUsage::Storage) == 0)
  {
    EZ_REPORT_FAILURE("To create a TextureView, the target texture must have either Sampled or Storage usage flags.");
  }
  if (!Features.SubsetTextureViewSupported() &&
      (description.BaseMipLevel != 0 || description.MipLevels != description.Target->GetMipLevels() || description.BaseArrayLayer != 0 || description.ArrayLayers != description.Target->GetArrayLayers()))
  {
    EZ_REPORT_FAILURE("GraphicsDevice does not support subset TextureViews.");
  }
  if (description.Format.has_value() && description.Format != description.Target->GetFormat())
  {
    if (!FormatHelpers::IsFormatViewCompatible(description.Format.value(), description.Target->GetFormat()))
    {
      ezStringBuilder errorSb;
      errorSb.AppendFormat("Cannot create a TextureView with format {} targeting a Texture with format ", description.Format.value());
      errorSb.AppendFormat("{}. A TextureView's format must have the same size and number of ", description.Target->GetFormat());
      errorSb.AppendFormat("components as the underlying Texture's format, or the same format.");

      EZ_REPORT_FAILURE(errorSb.GetData());
    }
  }
#endif

  return CreateTextureViewCore(description);
}

/// <summary>
/// Creates a new <see cref="DeviceBuffer"/>.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="DeviceBuffer"/>.</returns>

RHIBuffer* RHIResourceFactory::CreateBuffer(const RHIBufferDescription& description)
{

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  ezBitflags<RHIBufferUsage> usage = description.Usage;
  //if (usage.IsSet(RHIBufferUsage::StructuredBufferReadOnly) || usage.IsSet(RHIBufferUsage::StructuredBufferReadWrite))
  if ((usage & RHIBufferUsage::StructuredBufferReadOnly) == RHIBufferUsage::StructuredBufferReadOnly || (usage & RHIBufferUsage::StructuredBufferReadWrite) == RHIBufferUsage::StructuredBufferReadWrite)
  {
    if (!Features.StructuredBufferSupported())
    {
      EZ_REPORT_FAILURE("GraphicsDevice does not support structured buffers.");
    }

    if (description.StructureByteStride == 0)
    {
      EZ_REPORT_FAILURE("Structured Buffer objects must have a non-zero StructureByteStride.");
    }

    if ((usage & RHIBufferUsage::StructuredBufferReadWrite) != 0 && usage != RHIBufferUsage::StructuredBufferReadWrite)
    {
      EZ_REPORT_FAILURE("RHIBufferUsage::StructuredBufferReadWrite cannot be combined with any other flag.");
    }
    else if ((usage & RHIBufferUsage::VertexBuffer) != 0 || (usage & RHIBufferUsage::IndexBuffer) != 0 || (usage & RHIBufferUsage::IndirectBuffer) != 0)
    {
      EZ_REPORT_FAILURE("Read-Only Structured Buffer objects cannot specify BufferUsage::VertexBuffer, RHIBufferUsage::IndexBuffer, or RHIBufferUsage::IndirectBuffer.");
    }
  }
  else if (description.StructureByteStride != 0)
  {
    EZ_REPORT_FAILURE("Non-structured Buffers must have a StructureByteStride of zero.");
  }
  if ((usage & RHIBufferUsage::Staging) != 0 && usage != RHIBufferUsage::Staging)
  {
    EZ_REPORT_FAILURE("Buffers with Staging Usage must not specify any other Usage flags.");
  }
  if ((usage & RHIBufferUsage::UniformBuffer) != 0 && (description.Size % 16) != 0)
  {
    EZ_REPORT_FAILURE("Uniform buffer size must be a multiple of 16 bytes.");
  }
#endif
  return CreateBufferCore(description);
}

/// <summary>
/// Creates a new <see cref="Sampler"/>.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="Sampler"/>.</returns>

RHISampler* RHIResourceFactory::CreateSampler(const RHISamplerDescription& description)
{

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  if (!Features.SamplerLodBiasSupported() && description.LodBias != 0)
  {
    EZ_REPORT_FAILURE("GraphicsDevice does not support Sampler LOD bias. SamplerDescription.LodBias must be 0.");
  }
  if (!Features.SamplerAnisotropySupported() && description.Filter == RHISamplerFilter::Anisotropic)
  {
    EZ_REPORT_FAILURE("SamplerFilter.Anisotropic cannot be used unless GraphicsDeviceFeatures.SamplerAnisotropy is supported.");
  }
#endif

  return CreateSamplerCore(description);
}

/// <summary>
/// Creates a new <see cref="Shader"/>.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="Shader"/>.</returns>

RHIShader* RHIResourceFactory::CreateShader(const RHIShaderDescription& description)
{

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  if (!Features.ComputeShaderSupported() && description.Stage == RHIShaderStages::Compute)
  {
    EZ_REPORT_FAILURE("GraphicsDevice does not support Compute Shaders.");
  }
  if (!Features.GeometryShaderSupported() && description.Stage == RHIShaderStages::Geometry)
  {
    EZ_REPORT_FAILURE("GraphicsDevice does not support Compute Shaders.");
  }
  if (!Features.TessellationShadersSupported() && (description.Stage == RHIShaderStages::TessellationControl || description.Stage == RHIShaderStages::TessellationEvaluation))
  {
    EZ_REPORT_FAILURE("GraphicsDevice does not support Tessellation Shaders.");
  }
#endif
  return CreateShaderCore(description);
}

/// <summary>
/// Creates a new <see cref="CommandList"/>.
/// </summary>
/// <returns>A new <see cref="CommandList"/>.</returns>

RHICommandList* RHIResourceFactory::CreateCommandList()
{
  return CreateCommandList(RHICommandListDescription());
}

/// <summary>
/// Creates a new <see cref="CommandList"/>.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="CommandList"/>.</returns>

RHICommandList* RHIResourceFactory::CreateCommandList(const RHICommandListDescription& description)
{
  return CreateCommandListCore(description);
}

/// <summary>
/// Creates a new <see cref="ResourceLayout"/>.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="ResourceLayout"/>.</returns>

RHIResourceLayout* RHIResourceFactory::CreateResourceLayout(const RHIResourceLayoutDescription& description)
{
  return CreateResourceLayoutCore(description);
}

/// <summary>
/// Creates a new <see cref="ResourceSet"/>.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="ResourceSet"/>.</returns>

RHIResourceSet* RHIResourceFactory::CreateResourceSet(const RHIResourceSetDescription& description)
{
  return CreateResourceSetCore(description);
}

/// <summary>
/// Creates a new <see cref="Fence"/> in the given state.
/// </summary>
/// <param name="signaled">A value indicating whether the Fence should be in the signaled state when created.</param>
/// <returns>A new <see cref="Fence"/>.</returns>

RHIFence* RHIResourceFactory::CreateFence(bool signaled)
{
  return CreateFenceCore(signaled);
}

/// <summary>
/// Creates a new <see cref="Swapchain"/>.
/// </summary>
/// <param name="description">The desired properties of the created object.</param>
/// <returns>A new <see cref="Swapchain"/>.</returns>

RHISwapchain* RHIResourceFactory::CreateSwapchain(const RHISwapchainDescription& description)
{
  return CreateSwapchainCore(description);
}

/// <summary></summary>
/// <param name="features"></param>

RHIResourceFactory::RHIResourceFactory(const RHIGraphicsDeviceFeatures& features)
{
  Features = features;
}


EZ_STATICLINK_FILE(RHI, RHI_Resources_Implementation_ResourceFactory);

