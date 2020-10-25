#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <RHI/Resources/DeviceBuffer.h>
#include <RHI/Resources/DeviceResource.h>
#include <RHI/Resources/Framebuffer.h>
#include <RHI/Resources/Pipeline.h>
#include <RHI/Resources/ResourceSet.h>
#include <RHI/Utils.h>

/// <summary>
/// Describes a <see cref="RHICommandList"/>, for creation using a <see cref="RHIResourceFactory"/>.
/// </summary>
struct EZ_RHI_DLL RHICommandListDescription : public ezHashableStruct<RHICommandListDescription>
{
  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHICommandListDescription& other) const
  {
    return true;
  }
};

/// <summary>
/// A device resource which allows the recording of graphics commands, which can later be executed by a
/// <see cref="GraphicsDevice"/>.
/// Before graphics commands can be issued, the <see cref="Begin"/> method must be invoked.
/// When the <see cref="CommandList"/> is ready to be executed, <see cref="End"/> must be invoked, and then
/// <see cref="GraphicsDevice.SubmitCommands(CommandList)"/> should be used.
/// NOTE: The use of <see cref="CommandList"/> is not thread-safe. Access to the <see cref="CommandList"/> must be
/// externally synchronized.
/// There are some limitations dictating proper usage and ordering of graphics commands. For example, a
/// <see cref="Framebuffer"/>, <see cref="Pipeline"/>, vertex buffer, and index buffer must all be
/// bound before a call to <see cref="DrawIndexed(uint, uint, uint, int, uint)"/> will succeed.
/// These limitations are described in each function, where applicable.
/// <see cref="CommandList"/> instances cannot be executed multiple times per-recording. When executed by a
/// <see cref="GraphicsDevice"/>, they must be reset and commands must be issued again.
/// See <see cref="CommandListDescription"/>.
/// </summary>
class EZ_RHI_DLL RHICommandList : public RHIDeviceResource
{
public:
  RHICommandList(
    const RHICommandListDescription& description,
    RHIGraphicsDeviceFeatures features,
    ezUInt32 uniformBufferAlignment,
    ezUInt32 structuredBufferAlignment)
  {
    m_Features = features;
    m_UniformBufferAlignment = uniformBufferAlignment;
    m_StructuredBufferAlignment = structuredBufferAlignment;
  }

  /// <summary>
  /// Puts this <see cref="CommandList"/> into the initial state.
  /// This function must be called before other graphics commands can be issued.
  /// Begin must only be called if it has not been previously called, if <see cref="End"/> has been called,
  /// or if <see cref="GraphicsDevice.SubmitCommands(CommandList)"/> has been called on this instance.
  /// </summary>
  virtual void Begin() = 0;

  /// <summary>
  /// Completes this list of graphics commands, putting it into an executable state for a <see cref="GraphicsDevice"/>.
  /// This function must only be called after <see cref="Begin"/> has been called.
  /// It is an error to call this function in succession, unless <see cref="Begin"/> has been called in between invocations.
  /// </summary>
  virtual void End() = 0;

  /// <summary>
  /// Sets the active <see cref="Pipeline"/> used for rendering.
  /// When drawing, the active <see cref="Pipeline"/> must be compatible with the bound <see cref="Framebuffer"/>,
  /// <see cref="ResourceSet"/>, and <see cref="DeviceBuffer"/> objects.
  /// When a new Pipeline is set, the previously-bound ResourceSets on this CommandList become invalidated and must be
  /// re-bound.
  /// </summary>
  /// <param name="pipeline">The new <see cref="Pipeline"/> object.</param>
  void SetPipeline(RHIPipeline* pipeline)
  {
    if (pipeline->IsComputePipeline())
      m_pComputePipeline = pipeline;
    else
      m_pGraphicsPipeline = pipeline;

    SetPipelineCore(pipeline);
  }

  /// <summary>
  /// Sets the active <see cref="DeviceBuffer"/> for the given index.
  /// When drawing, the bound <see cref="DeviceBuffer"/> objects must be compatible with the bound <see cref="Pipeline"/>.
  /// The given buffer must be non-null. It is not necessary to un-bind vertex buffers for Pipelines which will not
  /// use them. All extra vertex buffers are simply ignored.
  /// </summary>
  /// <param name="index">The buffer slot.</param>
  /// <param name="buffer">The new <see cref="DeviceBuffer"/>.</param>
  void SetVertexBuffer(ezUInt32 index, RHIDeviceBuffer* buffer)
  {
    SetVertexBuffer(index, buffer, 0);
  }

  /// <summary>
  /// Sets the active <see cref="DeviceBuffer"/> for the given index.
  /// When drawing, the bound <see cref="DeviceBuffer"/> objects must be compatible with the bound <see cref="Pipeline"/>.
  /// The given buffer must be non-null. It is not necessary to un-bind vertex buffers for Pipelines which will not
  /// use them. All extra vertex buffers are simply ignored.
  /// </summary>
  /// <param name="index">The buffer slot.</param>
  /// <param name="buffer">The new <see cref="DeviceBuffer"/>.</param>
  /// <param name="offset">The offset from the start of the buffer, in bytes, from which data will start to be read.
  /// </param>
  void SetVertexBuffer(ezUInt32 index, RHIDeviceBuffer* buffer, ezUInt32 offset)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    EZ_VERIFY(buffer->GetUsage().IsSet(RHIBufferUsage::VertexBuffer), "Buffer cannot be bound as a vertex buffer because it was not created with BufferUsage.VertexBuffer.");
#endif

    SetVertexBufferCore(index, buffer, offset);
  }

  /// <summary>
  /// Sets the active <see cref="DeviceBuffer"/>.
  /// When drawing, an <see cref="DeviceBuffer"/> must be bound.
  /// </summary>
  /// <param name="buffer">The new <see cref="DeviceBuffer"/>.</param>
  /// <param name="format">The format of data in the <see cref="DeviceBuffer"/>.</param>
  void SetIndexBuffer(RHIDeviceBuffer* buffer, ezEnum<RHIIndexFormat> format)
  {
    SetIndexBuffer(buffer, format, 0);
  }

  /// <summary>
  /// Sets the active <see cref="DeviceBuffer"/>.
  /// When drawing, an <see cref="DeviceBuffer"/> must be bound.
  /// </summary>
  /// <param name="buffer">The new <see cref="DeviceBuffer"/>.</param>
  /// <param name="format">The format of data in the <see cref="DeviceBuffer"/>.</param>
  /// <param name="offset">The offset from the start of the buffer, in bytes, from which data will start to be read.
  /// </param>
  void SetIndexBuffer(RHIDeviceBuffer* buffer, ezEnum<RHIIndexFormat> format, ezUInt32 offset)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    EZ_VERIFY(buffer->GetUsage().IsSet(RHIBufferUsage::IndexBuffer), "Buffer cannot be bound as an index buffer because it was not created with BufferUsage.IndexBuffer.");
    m_pIndexBuffer = buffer;
    m_IndexFormat = format;
#endif

    SetIndexBufferCore(buffer, format, offset);
  }

  /// <summary>
  /// Sets the active <see cref="ResourceSet"/> for the given index. This ResourceSet is only active for the graphics
  /// Pipeline.
  /// </summary>
  /// <param name="slot">The resource slot.</param>
  /// <param name="rs">The new <see cref="ResourceSet"/>.</param>
  void SetGraphicsResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet)
  {
    SetGraphicsResourceSet(slot, resourceSet, 0, ezDynamicArray<ezUInt32>());
  }

  /// <summary>
  /// Sets the active <see cref="ResourceSet"/> for the given index. This ResourceSet is only active for the graphics
  /// Pipeline.
  /// </summary>
  /// <param name="slot">The resource slot.</param>
  /// <param name="rs">The new <see cref="ResourceSet"/>.</param>
  /// <param name="dynamicOffsets">An array containing the offsets to apply to the dynamic
  /// buffers contained in the <see cref="ResourceSet"/>. The number of elements in this array must be equal to the number
  /// of dynamic buffers (<see cref="ResourceLayoutElementOptions.DynamicBinding"/>) contained in the
  /// <see cref="ResourceSet"/>. These offsets are applied in the order that dynamic buffer
  /// elements appear in the <see cref="ResourceSet"/>. Each of these offsets must be a multiple of either
  /// <see cref="GraphicsDevice.UniformBufferMinOffsetAlignment"/> or
  /// <see cref="GraphicsDevice.StructuredBufferMinOffsetAlignment"/>, depending on the kind of resource.</param>
  void SetGraphicsResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet, const ezDynamicArray<ezUInt32>& dynamicOffsets)
  {
    SetGraphicsResourceSet(slot, resourceSet, dynamicOffsets.GetCount(), dynamicOffsets);
  }

  /// <summary>
  /// Sets the active <see cref="ResourceSet"/> for the given index. This ResourceSet is only active for the graphics
  /// Pipeline.
  /// </summary>
  /// <param name="slot">The resource slot.</param>
  /// <param name="rs">The new <see cref="ResourceSet"/>.</param>
  /// <param name="dynamicOffsetsCount">The number of dynamic offsets being used. This must be equal to the number of
  /// dynamic buffers (<see cref="ResourceLayoutElementOptions.DynamicBinding"/>) contained in the
  /// <see cref="ResourceSet"/>.</param>
  /// <param name="dynamicOffsets">A reference to the first of a series of offsets which will be applied to the dynamic
  /// buffers contained in the <see cref="ResourceSet"/>. These offsets are applied in the order that dynamic buffer
  /// elements appear in the <see cref="ResourceSet"/>. Each of these offsets must be a multiple of either
  /// <see cref="GraphicsDevice.UniformBufferMinOffsetAlignment"/> or
  /// <see cref="GraphicsDevice.StructuredBufferMinOffsetAlignment"/>, depending on the kind of resource.</param>
  void SetGraphicsResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    if (m_pGraphicsPipeline == nullptr)
    {
      EZ_REPORT_FAILURE("A graphics Pipeline must be active before SetGraphicsResourceSet can be called.");
    }

    ezUInt32 layoutsCount = m_pGraphicsPipeline->GetResourceLayouts().GetCount();
    if (layoutsCount <= slot)
    {
      EZ_REPORT_FAILURE("Failed to bind ResourceSet to slot {}. The active graphics Pipeline only contains {} ResourceLayouts.", slot, layoutsCount);
    }

    RHIResourceLayout* layout = m_pGraphicsPipeline->GetResourceLayouts()[slot];

    ezUInt32 pipelineLength = layout->GetDescription().Elements.GetCount();
    RHIResourceLayoutDescription layoutDesc = resourceSet->GetLayout()->GetDescription();
    ezUInt32 setLength = layoutDesc.Elements.GetCount();
    if (pipelineLength != setLength)
    {
      EZ_REPORT_FAILURE("Failed to bind ResourceSet to slot {}. The number of resources in the ResourceSet ({}) does not match the number expected by the active Pipeline ({}).", slot, setLength, pipelineLength);
    }

    for (ezUInt32 i = 0; i < pipelineLength; i++)
    {
      ezEnum<RHIResourceKind> pipelineKind = layout->GetDescription().Elements[i].Kind;
      ezEnum<RHIResourceKind> setKind = layoutDesc.Elements[i].Kind;

      if (pipelineKind != setKind)
      {
        EZ_REPORT_FAILURE("Failed to bind ResourceSet to slot {}. Resource element {} was of the incorrect type. The bound Pipeline expects {}, but the ResourceSet contained {}.", slot, i, pipelineKind, setKind);
      }
    }

    if (resourceSet->GetLayout()->GetDynamicBufferCount() != dynamicOffsetsCount)
    {
      ezStringBuilder errorSb("A dynamic offset must be provided for each resource that specifies ");
      errorSb.Append("RHIResourceLayoutElementOptions.RHIResourceLayoutElementOptions.DynamicBinding. ");
      errorSb.AppendFormat("{} offsets were expected, but only {} were provided.", resourceSet->GetLayout()->GetDynamicBufferCount(), dynamicOffsetsCount);
      EZ_REPORT_FAILURE(errorSb.GetData());
    }

    ezUInt32 dynamicOffsetIndex = 0;

    for (ezUInt32 i = 0; i < layoutDesc.Elements.GetCount(); i++)
    {
      if ((layoutDesc.Elements[i].Options & RHIResourceLayoutElementOptions::DynamicBinding) != 0)
      {
        ezUInt32 requiredAlignment = layoutDesc.Elements[i].Kind == RHIResourceKind::UniformBuffer
                                       ? m_UniformBufferAlignment
                                       : m_StructuredBufferAlignment;
        ezUInt32 desiredOffset = dynamicOffsetIndex;
        dynamicOffsetIndex += 1;
        RHIDeviceBufferRange range = RHIUtils::GetBufferRange(resourceSet->GetResources()[i], desiredOffset);

        if ((range.Offset % requiredAlignment) != 0)
        {
          ezStringBuilder errorSb;
          errorSb.AppendFormat("The effective offset of the buffer in slot {} does not meet the alignment ", i);
          errorSb.AppendFormat("equirements of this device. The offset must be a multiple of {}, but it is {}", requiredAlignment, range.Offset);

          EZ_REPORT_FAILURE(errorSb.GetData());
        }
      }
    }

#endif
    SetGraphicsResourceSet(slot, resourceSet, dynamicOffsetsCount, dynamicOffsets);
  }

  /// <summary>
  /// Sets the active <see cref="ResourceSet"/> for the given index. This ResourceSet is only active for the compute
  /// <see cref="Pipeline"/>.
  /// </summary>
  /// <param name="slot">The resource slot.</param>
  /// <param name="rs">The new <see cref="ResourceSet"/>.</param>
  void SetComputeResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet)
  {

    SetComputeResourceSet(slot, resourceSet, 0, ezDynamicArray<ezUInt32>());
  }

  /// <summary>
  /// Sets the active <see cref="ResourceSet"/> for the given index. This ResourceSet is only active for the compute
  /// <see cref="Pipeline"/>.
  /// </summary>
  /// <param name="slot">The resource slot.</param>
  /// <param name="rs">The new <see cref="ResourceSet"/>.</param>
  /// <param name="dynamicOffsets">An array containing the offsets to apply to the dynamic buffers contained in the
  /// <see cref="ResourceSet"/>. The number of elements in this array must be equal to the number of dynamic buffers
  /// (<see cref="ResourceLayoutElementOptions.DynamicBinding"/>) contained in the <see cref="ResourceSet"/>. These offsets
  /// are applied in the order that dynamic buffer elements appear in the <see cref="ResourceSet"/>.</param>
  void SetComputeResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet, const ezDynamicArray<ezUInt32>& dynamicOffsets)
  {
    SetComputeResourceSet(slot, resourceSet, dynamicOffsets.GetCount(), dynamicOffsets);
  }

  /// <summary>
  /// Sets the active <see cref="ResourceSet"/> for the given index. This ResourceSet is only active for the compute
  /// <see cref="Pipeline"/>.
  /// </summary>
  /// <param name="slot">The resource slot.</param>
  /// <param name="rs">The new <see cref="ResourceSet"/>.</param>
  /// <param name="dynamicOffsetsCount">The number of dynamic offsets being used. This must be equal to the number of
  /// dynamic buffers (<see cref="ResourceLayoutElementOptions.DynamicBinding"/>) contained in the
  /// <see cref="ResourceSet"/>.</param>
  /// <param name="dynamicOffsets">A reference to the first of a series of offsets which will be applied to the dynamic
  /// buffers contained in the <see cref="ResourceSet"/>. These offsets are applied in the order that dynamic buffer
  /// elements appear in the <see cref="ResourceSet"/>. Each of these offsets must be a multiple of either
  /// <see cref="GraphicsDevice.UniformBufferMinOffsetAlignment"/> or
  /// <see cref="GraphicsDevice.StructuredBufferMinOffsetAlignment"/>, depending on the kind of resource.</param>
  void SetComputeResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    if (m_pComputePipeline == nullptr)
    {
      EZ_REPORT_FAILURE("A graphics Pipeline must be active before SetComputeResourceSet can be called.");
    }

    ezUInt32 layoutsCount = m_pComputePipeline->GetResourceLayouts().GetCount();
    if (layoutsCount <= slot)
    {
      EZ_REPORT_FAILURE("Failed to bind ResourceSet to slot {}. The active compute Pipeline only contains {} ResourceLayouts.", slot, layoutsCount);
    }

    RHIResourceLayout* layout = m_pComputePipeline->GetResourceLayouts()[slot];

    ezUInt32 pipelineLength = layout->GetDescription().Elements.GetCount();
    ezUInt32 setLength = resourceSet->GetLayout()->GetDescription().Elements.GetCount();
    if (pipelineLength != setLength)
    {
      EZ_REPORT_FAILURE("Failed to bind ResourceSet to slot {}. The number of resources in the ResourceSet ({}) does not match the number expected by the active Pipeline ({}).", slot, setLength, pipelineLength);
    }

    for (ezUInt32 i = 0; i < pipelineLength; i++)
    {
      ezEnum<RHIResourceKind> pipelineKind = layout->GetDescription().Elements[i].Kind;
      ezEnum<RHIResourceKind> setKind = resourceSet->GetLayout()->GetDescription().Elements[i].Kind;

      if (pipelineKind != setKind)
      {
        EZ_REPORT_FAILURE("Failed to bind ResourceSet to slot {}. Resource element {} was of the incorrect type. The bound Pipeline expects {}, but the ResourceSet contained {}.", slot, i, pipelineKind, setKind);
      }
    }
#endif
    SetComputeResourceSetCore(slot, resourceSet, dynamicOffsetsCount, dynamicOffsets);
  }

  /// <summary>
  /// Sets the active <see cref="Framebuffer"/> which will be rendered to.
  /// When drawing, the active <see cref="Framebuffer"/> must be compatible with the active <see cref="Pipeline"/>.
  /// A compatible <see cref="Pipeline"/> has the same number of output attachments with matching formats.
  /// </summary>
  /// <param name="fb">The new <see cref="Framebuffer"/>.</param>
  void SetFramebuffer(RHIFramebuffer* framebuffer)
  {
    if (m_pFramebuffer != framebuffer)
    {
      m_pFramebuffer = framebuffer;
      SetFramebufferCore(framebuffer);
      SetFullViewports();
      SetFullScissorRects();
    }
  }

  /// <summary>
  /// Clears the color target at the given index of the active <see cref="Framebuffer"/>.
  /// The index given must be less than the number of color attachments in the active <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="index">The color target index.</param>
  /// <param name="clearColor">The value to clear the target to.</param>
  void ClearColorTarget(ezUInt32 index, ezColor clearColor)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    if (m_pFramebuffer == nullptr)
    {
      EZ_REPORT_FAILURE("Cannot use ClearColorTarget. There is no Framebuffer bound.");
    }

    if (m_pFramebuffer->GetColorTargets().GetCount() < index)
    {
      EZ_REPORT_FAILURE("ClearColorTarget index must be less than the current Framebuffer's color target count.");
    }
#endif
    ClearColorTargetCore(index, clearColor);
  }

  /// <summary>
  /// Clears the depth-stencil target of the active <see cref="Framebuffer"/>.
  /// The active <see cref="Framebuffer"/> must have a depth attachment.
  /// With this overload, the stencil buffer is cleared to 0.
  /// </summary>
  /// <param name="depth">The value to clear the depth buffer to.</param>
  void ClearDepthStencil(float depth)
  {
    ClearDepthStencil(depth, 0);
  }

  /// <summary>
  /// Clears the depth-stencil target of the active <see cref="Framebuffer"/>.
  /// The active <see cref="Framebuffer"/> must have a depth attachment.
  /// </summary>
  /// <param name="depth">The value to clear the depth buffer to.</param>
  /// <param name="stencil">The value to clear the stencil buffer to.</param>
  void ClearDepthStencil(float depth, ezUInt8 stencil)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    if (m_pFramebuffer == nullptr)
    {
      EZ_REPORT_FAILURE("Cannot use ClearDepthStencil. There is no Framebuffer bound.");
    }
    if (!m_pFramebuffer->GetDepthTarget().has_value())
    {
      EZ_REPORT_FAILURE("The current Framebuffer has no depth target, so ClearDepthStencil cannot be used.");
    }
#endif

    ClearDepthStencilCore(depth, stencil);
  }

  /// <summary>
  /// Sets all active viewports to cover the entire active <see cref="Framebuffer"/>.
  /// </summary>
  void SetFullViewports()
  {
    SetViewport(0, RHIViewport(0, 0, m_pFramebuffer->GetWidth(), m_pFramebuffer->GetHeight(), 0, 1));

    for (ezUInt32 index = 1; index < m_pFramebuffer->GetColorTargets().GetCount(); index++)
    {
      SetViewport(index, RHIViewport(0, 0, m_pFramebuffer->GetWidth(), m_pFramebuffer->GetHeight(), 0, 1));
    }
  }

  /// <summary>
  /// Sets the active viewport at the given index to cover the entire active <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="index">The color target index.</param>
  void SetFullViewport(ezUInt32 index)
  {
    SetViewport(index, RHIViewport(0, 0, m_pFramebuffer->GetWidth(), m_pFramebuffer->GetHeight(), 0, 1));
  }

  /// <summary>
  /// Sets the active <see cref="Viewport"/> at the given index.
  /// The index given must be less than the number of color attachments in the active <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="index">The color target index.</param>
  /// <param name="viewport">The new <see cref="Viewport"/>.</param>
  void SetViewport(ezUInt32 index, const RHIViewport& viewport)
  {
    SetViewportCore(index, viewport);
  }

  /// <summary>
  /// Sets all active scissor rectangles to cover the active <see cref="Framebuffer"/>.
  /// </summary>
  void SetFullScissorRects()
  {
    SetScissorRect(0, 0, 0, m_pFramebuffer->GetWidth(), m_pFramebuffer->GetHeight());

    for (ezUInt32 index = 1; index < m_pFramebuffer->GetColorTargets().GetCount(); index++)
    {
      SetScissorRect(index, 0, 0, m_pFramebuffer->GetWidth(), m_pFramebuffer->GetHeight());
    }
  }

  /// <summary>
  /// Sets the active scissor rectangle at the given index to cover the active <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="index">The color target index.</param>
  void SetFullScissorRect(ezUInt32 index)
  {
    SetScissorRect(index, 0, 0, m_pFramebuffer->GetWidth(), m_pFramebuffer->GetHeight());
  }

  /// <summary>
  /// Sets the active scissor rectangle at the given index.
  /// The index given must be less than the number of color attachments in the active <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="index">The color target index.</param>
  /// <param name="x">The X value of the scissor rectangle.</param>
  /// <param name="y">The Y value of the scissor rectangle.</param>
  /// <param name="width">The width of the scissor rectangle.</param>
  /// <param name="height">The height of the scissor rectangle.</param>
  virtual void SetScissorRect(ezUInt32 index, ezUInt32 x, ezUInt32 y, ezUInt32 width, ezUInt32 height)
  {
    SetScissorRectCore(index, x, y, width, height);
  }

  /// <summary>
  /// Draws primitives from the currently-bound state in this CommandList. An index Buffer is not used.
  /// </summary>
  /// <param name="vertexCount">The number of vertices.</param>
  void Draw(ezUInt32 vertexCount)
  {
    Draw(vertexCount, 1, 0, 0);
  }

  /// <summary>
  /// Draws primitives from the currently-bound state in this CommandList. An index Buffer is not used.
  /// </summary>
  /// <param name="vertexCount">The number of vertices.</param>
  /// <param name="instanceCount">The number of instances.</param>
  /// <param name="vertexStart">The first vertex to use when drawing.</param>
  /// <param name="instanceStart">The starting instance value.</param>
  void Draw(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 vertexStart, ezUInt32 instanceStart)
  {
    PreDrawValidation();
    DrawCore(vertexCount, instanceCount, vertexStart, instanceStart);
  }

  /// <summary>
  /// Draws indexed primitives from the currently-bound state in this <see cref="CommandList"/>.
  /// </summary>
  /// <param name="indexCount">The number of indices.</param>
  void DrawIndexed(ezUInt32 indexCount)
  {
    DrawIndexed(indexCount, 1, 0, 0, 0);
  };

  /// <summary>
  /// Draws indexed primitives from the currently-bound state in this <see cref="CommandList"/>.
  /// </summary>
  /// <param name="indexCount">The number of indices.</param>
  /// <param name="instanceCount">The number of instances.</param>
  /// <param name="indexStart">The number of indices to skip in the active index buffer.</param>
  /// <param name="vertexOffset">The base vertex value, which is added to each index value read from the index buffer.</param>
  /// <param name="instanceStart">The starting instance value.</param>
  void DrawIndexed(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 indexStart, ezInt32 vertexOffset, ezUInt32 instanceStart)
  {
    ValidateIndexBuffer(indexCount);
    PreDrawValidation();

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    if (!m_Features.DrawBaseVertexSupported() && vertexOffset != 0)
    {
      EZ_REPORT_FAILURE("Drawing with a non-zero base vertex is not supported on this device.");
    }

    if (!m_Features.DrawBaseInstanceSupported() && instanceStart != 0)
    {
      EZ_REPORT_FAILURE("Drawing with a non-zero base instance is not supported on this device.");
    }
#endif

    DrawIndexedCore(indexCount, instanceCount, indexStart, vertexOffset, instanceStart);
  }

  /// <summary>
  /// Issues indirect draw commands based on the information contained in the given indirect <see cref="DeviceBuffer"/>.
  /// The information stored in the indirect Buffer should conform to the structure of <see cref="IndirectDrawArguments"/>.
  /// </summary>
  /// <param name="indirectBuffer">The indirect Buffer to read from. Must have been created with the
  /// <see cref="BufferUsage.IndirectBuffer"/> flag.</param>
  /// <param name="offset">An offset, in bytes, from the start of the indirect buffer from which the draw commands will be
  /// read. This value must be a multiple of 4.</param>
  /// <param name="drawCount">The number of draw commands to read and issue from the indirect Buffer.</param>
  /// <param name="stride">The stride, in bytes, between consecutive draw commands in the indirect Buffer. This value must
  /// be a multiple of four, and must be larger than the size of <see cref="IndirectDrawArguments"/>.</param>
  void DrawIndirect(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride)
  {
    ValidateDrawIndirectSupport();
    ValidateIndirectBuffer(indirectBuffer);
    ValidateIndirectOffset(offset);
    ValidateIndirectStride(stride, sizeof(IndirectDrawArguments));
    PreDrawValidation();

    DrawIndirectCore(indirectBuffer, offset, drawCount, stride);
  }

  /// <summary>
  /// Issues indirect, indexed draw commands based on the information contained in the given indirect <see cref="DeviceBuffer"/>.
  /// The information stored in the indirect Buffer should conform to the structure of
  /// <see cref="IndirectDrawIndexedArguments"/>.
  /// </summary>
  /// <param name="indirectBuffer">The indirect Buffer to read from. Must have been created with the
  /// <see cref="BufferUsage.IndirectBuffer"/> flag.</param>
  /// <param name="offset">An offset, in bytes, from the start of the indirect buffer from which the draw commands will be
  /// read. This value must be a multiple of 4.</param>
  /// <param name="drawCount">The number of draw commands to read and issue from the indirect Buffer.</param>
  /// <param name="stride">The stride, in bytes, between consecutive draw commands in the indirect Buffer. This value must
  /// be a multiple of four, and must be larger than the size of <see cref="IndirectDrawIndexedArguments"/>.</param>
  void DrawIndexedIndirect(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride)
  {
    ValidateDrawIndirectSupport();
    ValidateIndirectBuffer(indirectBuffer);
    ValidateIndirectOffset(offset);
    ValidateIndirectStride(stride, sizeof(IndirectDrawIndexedArguments));
    PreDrawValidation();

    DrawIndexedIndirectCore(indirectBuffer, offset, drawCount, stride);
  }

  /// <summary>
  /// Dispatches a compute operation from the currently-bound compute state of this Pipeline.
  /// </summary>
  /// <param name="groupCountX">The X dimension of the compute thread groups that are dispatched.</param>
  /// <param name="groupCountY">The Y dimension of the compute thread groups that are dispatched.</param>
  /// <param name="groupCountZ">The Z dimension of the compute thread groups that are dispatched.</param>
  void Dispatch(ezUInt32 groupCountX, ezUInt32 groupCountY, ezUInt32 groupCountZ)
  {
    DispatchCore(groupCountX, groupCountY, groupCountZ);
  }

  /// <summary>
  /// Issues an indirect compute dispatch command based on the information contained in the given indirect
  /// <see cref="DeviceBuffer"/>. The information stored in the indirect Buffer should conform to the structure of
  /// <see cref="IndirectDispatchArguments"/>.
  /// </summary>
  /// <param name="indirectBuffer">The indirect Buffer to read from. Must have been created with the
  /// <see cref="BufferUsage.IndirectBuffer"/> flag.</param>
  /// <param name="offset">An offset, in bytes, from the start of the indirect buffer from which the draw commands will be
  /// read. This value must be a multiple of 4.</param>
  void DispatchIndirect(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset)
  {
    ValidateIndirectBuffer(indirectBuffer);
    ValidateIndirectOffset(offset);
    DispatchIndirectCore(indirectBuffer, offset);
  }

  /// <summary>
  /// Resolves a multisampled source <see cref="Texture"/> into a non-multisampled destination <see cref="Texture"/>.
  /// </summary>
  /// <param name="source">The source of the resolve operation. Must be a multisampled <see cref="Texture"/>
  /// (<see cref="Texture.SampleCount"/> > 1).</param>
  /// <param name="destination">The destination of the resolve operation. Must be a non-multisampled <see cref="Texture"/>
  /// (<see cref="Texture.SampleCount"/> == 1).</param>
  void ResolveTexture(RHITexture* source, RHITexture* destination)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    if (source->GetSampleCount() == RHITextureSampleCount::Count1)
    {
      EZ_REPORT_FAILURE("The source parameter of ResolveTexture must be a multisample texture.");
    }

    if (destination->GetSampleCount() != RHITextureSampleCount::Count1)
    {
      EZ_REPORT_FAILURE("The destination parameter of ResolveTexture must be a non-multisample texture. Instead, it is a texture with {FormatHelpers.GetSampleCountUInt32(source.SampleCount)} samples.");
    }
#endif

    ResolveTextureCore(source, destination);
  }

  /// <summary>
  /// Updates a <see cref="DeviceBuffer"/> region with new data.
  /// This function must be used with a blittable value type <typeparamref name="T"/>.
  /// </summary>
  /// <typeparam name="T">The type of data to upload.</typeparam>
  /// <param name="buffer">The resource to update.</param>
  /// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/> storage, at
  /// which new data will be uploaded.</param>
  /// <param name="source">The value to upload.</param>
  template <typename T>
  void UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, T source)
  {
    ezUInt8* sourcePtr = &source;
    UpdateBuffer(buffer, bufferOffset, sourcePtr, sizeof(T));
  }

  /// <summary>
  /// Updates a <see cref="DeviceBuffer"/> region with new data.
  /// This function must be used with a blittable value type <typeparamref name="T"/>.
  /// </summary>
  /// <typeparam name="T">The type of data to upload.</typeparam>
  /// <param name="buffer">The resource to update.</param>
  /// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
  /// which new data will be uploaded.</param>
  /// <param name="source">A reference to the single value to upload.</param>
  template <typename T>
  void UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, const T& source)
  {
    ezUInt8* sourcePtr = &source;
    UpdateBuffer(buffer, bufferOffset, sourcePtr, sizeof(T));
  }

  /// <summary>
  /// Updates a <see cref="DeviceBuffer"/> region with new data.
  /// This function must be used with a blittable value type <typeparamref name="T"/>.
  /// </summary>
  /// <typeparam name="T">The type of data to upload.</typeparam>
  /// <param name="buffer">The resource to update.</param>
  /// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
  /// which new data will be uploaded.</param>
  /// <param name="source">A reference to the first of a series of values to upload.</param>
  /// <param name="size">The total size of the uploaded data, in bytes.</param>
  template <typename T>
  void UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, const T& source, ezUInt32 size)
  {
    ezUInt8* sourcePtr = &source;
    UpdateBuffer(buffer, bufferOffset, sourcePtr, size);
  }

  /// <summary>
  /// Updates a <see cref="DeviceBuffer"/> region with new data.
  /// </summary>
  /// <param name="buffer">The resource to update.</param>
  /// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
  /// which new data will be uploaded.</param>
  /// <param name="source">A pointer to the start of the data to upload.</param>
  /// <param name="size">The total size of the uploaded data, in bytes.</param>
  void UpdateBuffer(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size)
  {
    if (bufferOffset + size > buffer->GetSize())
    {
      ezStringBuilder errorSb("");
      errorSb.AppendFormat("The DeviceBuffer's capacity ({}) is not large enough to store the amount of ", buffer->GetSize());
      errorSb.AppendFormat("data specified ({}) at the given offset ({}).", size, bufferOffset);
      EZ_REPORT_FAILURE(errorSb.GetData());
    }

    if (size == 0)
    {
      return;
    }

    UpdateBufferCore(buffer, bufferOffset, source, size);
  }

  /// <summary>
  /// Copies a region from the source <see cref="DeviceBuffer"/> to another region in the destination <see cref="DeviceBuffer"/>.
  /// </summary>
  /// <param name="source">The source <see cref="DeviceBuffer"/> from which data will be copied.</param>
  /// <param name="sourceOffset">An offset into <paramref name="source"/> at which the copy region begins.</param>
  /// <param name="destination">The destination <see cref="DeviceBuffer"/> into which data will be copied.</param>
  /// <param name="destinationOffset">An offset into <paramref name="destination"/> at which the data will be copied.
  /// </param>
  /// <param name="size">The number of bytes to copy.</param>
  void CopyBuffer(RHIDeviceBuffer* source, ezUInt32 sourceOffset, RHIDeviceBuffer* destination, ezUInt32 destinationOffset, ezUInt32 size)
  {
    if (size == 0)
    {
      return;
    }

    CopyBufferCore(source, sourceOffset, destination, destinationOffset, size);
  }

  /// <summary>
  /// Copies all subresources from one <see cref="Texture"/> to another.
  /// </summary>
  /// <param name="source">The source of Texture data.</param>
  /// <param name="destination">The destination of Texture data.</param>
  void CopyTexture(RHITexture* source, RHITexture* destination)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    ezUInt32 effectiveSrcArrayLayers = (source->GetUsage() & RHITextureUsage::Cubemap) != 0
                                         ? source->GetArrayLayers() * 6
                                         : source->GetArrayLayers();

    ezUInt32 effectiveDstArrayLayers = (destination->GetUsage() & RHITextureUsage::Cubemap) != 0
                                         ? destination->GetArrayLayers() * 6
                                         : destination->GetArrayLayers();

    if (effectiveSrcArrayLayers != effectiveDstArrayLayers ||
        source->GetMipLevels() != destination->GetMipLevels() ||
        source->GetSampleCount() != destination->GetSampleCount() ||
        source->GetWidth() != destination->GetWidth() ||
        source->GetHeight() != destination->GetHeight() ||
        source->GetDepth() != destination->GetDepth() ||
        source->GetFormat() != destination->GetFormat())
    {
      EZ_REPORT_FAILURE("Source and destination Textures are not compatible to be copied.");
    }
#endif

    for (ezUInt32 level = 0; level < source->GetMipLevels(); level++)
    {
      ezUInt32 mipWidth;
      ezUInt32 mipHeight;
      ezUInt32 mipDepth;
      RHIUtils::GetMipDimensions(source, level, mipWidth, mipHeight, mipDepth);

      CopyTexture(
        source, 0, 0, 0, level, 0,
        destination, 0, 0, 0, level, 0,
        mipWidth, mipHeight, mipDepth,
        effectiveSrcArrayLayers);
    }
  }

  /// <summary>
  /// Copies one subresource from one <see cref="Texture"/> to another.
  /// </summary>
  /// <param name="source">The source of Texture data.</param>
  /// <param name="destination">The destination of Texture data.</param>
  /// <param name="mipLevel">The mip level to copy.</param>
  /// <param name="arrayLayer">The array layer to copy.</param>
  void CopyTexture(RHITexture* source, RHITexture* destination, ezUInt32 mipLevel, ezUInt32 arrayLayer)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    ezUInt32 effectiveSrcArrayLayers = (source->GetUsage() & RHITextureUsage::Cubemap) != 0
                                         ? source->GetArrayLayers() * 6
                                         : source->GetArrayLayers();

    ezUInt32 effectiveDstArrayLayers = (destination->GetUsage() & RHITextureUsage::Cubemap) != 0
                                         ? destination->GetArrayLayers() * 6
                                         : destination->GetArrayLayers();

    if (effectiveSrcArrayLayers != effectiveDstArrayLayers ||
        source->GetMipLevels() != destination->GetMipLevels() ||
        source->GetSampleCount() != destination->GetSampleCount() ||
        source->GetWidth() != destination->GetWidth() ||
        source->GetHeight() != destination->GetHeight() ||
        source->GetDepth() != destination->GetDepth() ||
        source->GetFormat() != destination->GetFormat())
    {
      EZ_REPORT_FAILURE("Source and destination Textures are not compatible to be copied.");
    }
    if (mipLevel >= source->GetMipLevels() || arrayLayer >= effectiveSrcArrayLayers)
    {
      EZ_REPORT_FAILURE("mipLevel and arrayLayer must be less than the given Textures' mip level count and array layer count.");
    }
#endif

    ezUInt32 width;
    ezUInt32 height;
    ezUInt32 depth;
    RHIUtils::GetMipDimensions(source, mipLevel, width, height, depth);
    CopyTexture(
      source, 0, 0, 0, mipLevel, arrayLayer,
      destination, 0, 0, 0, mipLevel, arrayLayer,
      width, height, depth,
      1);
  }

  /// <summary>
  /// Copies a region from one <see cref="Texture"/> into another.
  /// </summary>
  /// <param name="source">The source <see cref="Texture"/> from which data is copied.</param>
  /// <param name="srcX">The X coordinate of the source copy region.</param>
  /// <param name="srcY">The Y coordinate of the source copy region.</param>
  /// <param name="srcZ">The Z coordinate of the source copy region.</param>
  /// <param name="srcMipLevel">The mip level to copy from the source Texture.</param>
  /// <param name="srcBaseArrayLayer">The starting array layer to copy from the source Texture.</param>
  /// <param name="destination">The destination <see cref="Texture"/> into which data is copied.</param>
  /// <param name="dstX">The X coordinate of the destination copy region.</param>
  /// <param name="dstY">The Y coordinate of the destination copy region.</param>
  /// <param name="dstZ">The Z coordinate of the destination copy region.</param>
  /// <param name="dstMipLevel">The mip level to copy the data into.</param>
  /// <param name="dstBaseArrayLayer">The starting array layer to copy data into.</param>
  /// <param name="width">The width in texels of the copy region.</param>
  /// <param name="height">The height in texels of the copy region.</param>
  /// <param name="depth">The depth in texels of the copy region.</param>
  /// <param name="layerCount">The number of array layers to copy.</param>
  void CopyTexture(
    RHITexture* source,
    ezUInt32 srcX, ezUInt32 srcY, ezUInt32 srcZ,
    ezUInt32 srcMipLevel,
    ezUInt32 srcBaseArrayLayer,
    RHITexture* destination,
    ezUInt32 dstX, ezUInt32 dstY, ezUInt32 dstZ,
    ezUInt32 dstMipLevel,
    ezUInt32 dstBaseArrayLayer,
    ezUInt32 width, ezUInt32 height, ezUInt32 depth,
    ezUInt32 layerCount)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    if (width == 0 || height == 0 || depth == 0)
    {
      EZ_REPORT_FAILURE("The given copy region is empty.");
    }
    if (layerCount == 0)
    {
      EZ_REPORT_FAILURE("(layerCount) must be greater than 0.");
    }
    ezUInt32 srcWidth;
    ezUInt32 srcHeight;
    ezUInt32 srcDepth;

    RHIUtils::GetMipDimensions(source, srcMipLevel, srcWidth, srcHeight, srcDepth);
    ezUInt32 srcBlockSize = RHIFormatUtils::IsCompressedFormat(source->GetFormat()) ? 4u : 1u;
    ezUInt32 roundedSrcWidth = (srcWidth + srcBlockSize - 1) / srcBlockSize * srcBlockSize;
    ezUInt32 roundedSrcHeight = (srcHeight + srcBlockSize - 1) / srcBlockSize * srcBlockSize;
    if (srcX + width > roundedSrcWidth || srcY + height > roundedSrcHeight || srcZ + depth > srcDepth)
    {
      EZ_REPORT_FAILURE("The given copy region is not valid for the source Texture.");
    }
    ezUInt32 dstWidth;
    ezUInt32 dstHeight;
    ezUInt32 dstDepth;

    RHIUtils::GetMipDimensions(destination, dstMipLevel, dstWidth, dstHeight, dstDepth);
    ezUInt32 dstBlockSize = RHIFormatUtils::IsCompressedFormat(destination->GetFormat()) ? 4u : 1u;
    ezUInt32 roundedDstWidth = (dstWidth + dstBlockSize - 1) / dstBlockSize * dstBlockSize;
    ezUInt32 roundedDstHeight = (dstHeight + dstBlockSize - 1) / dstBlockSize * dstBlockSize;
    if (dstX + width > roundedDstWidth || dstY + height > roundedDstHeight || dstZ + depth > dstDepth)
    {
      EZ_REPORT_FAILURE("The given copy region is not valid for the destination Texture.");
    }
    if (srcMipLevel >= source->GetMipLevels())
    {
      EZ_REPORT_FAILURE("{nameof(srcMipLevel)} must be less than the number of mip levels in the source Texture.");
    }
    ezUInt32 effectiveSrcArrayLayers = (source->GetUsage() & RHITextureUsage::Cubemap) != 0
                                         ? source->GetArrayLayers() * 6
                                         : source->GetArrayLayers();
    if (srcBaseArrayLayer + layerCount > effectiveSrcArrayLayers)
    {
      EZ_REPORT_FAILURE("An invalid mip range was given for the source Texture.");
    }
    if (dstMipLevel >= destination->GetMipLevels())
    {
      EZ_REPORT_FAILURE("dstMipLevel must be less than the number of mip levels in the destination Texture.");
    }
    ezUInt32 effectiveDstArrayLayers = (destination->GetUsage() & RHITextureUsage::Cubemap) != 0
                                         ? destination->GetArrayLayers() * 6
                                         : destination->GetArrayLayers();
    if (dstBaseArrayLayer + layerCount > effectiveDstArrayLayers)
    {
      EZ_REPORT_FAILURE("An invalid mip range was given for the destination Texture.");
    }
#endif
    CopyTextureCore(
      source,
      srcX, srcY, srcZ,
      srcMipLevel,
      srcBaseArrayLayer,
      destination,
      dstX, dstY, dstZ,
      dstMipLevel,
      dstBaseArrayLayer,
      width, height, depth,
      layerCount);
  }

  /// <summary>
  /// Generates mipmaps for the given <see cref="Texture"/>. The largest mipmap is used to generate all of the lower mipmap
  /// levels contained in the Texture. The previous contents of all lower mipmap levels are overwritten by this operation.
  /// The target Texture must have been created with <see cref="TextureUsage"/>.<see cref="TextureUsage.GenerateMipmaps"/>.
  /// </summary>
  /// <param name="texture">The <see cref="Texture"/> to generate mipmaps for. This Texture must have been created with
  /// <see cref="TextureUsage"/>.<see cref="TextureUsage.GenerateMipmaps"/>.</param>
  void GenerateMipmaps(RHITexture* texture)
  {
    if ((texture->GetUsage() & RHITextureUsage::GenerateMipmaps) == 0)
    {
      EZ_REPORT_FAILURE("GenerateMipmaps requires a target Texture with Usage==RHITextureUsage::GenerateMipmaps");
    }

    if (texture->GetMipLevels() > 1)
    {
      GenerateMipmapsCore(texture);
    }
  }

  /// <summary>
  /// Pushes a debug group at the current position in the <see cref="CommandList"/>. This allows subsequent commands to be
  /// categorized and filtered when viewed in external debugging tools. This method can be called multiple times in order
  /// to create nested debug groupings. Each call to PushDebugGroup must be followed by a matching call to
  /// <see cref="PopDebugGroup"/>.
  /// </summary>
  /// <param name="name">The name of the group. This is an opaque identifier used for display by graphics debuggers.</param>
  void PushDebugGroup(ezString name)
  {
    PushDebugGroupCore(name);
  }

  /// <summary>
  /// Pops the current debug group. This method must only be called after <see cref="PushDebugGroup(string)"/> has been
  /// called on this instance.
  /// </summary>
  void PopDebugGroup()
  {
    PopDebugGroupCore();
  }

  /// <summary>
  /// Inserts a debug marker into the CommandList at the current position. This is used by graphics debuggers to identify
  /// points of interest in a command stream.
  /// </summary>
  /// <param name="name">The name of the marker. This is an opaque identifier used for display by graphics debuggers.</param>
  void InsertDebugMarker(ezString name)
  {
    InsertDebugMarkerCore(name);
  }

protected:
  virtual void SetPipelineCore(RHIPipeline* pipeline) = 0;
  virtual void SetVertexBufferCore(ezUInt32 index, RHIDeviceBuffer* buffer, ezUInt32 offset) = 0;
  virtual void SetIndexBufferCore(RHIDeviceBuffer* buffer, ezEnum<RHIIndexFormat> format, ezUInt32 offset) = 0;
  virtual void SetGraphicsResourceSetCore(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets) = 0;
  virtual void SetComputeResourceSetCore(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets) = 0;
  virtual void SetFramebufferCore(RHIFramebuffer* framebuffer) = 0;
  virtual void ClearColorTargetCore(ezUInt32 index, ezColor clearColor) = 0;
  virtual void ClearDepthStencilCore(float depth, ezUInt8 stencil) = 0;
  virtual void SetViewportCore(ezUInt32 index, const RHIViewport& viewport) = 0;
  virtual void SetScissorRectCore(ezUInt32 index, ezUInt32 x, ezUInt32 y, ezUInt32 width, ezUInt32 height) = 0;
  virtual void DrawCore(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 vertexStart, ezUInt32 instanceStart) = 0;
  virtual void DrawIndexedCore(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 indexStart, ezInt32 vertexOffset, ezUInt32 instanceStart) = 0;
  virtual void DrawIndirectCore(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride) = 0;
  virtual void DrawIndexedIndirectCore(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride) = 0;
  virtual void DispatchCore(ezUInt32 groupCountX, ezUInt32 groupCountY, ezUInt32 groupCountZ) = 0;
  virtual void DispatchIndirectCore(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset) = 0;
  virtual void ResolveTextureCore(RHITexture* source, RHITexture* destination) = 0;
  virtual void UpdateBufferCore(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size) = 0;
  virtual void CopyBufferCore(RHIDeviceBuffer* source, ezUInt32 sourceOffset, RHIDeviceBuffer* destination, ezUInt32 destinationOffset, ezUInt32 size) = 0;
  virtual void CopyTextureCore(
    RHITexture* source,
    ezUInt32 srcX, ezUInt32 srcY, ezUInt32 srcZ,
    ezUInt32 srcMipLevel,
    ezUInt32 srcBaseArrayLayer,
    RHITexture* destination,
    ezUInt32 dstX, ezUInt32 dstY, ezUInt32 dstZ,
    ezUInt32 dstMipLevel,
    ezUInt32 dstBaseArrayLayer,
    ezUInt32 width, ezUInt32 height, ezUInt32 depth,
    ezUInt32 layerCount) = 0;
  virtual void GenerateMipmapsCore(RHITexture* texture) = 0;
  virtual void PushDebugGroupCore(ezString name) = 0;
  virtual void PopDebugGroupCore() = 0;
  virtual void InsertDebugMarkerCore(ezString name) = 0;

private:
  void ClearCachedState()
  {
    m_pFramebuffer = nullptr;
    m_pGraphicsPipeline = nullptr;
    m_pComputePipeline = nullptr;

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    m_pIndexBuffer = nullptr;
#endif
  }

  static void ValidateIndirectOffset(ezUInt32 offset)
  {
    if ((offset % 4) != 0)
    {
      EZ_REPORT_FAILURE("offset must be a multiple of 4.");
    }
  }
  void ValidateDrawIndirectSupport()
  {
    if (!m_Features.DrawIndirectSupported())
    {
      EZ_REPORT_FAILURE("Indirect drawing is not supported by this device.");
    }
  }

  static void ValidateIndirectBuffer(RHIDeviceBuffer* indirectBuffer)
  {
    if ((indirectBuffer->GetUsage() & RHIBufferUsage::IndirectBuffer) != RHIBufferUsage::IndirectBuffer)
    {
      EZ_REPORT_FAILURE("indirectBuffer parameter must have been created with RHIBufferUsage::IndirectBuffer. Instead, it was {}.", indirectBuffer->GetUsage());
    }
  }

  static void ValidateIndirectStride(ezUInt32 stride, int argumentSize)
  {
    if (stride < argumentSize || ((stride % 4) != 0))
    {
      EZ_REPORT_FAILURE("stride parameter must be a multiple of 4, and must be larger than the size of the corresponding argument structure.");
    }
  }

  void ValidateIndexBuffer(ezUInt32 indexCount)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    if (m_pIndexBuffer == nullptr)
    {
      EZ_REPORT_FAILURE("An index buffer must be bound before {nameof(CommandList)}.{nameof(DrawIndexed)} can be called.");
    }

    ezUInt32 indexFormatSize = m_IndexFormat == RHIIndexFormat::UInt16 ? 2u : 4u;
    ezUInt32 bytesNeeded = indexCount * indexFormatSize;
    if (m_pIndexBuffer->GetSize() < bytesNeeded)
    {
      EZ_REPORT_FAILURE("The active index buffer does not contain enough data to satisfy the given draw command. {} bytes are needed, but the buffer only contains {}.", bytesNeeded, m_pIndexBuffer->GetSize());
    }
#endif
  }

  void PreDrawValidation()
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    if (m_pGraphicsPipeline == nullptr)
    {
      EZ_REPORT_FAILURE("A graphics Pipeline must be set in order to issue draw commands.");
    }
    if (m_pFramebuffer == nullptr)
    {
      EZ_REPORT_FAILURE("A Framebuffer must be set in order to issue draw commands.");
    }
    if (!(m_pGraphicsPipeline->GetGraphicsOutputDescription() == m_pFramebuffer->GetOutputDescription()))
    {
      EZ_REPORT_FAILURE("The OutputDescription of the current graphics Pipeline is not compatible with the current Framebuffer.");
    }
#endif
  }

protected:
  RHIFramebuffer* m_pFramebuffer;
  RHIPipeline* m_pGraphicsPipeline;
  RHIPipeline* m_pComputePipeline;

private:
  RHIGraphicsDeviceFeatures m_Features;
  ezUInt32 m_UniformBufferAlignment;
  ezUInt32 m_StructuredBufferAlignment;

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  RHIDeviceBuffer* m_pIndexBuffer;
  ezEnum<RHIIndexFormat> m_IndexFormat;
#endif
};
