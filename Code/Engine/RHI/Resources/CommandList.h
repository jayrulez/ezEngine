#pragma once

#include <RHI/Descriptors/CommandListDescription.h>
#include <RHI/Descriptors/ResourceLayoutDescription.h>
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Buffer.h>
#include <RHI/Resources/BufferRange.h>
#include <RHI/Resources/Framebuffer.h>
#include <RHI/Resources/Pipeline.h>
#include <RHI/Resources/ResourceLayout.h>
#include <RHI/Resources/ResourceSet.h>
#include <RHI/Resources/Texture.h>
#include <RHI/Resources/Viewport.h>

#include <RHI/Resources/Resource.h>

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
class EZ_RHI_DLL RHICommandList : public RHIResource
{
public:
  RHICommandList(
    const RHICommandListDescription& description,
    RHIGraphicsDeviceFeatures features,
    ezUInt32 uniformBufferAlignment,
    ezUInt32 structuredBufferAlignment);

  /// <summary>
  /// Puts this <see cref="CommandList"/> into the initial state.
  /// This function must be called before other graphics commands can be issued.
  /// Begin must only be called if it has not been previously called, if <see cref="End"/> has been called,
  /// or if <see cref="GraphicsDevice.SubmitCommands(CommandList)"/> has been called on this instance.
  /// </summary>
  void Begin();

  /// <summary>
  /// Completes this list of graphics commands, putting it into an executable state for a <see cref="GraphicsDevice"/>.
  /// This function must only be called after <see cref="Begin"/> has been called.
  /// It is an error to call this function in succession, unless <see cref="Begin"/> has been called in between invocations.
  /// </summary>
  void End();

  /// <summary>
  /// Sets the active <see cref="Pipeline"/> used for rendering.
  /// When drawing, the active <see cref="Pipeline"/> must be compatible with the bound <see cref="Framebuffer"/>,
  /// <see cref="ResourceSet"/>, and <see cref="DeviceBuffer"/> objects.
  /// When a new Pipeline is set, the previously-bound ResourceSets on this CommandList become invalidated and must be
  /// re-bound.
  /// </summary>
  /// <param name="pipeline">The new <see cref="Pipeline"/> object.</param>
  void SetPipeline(RHIPipeline* pipeline);

  /// <summary>
  /// Sets the active <see cref="DeviceBuffer"/> for the given index.
  /// When drawing, the bound <see cref="DeviceBuffer"/> objects must be compatible with the bound <see cref="Pipeline"/>.
  /// The given buffer must be non-null. It is not necessary to un-bind vertex buffers for Pipelines which will not
  /// use them. All extra vertex buffers are simply ignored.
  /// </summary>
  /// <param name="index">The buffer slot.</param>
  /// <param name="buffer">The new <see cref="DeviceBuffer"/>.</param>
  void SetVertexBuffer(ezUInt32 index, RHIBuffer* buffer);

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
  void SetVertexBuffer(ezUInt32 index, RHIBuffer* buffer, ezUInt32 offset);

  /// <summary>
  /// Sets the active <see cref="DeviceBuffer"/>.
  /// When drawing, an <see cref="DeviceBuffer"/> must be bound.
  /// </summary>
  /// <param name="buffer">The new <see cref="DeviceBuffer"/>.</param>
  /// <param name="format">The format of data in the <see cref="DeviceBuffer"/>.</param>
  void SetIndexBuffer(RHIBuffer* buffer, ezEnum<RHIIndexFormat> format);

  /// <summary>
  /// Sets the active <see cref="DeviceBuffer"/>.
  /// When drawing, an <see cref="DeviceBuffer"/> must be bound.
  /// </summary>
  /// <param name="buffer">The new <see cref="DeviceBuffer"/>.</param>
  /// <param name="format">The format of data in the <see cref="DeviceBuffer"/>.</param>
  /// <param name="offset">The offset from the start of the buffer, in bytes, from which data will start to be read.
  /// </param>
  void SetIndexBuffer(RHIBuffer* buffer, ezEnum<RHIIndexFormat> format, ezUInt32 offset);

  /// <summary>
  /// Sets the active <see cref="ResourceSet"/> for the given index. This ResourceSet is only active for the graphics
  /// Pipeline.
  /// </summary>
  /// <param name="slot">The resource slot.</param>
  /// <param name="rs">The new <see cref="ResourceSet"/>.</param>
  void SetGraphicsResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet);

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
  void SetGraphicsResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet, const ezDynamicArray<ezUInt32>& dynamicOffsets);

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
  void SetGraphicsResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets);

  /// <summary>
  /// Sets the active <see cref="ResourceSet"/> for the given index. This ResourceSet is only active for the compute
  /// <see cref="Pipeline"/>.
  /// </summary>
  /// <param name="slot">The resource slot.</param>
  /// <param name="rs">The new <see cref="ResourceSet"/>.</param>
  void SetComputeResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet);

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
  void SetComputeResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet, const ezDynamicArray<ezUInt32>& dynamicOffsets);

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
  void SetComputeResourceSet(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets);

  /// <summary>
  /// Sets the active <see cref="Framebuffer"/> which will be rendered to.
  /// When drawing, the active <see cref="Framebuffer"/> must be compatible with the active <see cref="Pipeline"/>.
  /// A compatible <see cref="Pipeline"/> has the same number of output attachments with matching formats.
  /// </summary>
  /// <param name="fb">The new <see cref="Framebuffer"/>.</param>
  void SetFramebuffer(RHIFramebuffer* framebuffer);

  /// <summary>
  /// Clears the color target at the given index of the active <see cref="Framebuffer"/>.
  /// The index given must be less than the number of color attachments in the active <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="index">The color target index.</param>
  /// <param name="clearColor">The value to clear the target to.</param>
  void ClearColorTarget(ezUInt32 index, ezColor clearColor);

  /// <summary>
  /// Clears the depth-stencil target of the active <see cref="Framebuffer"/>.
  /// The active <see cref="Framebuffer"/> must have a depth attachment.
  /// With this overload, the stencil buffer is cleared to 0.
  /// </summary>
  /// <param name="depth">The value to clear the depth buffer to.</param>
  void ClearDepthStencil(float depth);

  /// <summary>
  /// Clears the depth-stencil target of the active <see cref="Framebuffer"/>.
  /// The active <see cref="Framebuffer"/> must have a depth attachment.
  /// </summary>
  /// <param name="depth">The value to clear the depth buffer to.</param>
  /// <param name="stencil">The value to clear the stencil buffer to.</param>
  void ClearDepthStencil(float depth, ezUInt8 stencil);

  /// <summary>
  /// Sets all active viewports to cover the entire active <see cref="Framebuffer"/>.
  /// </summary>
  void SetFullViewports();

  /// <summary>
  /// Sets the active viewport at the given index to cover the entire active <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="index">The color target index.</param>
  void SetFullViewport(ezUInt32 index);

  /// <summary>
  /// Sets the active <see cref="Viewport"/> at the given index.
  /// The index given must be less than the number of color attachments in the active <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="index">The color target index.</param>
  /// <param name="viewport">The new <see cref="Viewport"/>.</param>
  void SetViewport(ezUInt32 index, const RHIViewport& viewport);

  /// <summary>
  /// Sets all active scissor rectangles to cover the active <see cref="Framebuffer"/>.
  /// </summary>
  void SetFullScissorRects();

  /// <summary>
  /// Sets the active scissor rectangle at the given index to cover the active <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="index">The color target index.</param>
  void SetFullScissorRect(ezUInt32 index);

  /// <summary>
  /// Sets the active scissor rectangle at the given index.
  /// The index given must be less than the number of color attachments in the active <see cref="Framebuffer"/>.
  /// </summary>
  /// <param name="index">The color target index.</param>
  /// <param name="x">The X value of the scissor rectangle.</param>
  /// <param name="y">The Y value of the scissor rectangle.</param>
  /// <param name="width">The width of the scissor rectangle.</param>
  /// <param name="height">The height of the scissor rectangle.</param>
  void SetScissorRect(ezUInt32 index, ezUInt32 x, ezUInt32 y, ezUInt32 width, ezUInt32 height);

  /// <summary>
  /// Draws primitives from the currently-bound state in this CommandList. An index Buffer is not used.
  /// </summary>
  /// <param name="vertexCount">The number of vertices.</param>
  void Draw(ezUInt32 vertexCount);

  /// <summary>
  /// Draws primitives from the currently-bound state in this CommandList. An index Buffer is not used.
  /// </summary>
  /// <param name="vertexCount">The number of vertices.</param>
  /// <param name="instanceCount">The number of instances.</param>
  /// <param name="vertexStart">The first vertex to use when drawing.</param>
  /// <param name="instanceStart">The starting instance value.</param>
  void Draw(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 vertexStart, ezUInt32 instanceStart);

  /// <summary>
  /// Draws indexed primitives from the currently-bound state in this <see cref="CommandList"/>.
  /// </summary>
  /// <param name="indexCount">The number of indices.</param>
  void DrawIndexed(ezUInt32 indexCount);
  ;

  /// <summary>
  /// Draws indexed primitives from the currently-bound state in this <see cref="CommandList"/>.
  /// </summary>
  /// <param name="indexCount">The number of indices.</param>
  /// <param name="instanceCount">The number of instances.</param>
  /// <param name="indexStart">The number of indices to skip in the active index buffer.</param>
  /// <param name="vertexOffset">The base vertex value, which is added to each index value read from the index buffer.</param>
  /// <param name="instanceStart">The starting instance value.</param>
  void DrawIndexed(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 indexStart, ezInt32 vertexOffset, ezUInt32 instanceStart);

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
  void DrawIndirect(RHIBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride);

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
  void DrawIndexedIndirect(RHIBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride);

  /// <summary>
  /// Dispatches a compute operation from the currently-bound compute state of this Pipeline.
  /// </summary>
  /// <param name="groupCountX">The X dimension of the compute thread groups that are dispatched.</param>
  /// <param name="groupCountY">The Y dimension of the compute thread groups that are dispatched.</param>
  /// <param name="groupCountZ">The Z dimension of the compute thread groups that are dispatched.</param>
  void Dispatch(ezUInt32 groupCountX, ezUInt32 groupCountY, ezUInt32 groupCountZ);

  /// <summary>
  /// Issues an indirect compute dispatch command based on the information contained in the given indirect
  /// <see cref="DeviceBuffer"/>. The information stored in the indirect Buffer should conform to the structure of
  /// <see cref="IndirectDispatchArguments"/>.
  /// </summary>
  /// <param name="indirectBuffer">The indirect Buffer to read from. Must have been created with the
  /// <see cref="BufferUsage.IndirectBuffer"/> flag.</param>
  /// <param name="offset">An offset, in bytes, from the start of the indirect buffer from which the draw commands will be
  /// read. This value must be a multiple of 4.</param>
  void DispatchIndirect(RHIBuffer* indirectBuffer, ezUInt32 offset);

  /// <summary>
  /// Resolves a multisampled source <see cref="Texture"/> into a non-multisampled destination <see cref="Texture"/>.
  /// </summary>
  /// <param name="source">The source of the resolve operation. Must be a multisampled <see cref="Texture"/>
  /// (<see cref="Texture.SampleCount"/> > 1).</param>
  /// <param name="destination">The destination of the resolve operation. Must be a non-multisampled <see cref="Texture"/>
  /// (<see cref="Texture.SampleCount"/> == 1).</param>
  void ResolveTexture(RHITexture* source, RHITexture* destination);

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
  void UpdateBuffer(RHIBuffer* buffer, ezUInt32 bufferOffset, T source);

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
  void UpdateBuffer(RHIBuffer* buffer, ezUInt32 bufferOffset, const T& source);

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
  void UpdateBuffer(RHIBuffer* buffer, ezUInt32 bufferOffset, const T& source, ezUInt32 size);

  /// <summary>
  /// Updates a <see cref="DeviceBuffer"/> region with new data.
  /// </summary>
  /// <param name="buffer">The resource to update.</param>
  /// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
  /// which new data will be uploaded.</param>
  /// <param name="source">A pointer to the start of the data to upload.</param>
  /// <param name="size">The total size of the uploaded data, in bytes.</param>
  void UpdateBuffer(RHIBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size);

  /// <summary>
  /// Copies a region from the source <see cref="DeviceBuffer"/> to another region in the destination <see cref="DeviceBuffer"/>.
  /// </summary>
  /// <param name="source">The source <see cref="DeviceBuffer"/> from which data will be copied.</param>
  /// <param name="sourceOffset">An offset into <paramref name="source"/> at which the copy region begins.</param>
  /// <param name="destination">The destination <see cref="DeviceBuffer"/> into which data will be copied.</param>
  /// <param name="destinationOffset">An offset into <paramref name="destination"/> at which the data will be copied.
  /// </param>
  /// <param name="size">The number of bytes to copy.</param>
  void CopyBuffer(RHIBuffer* source, ezUInt32 sourceOffset, RHIBuffer* destination, ezUInt32 destinationOffset, ezUInt32 size);

  /// <summary>
  /// Copies all subresources from one <see cref="Texture"/> to another.
  /// </summary>
  /// <param name="source">The source of Texture data.</param>
  /// <param name="destination">The destination of Texture data.</param>
  void CopyTexture(RHITexture* source, RHITexture* destination);

  /// <summary>
  /// Copies one subresource from one <see cref="Texture"/> to another.
  /// </summary>
  /// <param name="source">The source of Texture data.</param>
  /// <param name="destination">The destination of Texture data.</param>
  /// <param name="mipLevel">The mip level to copy.</param>
  /// <param name="arrayLayer">The array layer to copy.</param>
  void CopyTexture(RHITexture* source, RHITexture* destination, ezUInt32 mipLevel, ezUInt32 arrayLayer);

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
    ezUInt32 layerCount);

  /// <summary>
  /// Generates mipmaps for the given <see cref="Texture"/>. The largest mipmap is used to generate all of the lower mipmap
  /// levels contained in the Texture. The previous contents of all lower mipmap levels are overwritten by this operation.
  /// The target Texture must have been created with <see cref="TextureUsage"/>.<see cref="TextureUsage.GenerateMipmaps"/>.
  /// </summary>
  /// <param name="texture">The <see cref="Texture"/> to generate mipmaps for. This Texture must have been created with
  /// <see cref="TextureUsage"/>.<see cref="TextureUsage.GenerateMipmaps"/>.</param>
  void GenerateMipmaps(RHITexture* texture);

  /// <summary>
  /// Pushes a debug group at the current position in the <see cref="CommandList"/>. This allows subsequent commands to be
  /// categorized and filtered when viewed in external debugging tools. This method can be called multiple times in order
  /// to create nested debug groupings. Each call to PushDebugGroup must be followed by a matching call to
  /// <see cref="PopDebugGroup"/>.
  /// </summary>
  /// <param name="name">The name of the group. This is an opaque identifier used for display by graphics debuggers.</param>
  void PushDebugGroup(ezString name);

  /// <summary>
  /// Pops the current debug group. This method must only be called after <see cref="PushDebugGroup(string)"/> has been
  /// called on this instance.
  /// </summary>
  void PopDebugGroup();

  /// <summary>
  /// Inserts a debug marker into the CommandList at the current position. This is used by graphics debuggers to identify
  /// points of interest in a command stream.
  /// </summary>
  /// <param name="name">The name of the marker. This is an opaque identifier used for display by graphics debuggers.</param>
  void InsertDebugMarker(ezString name);

protected:
  virtual void BeginCore() = 0;
  virtual void EndCore() = 0;
  virtual void SetPipelineCore(RHIPipeline* pipeline) = 0;
  virtual void SetVertexBufferCore(ezUInt32 index, RHIBuffer* buffer, ezUInt32 offset) = 0;
  virtual void SetIndexBufferCore(RHIBuffer* buffer, ezEnum<RHIIndexFormat> format, ezUInt32 offset) = 0;
  virtual void SetGraphicsResourceSetCore(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets) = 0;
  virtual void SetComputeResourceSetCore(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets) = 0;
  virtual void SetFramebufferCore(RHIFramebuffer* framebuffer) = 0;
  virtual void ClearColorTargetCore(ezUInt32 index, ezColor clearColor) = 0;
  virtual void ClearDepthStencilCore(float depth, ezUInt8 stencil) = 0;
  virtual void SetViewportCore(ezUInt32 index, const RHIViewport& viewport) = 0;
  virtual void SetScissorRectCore(ezUInt32 index, ezUInt32 x, ezUInt32 y, ezUInt32 width, ezUInt32 height) = 0;
  virtual void DrawCore(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 vertexStart, ezUInt32 instanceStart) = 0;
  virtual void DrawIndexedCore(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 indexStart, ezInt32 vertexOffset, ezUInt32 instanceStart) = 0;
  virtual void DrawIndirectCore(RHIBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride) = 0;
  virtual void DrawIndexedIndirectCore(RHIBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride) = 0;
  virtual void DispatchCore(ezUInt32 groupCountX, ezUInt32 groupCountY, ezUInt32 groupCountZ) = 0;
  virtual void DispatchIndirectCore(RHIBuffer* indirectBuffer, ezUInt32 offset) = 0;
  virtual void ResolveTextureCore(RHITexture* source, RHITexture* destination) = 0;
  virtual void UpdateBufferCore(RHIBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size) = 0;
  virtual void CopyBufferCore(RHIBuffer* source, ezUInt32 sourceOffset, RHIBuffer* destination, ezUInt32 destinationOffset, ezUInt32 size) = 0;
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
  static void ValidateIndirectOffset(ezUInt32 offset);
  void ValidateDrawIndirectSupport();

  static void ValidateIndirectBuffer(RHIBuffer* indirectBuffer);

  static void ValidateIndirectStride(ezUInt32 stride, ezUInt32 argumentSize);

  void ValidateIndexBuffer(ezUInt32 indexCount);

  void PreDrawValidation();

protected:
  void ClearCachedState();

protected:
  RHIFramebuffer* m_pFramebuffer = nullptr;
  RHIPipeline* m_pGraphicsPipeline = nullptr;
  RHIPipeline* m_pComputePipeline = nullptr;

private:
  RHIGraphicsDeviceFeatures m_Features;
  ezUInt32 m_UniformBufferAlignment;
  ezUInt32 m_StructuredBufferAlignment;

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  RHIBuffer* m_pIndexBuffer = nullptr;
  ezEnum<RHIIndexFormat> m_IndexFormat;
#endif
};

/// <summary>
/// Updates a <see cref="DeviceBuffer"/> region with new data.
/// This function must be used with a blittable value type <typeparamref name="T"/>.
/// </summary>
/// <typeparam name="T">The type of data to upload.</typeparam>
/// <param name="buffer">The resource to update.</param>
/// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/> storage, at
/// which new data will be uploaded.</param>
/// <param name="source">The value to upload.</param>

/// <summary>
/// Updates a <see cref="DeviceBuffer"/> region with new data.
/// This function must be used with a blittable value type <typeparamref name="T"/>.
/// </summary>
/// <typeparam name="T">The type of data to upload.</typeparam>
/// <param name="buffer">The resource to update.</param>
/// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
/// which new data will be uploaded.</param>
/// <param name="source">A reference to the single value to upload.</param>

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


/// <summary>
/// Updates a <see cref="DeviceBuffer"/> region with new data.
/// This function must be used with a blittable value type <typeparamref name="T"/>.
/// </summary>
/// <typeparam name="T">The type of data to upload.</typeparam>
/// <param name="buffer">The resource to update.</param>
/// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/>'s storage, at
/// which new data will be uploaded.</param>
/// <param name="source">A reference to the single value to upload.</param>


/// <summary>
/// Updates a <see cref="DeviceBuffer"/> region with new data.
/// This function must be used with a blittable value type <typeparamref name="T"/>.
/// </summary>
/// <typeparam name="T">The type of data to upload.</typeparam>
/// <param name="buffer">The resource to update.</param>
/// <param name="bufferOffset">An offset, in bytes, from the beginning of the <see cref="DeviceBuffer"/> storage, at
/// which new data will be uploaded.</param>
/// <param name="source">The value to upload.</param>


