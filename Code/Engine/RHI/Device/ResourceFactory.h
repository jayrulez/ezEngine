#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/GraphicsPipelineDescription.h>
#include <RHI/Descriptors/ComputePipelineDescription.h>
#include <RHI/Descriptors/VertexLayoutDescription.h>
#include <RHI/Descriptors/FramebufferDescription.h>
#include <RHI/Descriptors/TextureDescription.h>
#include <RHI/Descriptors/TextureViewDescription.h>
#include <RHI/Descriptors/DeviceBufferDescription.h>
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
#include <RHI/Resources/DeviceBuffer.h>
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
  RHIGraphicsDeviceFeatures GetFeatures() const
  {
    return Features;
  }

  /// <summary>
  /// Gets the <see cref="GraphicsBackend"/> of this instance.
  /// </summary>
  virtual ezEnum<RHIGraphicsBackend> GetBackendType() = 0;

  /// <summary>
  /// Creates a new <see cref="Pipeline"/> object.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="Pipeline"/> which, when bound to a CommandList, is used to dispatch draw commands.</returns>
  RHIPipeline* CreateGraphicsPipeline(const RHIGraphicsPipelineDescription& description)
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
        for (int i = 1; i < description.BlendState.AttachmentStates.GetCount(); i++)
        {
          if (!(attachmentState == description.BlendState.AttachmentStates[i]))
          {
            EZ_REPORT_FAILURE("If GraphcsDeviceFeatures.IndependentBlend is false, then all members of BlendState.AttachmentStates must be equal.");
          }
        }
      }
    }
    for (RHIVertexLayoutDescription layoutDesc : description.ShaderSet.VertexLayouts)
    {
      bool hasExplicitLayout = false;
      ezUInt32 minOffset = 0;
      for (RHIVertexElementDescription elementDesc : layoutDesc.Elements)
      {
        if (hasExplicitLayout && elementDesc.Offset == 0)
        {
          EZ_REPORT_FAILURE("If any vertex element has an explicit offset, then all elements must have an explicit offset.");
        }

        if (elementDesc.Offset != 0 && elementDesc.Offset < minOffset)
        {
          EZ_REPORT_FAILURE("Vertex element \"{}\" has an explicit offset which overlaps with the previous element.", elementDesc.Name);
        }

        minOffset = elementDesc.Offset + RHIFormatUtils::GetSize(elementDesc.Format);
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
  virtual RHIPipeline* CreateComputePipeline(const RHIComputePipelineDescription& description) = 0;

  /// <summary>
  /// Creates a new <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="Framebuffer"/>.</returns>
  virtual RHIFramebuffer* CreateFramebuffer(const RHIFramebufferDescription& description) = 0;

  /// <summary>
  /// Creates a new <see cref="Texture"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="Texture"/>.</returns>
  RHITexture* CreateTexture(const RHITextureDescription& description)
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
  RHITexture* CreateTexture(ezUInt64 nativeTexture, const RHITextureDescription& description)
  {
    return CreateTextureCore(nativeTexture, description);
  }

  /// <summary>
  /// Creates a new <see cref="TextureView"/>.
  /// </summary>
  /// <param name="target">The target <see cref="Texture"/> used in the new view.</param>
  /// <returns>A new <see cref="TextureView"/>.</returns>
  RHITextureView* CreateTextureView(RHITexture* target)
  {
    return CreateTextureView(RHITextureViewDescription(target));
  }

  /// <summary>
  /// Creates a new <see cref="TextureView"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="TextureView"/>.</returns>
  RHITextureView* CreateTextureView(const RHITextureViewDescription& description)
  {

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    if (description.MipLevels == 0 || description.ArrayLayers == 0 || (description.BaseMipLevel + description.MipLevels) > description.Target->GetMipLevels() || (description.BaseArrayLayer + description.ArrayLayers) > description.Target->GetArrayLayers())
    {
      EZ_REPORT_FAILURE(
        "TextureView mip level and array layer range must be contained in the target Texture.");
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
      if (!RHIFormatUtils::IsFormatViewCompatible(description.Format.value(), description.Target->GetFormat()))
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
  RHIDeviceBuffer* CreateBuffer(const RHIBufferDescription& description)
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
  RHISampler* CreateSampler(const RHISamplerDescription& description)
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
  RHIShader* CreateShader(const RHIShaderDescription& description)
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
  RHICommandList* CreateCommandList()
  {
    return CreateCommandList(RHICommandListDescription());
  }

  /// <summary>
  /// Creates a new <see cref="CommandList"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="CommandList"/>.</returns>
  virtual RHICommandList* CreateCommandList(const RHICommandListDescription& description)
  {
    return CreateCommandListCore(description);
  }

  /// <summary>
  /// Creates a new <see cref="ResourceLayout"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="ResourceLayout"/>.</returns>
  RHIResourceLayout* CreateResourceLayout(const RHIResourceLayoutDescription& description)
  {
    return CreateResourceLayoutCore(description);
  }
  /// <summary>
  /// Creates a new <see cref="ResourceSet"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="ResourceSet"/>.</returns>
  RHIResourceSet* CreateResourceSet(const RHIResourceSetDescription& description)
  {
    return CreateResourceSetCore(description);
  }

  /// <summary>
  /// Creates a new <see cref="Fence"/> in the given state.
  /// </summary>
  /// <param name="signaled">A value indicating whether the Fence should be in the signaled state when created.</param>
  /// <returns>A new <see cref="Fence"/>.</returns>
  RHIFence* CreateFence(bool signaled)
  {
    return CreateFenceCore(signaled);
  }

  /// <summary>
  /// Creates a new <see cref="Swapchain"/>.
  /// </summary>
  /// <param name="description">The desired properties of the created object.</param>
  /// <returns>A new <see cref="Swapchain"/>.</returns>
  RHISwapchain* CreateSwapchain(const RHISwapchainDescription& description)
  {
    return CreateSwapchainCore(description);
  }

protected:
  /// <summary></summary>
  /// <param name="features"></param>
  RHIResourceFactory(const RHIGraphicsDeviceFeatures& features)
  {
    Features = features;
  }

  virtual RHIPipeline* CreateGraphicsPipelineCore(const RHIGraphicsPipelineDescription& description) = 0;
  virtual RHITexture* CreateTextureCore(ezUInt64 nativeTexture, const RHITextureDescription& description) = 0;
  virtual RHITexture* CreateTextureCore(const RHITextureDescription& description) = 0;
  virtual RHITextureView* CreateTextureViewCore(const RHITextureViewDescription& description) = 0;
  virtual RHIDeviceBuffer* CreateBufferCore(const RHIBufferDescription& description) = 0;
  virtual RHISampler* CreateSamplerCore(const RHISamplerDescription& description) = 0;
  virtual RHIShader* CreateShaderCore(const RHIShaderDescription& description) = 0;
  virtual RHICommandList* CreateCommandListCore(const RHICommandListDescription& description) = 0;
  virtual RHIResourceLayout* CreateResourceLayoutCore(const RHIResourceLayoutDescription& description) = 0;
  virtual RHIResourceSet* CreateResourceSetCore(const RHIResourceSetDescription& description) = 0;
  virtual RHIFence* CreateFenceCore(bool signaled) = 0;
  virtual RHISwapchain* CreateSwapchainCore(const RHISwapchainDescription& description) = 0;

private:
  RHIGraphicsDeviceFeatures Features;
};
