#include <RHI/Backends/D3D11/D3D11CommandList.h>
#include <RHI/Backends/D3D11/D3D11Framebuffer.h>
#include <RHI/Backends/D3D11/D3D11GraphicsDevice.h>
#include <RHI/Backends/D3D11/D3D11Pipeline.h>
#include <RHI/Backends/D3D11/D3D11ResourceSet.h>
#include <RHI/Backends/D3D11/D3D11Sampler.h>
#include <RHI/Backends/D3D11/D3D11Swapchain.h>
#include <RHI/Backends/D3D11/D3D11Texture.h>
#include <RHI/Backends/D3D11/D3D11TextureView.h>

#include <RHI/Util.h>

#include <d3d11_1.h>
#include <vector>


bool D3D11CommandList::D3D11BufferRange::IsFullRange()
{
  return Offset == 0 && Size == Buffer->GetSize();
}

D3D11CommandList::D3D11BufferRange::D3D11BufferRange(D3D11DeviceBuffer* buffer, ezUInt32 offset, ezUInt32 size)
{
  Buffer = buffer;
  Offset = offset;
  Size = size;
}

D3D11CommandList::BoundTextureInfo::BoundTextureInfo(ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezUInt32 resourceSet)
{
  Slot = slot;
  Stages = stages;
  ResourceSet = resourceSet;
}

ezString D3D11CommandList::GetName() const
{
  return Name;
}

void D3D11CommandList::SetName(const ezString& name)
{
  Name = name;

  if (Context != nullptr)
    Context->SetPrivateData(WKPDID_D3DDebugObjectName, name.GetCharacterCount(), name.GetData());
}

bool D3D11CommandList::IsDisposed() const
{
  return Disposed;
}

void D3D11CommandList::Dispose()
{
  if (!Disposed)
  {
    if (UDA != nullptr)
    {
      UDA->Release();
      UDA = nullptr;
    }

    if (CommandList != nullptr)
    {
      CommandList->Release();
      CommandList = nullptr;
    }

    if (Context1 != nullptr)
    {
      Context1->Release();
      Context1 = nullptr;
    }

    if (Context != nullptr)
    {
      Context->Release();
      Context = nullptr;
    }

    for (RHIBoundResourceSetInfo& boundGraphicsSet : GraphicsResourceSets)
    {
      boundGraphicsSet.Offsets.Clear();
    }
    for (RHIBoundResourceSetInfo& boundComputeSet : ComputeResourceSets)
    {
      boundComputeSet.Offsets.Clear();
    }

    for (D3D11DeviceBuffer* buffer : AvailableStagingBuffers)
    {
      buffer->Dispose();
    }
    AvailableStagingBuffers.Clear();

    Disposed = true;
  }
}

void D3D11CommandList::BeginCore()
{
  if (CommandList != nullptr)
  {
    CommandList->Release();
    CommandList = nullptr;
    ClearState();
    Begun = true;
  }
}

void D3D11CommandList::EndCore()
{
  if (CommandList != nullptr)
  {
    EZ_REPORT_FAILURE("Invalid use of EndCore().");
  }

  Context->FinishCommandList(false, &CommandList);

  CommandList->SetPrivateData(WKPDID_D3DDebugObjectName, Name.GetCharacterCount(), Name.GetData());

  ResetManagedState();
  Begun = false;
}

void D3D11CommandList::SetPipelineCore(RHIPipeline* pipeline)
{
  if (!pipeline->IsComputePipeline() && GraphicsPipeline != pipeline)
  {
    D3D11Pipeline* d3dPipeline = Util::AssertSubtype<RHIPipeline, D3D11Pipeline>(pipeline);

    GraphicsPipeline = d3dPipeline;
    ClearSets(GraphicsResourceSets); // Invalidate resource set bindings -- they may be invalid.
    Util::ClearArray(InvalidatedGraphicsResourceSets);

    ID3D11BlendState* blendState = d3dPipeline->GetBlendState();
    if (BlendState != blendState)
    {
      BlendState = blendState;
      Context->OMSetBlendState(blendState, nullptr, -1);
    }

    ID3D11DepthStencilState* depthStencilState = d3dPipeline->GetDepthStencilState();
    ezUInt32 stencilReference = d3dPipeline->GetStencilReference();
    if (DepthStencilState != depthStencilState || StencilReference != stencilReference)
    {
      DepthStencilState = depthStencilState;
      StencilReference = stencilReference;
      Context->OMSetDepthStencilState(depthStencilState, (int)stencilReference);
    }

    ID3D11RasterizerState* rasterizerState = d3dPipeline->GetRasterizerState();
    if (RasterizerState != rasterizerState)
    {
      RasterizerState = rasterizerState;
      Context->RSSetState(rasterizerState);
    }

    D3D_PRIMITIVE_TOPOLOGY primitiveTopology = d3dPipeline->GetPrimitiveTopology();
    if (PrimitiveTopology != primitiveTopology)
    {
      PrimitiveTopology = primitiveTopology;
      Context->IASetPrimitiveTopology(primitiveTopology);
    }

    ID3D11InputLayout* inputLayout = d3dPipeline->GetInputLayout();
    if (InputLayout != inputLayout)
    {
      InputLayout = inputLayout;
      Context->IASetInputLayout(inputLayout);
    }

    ID3D11VertexShader* vertexShader = d3dPipeline->GetVertexShader();
    if (VertexShader != vertexShader)
    {
      VertexShader = vertexShader;
      Context->VSSetShader(vertexShader, nullptr, 0);
    }

    ID3D11GeometryShader* geometryShader = d3dPipeline->GetGeometryShader();
    if (GeometryShader != geometryShader)
    {
      GeometryShader = geometryShader;
      Context->GSSetShader(geometryShader, nullptr, 0);
    }

    ID3D11HullShader* hullShader = d3dPipeline->GetHullShader();
    if (HullShader != hullShader)
    {
      HullShader = hullShader;
      Context->HSSetShader(hullShader, nullptr, 0);
    }

    ID3D11DomainShader* domainShader = d3dPipeline->GetDomainShader();
    if (DomainShader != domainShader)
    {
      DomainShader = domainShader;
      Context->DSSetShader(domainShader, nullptr, 0);
    }

    ID3D11PixelShader* pixelShader = d3dPipeline->GetPixelShader();
    if (PixelShader != pixelShader)
    {
      PixelShader = pixelShader;
      Context->PSSetShader(pixelShader, nullptr, 0);
    }

    VertexStrides = d3dPipeline->GetVertexStrides();
    if (VertexStrides.GetCount() > 0)
    {
      ezUInt32 vertexStridesCount = VertexStrides.GetCount();
      VertexBindings.EnsureCount(vertexStridesCount);
      VertexOffsets.EnsureCount(vertexStridesCount);
    }

    GraphicsResourceSets.EnsureCount(d3dPipeline->GetResourceLayouts().GetCount());
    InvalidatedGraphicsResourceSets.EnsureCount(d3dPipeline->GetResourceLayouts().GetCount());
  }
  else if (pipeline->IsComputePipeline() && ComputePipeline != pipeline)
  {
    D3D11Pipeline* d3dPipeline = Util::AssertSubtype<RHIPipeline, D3D11Pipeline>(pipeline);
    ComputePipeline = d3dPipeline;
    ClearSets(ComputeResourceSets); // Invalidate resource set bindings -- they may be invalid.
    Util::ClearArray(InvalidatedComputeResourceSets);

    ID3D11ComputeShader* computeShader = d3dPipeline->GetComputeShader();
    Context->CSSetShader(computeShader, nullptr, 0);
    ComputeResourceSets.EnsureCount(d3dPipeline->GetResourceLayouts().GetCount());
    InvalidatedComputeResourceSets.EnsureCount(d3dPipeline->GetResourceLayouts().GetCount());
  }
}

void D3D11CommandList::SetVertexBufferCore(ezUInt32 index, RHIBuffer* buffer, ezUInt32 offset)
{
  D3D11DeviceBuffer* d3d11Buffer = Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(buffer);
  if (VertexBindings[index] != d3d11Buffer->GetBuffer() || VertexOffsets[index] != offset)
  {
    VertexBindingsChanged = true;
    UnbindUAVBuffer(buffer);
    VertexBindings[index] = d3d11Buffer->GetBuffer();
    VertexOffsets[index] = (int)offset;
    NumVertexBindings = ezMath::Max((index + 1), NumVertexBindings);
  }
}

void D3D11CommandList::SetIndexBufferCore(RHIBuffer* buffer, ezEnum<RHIIndexFormat> format, ezUInt32 offset)
{
  if (_ib != buffer || _ibOffset != offset)
  {
    _ib = buffer;
    _ibOffset = offset;
    D3D11DeviceBuffer* d3d11Buffer = Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(buffer);

    UnbindUAVBuffer(buffer);
    Context->IASetIndexBuffer(d3d11Buffer->GetBuffer(), D3D11FormatUtils::ToDxgiFormat(format), offset);
  }
}

void D3D11CommandList::SetGraphicsResourceSetCore(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets)
{
  if (GraphicsResourceSets[slot].Equals(resourceSet, dynamicOffsetsCount, dynamicOffsets))
  {
    return;
  }

  GraphicsResourceSets[slot].Offsets.Clear();
  GraphicsResourceSets[slot] = RHIBoundResourceSetInfo(resourceSet /* TODO:, dynamicOffsetsCount*/, dynamicOffsets);
  ActivateResourceSet(slot, GraphicsResourceSets[slot], true);
}

void D3D11CommandList::SetComputeResourceSetCore(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets)
{
  if (ComputeResourceSets[slot].Equals(resourceSet, dynamicOffsetsCount, dynamicOffsets))
  {
    return;
  }

  ComputeResourceSets[slot].Offsets.Clear();
  ComputeResourceSets[slot] = RHIBoundResourceSetInfo(resourceSet, /*TODO dynamicOffsetsCount,*/ dynamicOffsets);
  ActivateResourceSet(slot, ComputeResourceSets[slot], false);
}

void D3D11CommandList::SetFramebufferCore(RHIFramebuffer* framebuffer)
{
  D3D11Framebuffer* d3dFB = Util::AssertSubtype<RHIFramebuffer, D3D11Framebuffer>(framebuffer);
  if (d3dFB->GetSwapchain() != nullptr)
  {
    d3dFB->GetSwapchain()->AddCommandListReference(this);
    ReferencedSwapchains.PushBack(d3dFB->GetSwapchain());
  }

  for (ezUInt32 i = 0; i < framebuffer->GetColorTargets().GetCount(); i++)
  {
    UnbindSRVTexture(framebuffer->GetColorTargets()[i].GetTarget());
  }

  Context->OMSetRenderTargets(d3dFB->GetRenderTargetViews().GetCount(), d3dFB->GetRenderTargetViews().GetData(), d3dFB->GetDepthStencilView());
}

void D3D11CommandList::ClearColorTargetCore(ezUInt32 index, ezColor clearColor)
{
  float color[4] = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
  Context->ClearRenderTargetView(GetD3D11Framebuffer()->GetRenderTargetViews()[index], color);
}

void D3D11CommandList::ClearDepthStencilCore(float depth, ezUInt8 stencil)
{
  Context->ClearDepthStencilView(GetD3D11Framebuffer()->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
}

void D3D11CommandList::SetViewportCore(ezUInt32 index, const RHIViewport& viewport)
{
  ViewportsChanged = true;
  Viewports.EnsureCount(index + 1);

  Viewports[index] = viewport;
}

void D3D11CommandList::SetScissorRectCore(ezUInt32 index, ezUInt32 x, ezUInt32 y, ezUInt32 width, ezUInt32 height)
{
  ScissorRectsChanged = true;
  Scissors.EnsureCount(index + 1);

  Scissors[index] = {x, y, (x + width), (y + height)};
}

void D3D11CommandList::DrawCore(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 vertexStart, ezUInt32 instanceStart)
{
  PreDrawCommand();

  if (instanceCount == 1 && instanceStart == 0)
  {
    Context->Draw((int)vertexCount, (int)vertexStart);
  }
  else
  {
    Context->DrawInstanced((int)vertexCount, (int)instanceCount, (int)vertexStart, (int)instanceStart);
  }
}

void D3D11CommandList::DrawIndexedCore(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 indexStart, ezInt32 vertexOffset, ezUInt32 instanceStart)
{
  PreDrawCommand();

  EZ_ASSERT_DEBUG(_ib != nullptr, "Index buffer is null");
  if (instanceCount == 1 && instanceStart == 0)
  {
    Context->DrawIndexed((int)indexCount, (int)indexStart, vertexOffset);
  }
  else
  {
    Context->DrawIndexedInstanced(indexCount, instanceCount, indexStart, vertexOffset, instanceStart);
  }
}

void D3D11CommandList::DrawIndirectCore(RHIBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride)
{
  PreDrawCommand();

  D3D11DeviceBuffer* d3d11Buffer = Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(indirectBuffer);
  ezUInt32 currentOffset = offset;
  for (ezUInt32 i = 0; i < drawCount; i++)
  {
    Context->DrawInstancedIndirect(d3d11Buffer->GetBuffer(), currentOffset);
    currentOffset += (int)stride;
  }
}

void D3D11CommandList::DrawIndexedIndirectCore(RHIBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride)
{
  PreDrawCommand();

  D3D11DeviceBuffer* d3d11Buffer = Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(indirectBuffer);
  ezUInt32 currentOffset = offset;
  for (ezUInt32 i = 0; i < drawCount; i++)
  {
    Context->DrawIndexedInstancedIndirect(d3d11Buffer->GetBuffer(), currentOffset);
    currentOffset += (int)stride;
  }
}

void D3D11CommandList::DispatchCore(ezUInt32 groupCountX, ezUInt32 groupCountY, ezUInt32 groupCountZ)
{
  PreDispatchCommand();

  Context->Dispatch((int)groupCountX, (int)groupCountY, (int)groupCountZ);
}

void D3D11CommandList::DispatchIndirectCore(RHIBuffer* indirectBuffer, ezUInt32 offset)
{
  PreDispatchCommand();
  D3D11DeviceBuffer* d3d11Buffer = Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(indirectBuffer);
  Context->DispatchIndirect(d3d11Buffer->GetBuffer(), offset);
}

void D3D11CommandList::ResolveTextureCore(RHITexture* source, RHITexture* destination)
{
  D3D11Texture* d3d11Source = Util::AssertSubtype<RHITexture, D3D11Texture>(source);
  D3D11Texture* d3d11Destination = Util::AssertSubtype<RHITexture, D3D11Texture>(destination);
  Context->ResolveSubresource(
    d3d11Destination->GetDeviceTexture(),
    0,
    d3d11Source->GetDeviceTexture(),
    0,
    d3d11Destination->GetDxgiFormat());
}

void D3D11CommandList::UpdateBufferCore(RHIBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size)
{
  D3D11DeviceBuffer* d3dBuffer = Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(buffer);
  if (size == 0)
  {
    return;
  }

  bool isDynamic = (buffer->GetUsage() & RHIBufferUsage::Dynamic) == RHIBufferUsage::Dynamic;
  bool isStaging = (buffer->GetUsage() & RHIBufferUsage::Staging) == RHIBufferUsage::Staging;
  bool isUniformBuffer = (buffer->GetUsage() & RHIBufferUsage::UniformBuffer) == RHIBufferUsage::UniformBuffer;
  bool useMap = isDynamic;
  bool updateFullBuffer = bufferOffset == 0 && size == buffer->GetSize();
  bool useUpdateSubresource = !isDynamic && !isStaging && (!isUniformBuffer || updateFullBuffer);

  if (useUpdateSubresource)
  {
    D3D11_BOX* subregion = new D3D11_BOX;
    subregion->left = bufferOffset;
    subregion->right = size + bufferOffset;
    subregion->bottom = 1;
    subregion->back = 1;
    subregion->front = 0;
    subregion->top = 0;

    RawRect workAroundRegion;
    workAroundRegion.left = bufferOffset;
    workAroundRegion.right = size + bufferOffset;
    workAroundRegion.bottom = 1;
    workAroundRegion.back = 1;
    workAroundRegion.front = 0;
    workAroundRegion.top = 0;

    if (isUniformBuffer)
    {
      subregion = nullptr;
    }

    if (bufferOffset == 0)
    {
      Context->UpdateSubresource(d3dBuffer->GetBuffer(), 0, subregion, source, 0, 0);
    }
    else
    {
      UpdateSubresource_Workaround(d3dBuffer->GetBuffer(), 0, workAroundRegion, source);
    }
  }
  else if (useMap && updateFullBuffer) // Can only update full buffer with WriteDiscard.
  {
    D3D11_MAPPED_SUBRESOURCE msb;
    HRESULT hr = Context->Map(
      d3dBuffer->GetBuffer(),
      0,
      D3D11FormatUtils::RHIToD3D11MapMode(isDynamic, RHIMapMode::Write),
      0,
      &msb);
    if (size < 1024)
    {
      std::memcpy(msb.pData, source, size);
    }
    else
    {
      //Buffer.MemoryCopy(source, msb.pData, buffer->GetSize(), size);
      //ezMemoryUtils::CopyOverlapped(msb.pData, reinterpret_cast<void*>(source), size);
    }
    Context->Unmap(d3dBuffer->GetBuffer(), 0);
  }
  else
  {
    D3D11DeviceBuffer* staging = GetFreeStagingBuffer(size);
    GraphicsDevice->UpdateBuffer(staging, 0, source, size);
    CopyBuffer(staging, 0, buffer, bufferOffset, size);
    SubmittedStagingBuffers.PushBack(staging);
  }
}

void D3D11CommandList::CopyBufferCore(RHIBuffer* source, ezUInt32 sourceOffset, RHIBuffer* destination, ezUInt32 destinationOffset, ezUInt32 size)
{
  D3D11DeviceBuffer* srcD3D11Buffer = Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(source);
  D3D11DeviceBuffer* dstD3D11Buffer = Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(destination);

  D3D11_BOX region = {sourceOffset, 0, 0, (sourceOffset + size), 1, 1};

  Context->CopySubresourceRegion(dstD3D11Buffer->GetBuffer(), 0, destinationOffset, 0, 0, srcD3D11Buffer->GetBuffer(), 0, &region);
}

void D3D11CommandList::CopyTextureCore(RHITexture* source, ezUInt32 srcX, ezUInt32 srcY, ezUInt32 srcZ, ezUInt32 srcMipLevel, ezUInt32 srcBaseArrayLayer, RHITexture* destination, ezUInt32 dstX, ezUInt32 dstY, ezUInt32 dstZ, ezUInt32 dstMipLevel, ezUInt32 dstBaseArrayLayer, ezUInt32 width, ezUInt32 height, ezUInt32 depth, ezUInt32 layerCount)
{
  D3D11Texture* srcD3D11Texture = Util::AssertSubtype<RHITexture, D3D11Texture>(source);
  D3D11Texture* dstD3D11Texture = Util::AssertSubtype<RHITexture, D3D11Texture>(destination);

  ezUInt32 blockSize = FormatHelpers::IsCompressedFormat(source->GetFormat()) ? 4u : 1u;
  ezUInt32 clampedWidth = ezMath::Max(blockSize, width);
  ezUInt32 clampedHeight = ezMath::Max(blockSize, height);

  D3D11_BOX* region = nullptr;
  if (srcX != 0 || srcY != 0 || srcZ != 0 || clampedWidth != source->GetWidth() || clampedHeight != source->GetHeight() || depth != source->GetDepth())
  {
    D3D11_BOX box{
      srcX,
      srcY,
      srcZ,
      (srcX + clampedWidth),
      (srcY + clampedHeight),
      (srcZ + depth)};
    region = &box;
  }

  for (ezUInt32 i = 0; i < layerCount; i++)
  {
    ezUInt32 srcSubresource = D3D11Util::ComputeSubresource(srcMipLevel, source->GetMipLevels(), srcBaseArrayLayer + i);
    ezUInt32 dstSubresource = D3D11Util::ComputeSubresource(dstMipLevel, destination->GetMipLevels(), dstBaseArrayLayer + i);

    Context->CopySubresourceRegion(
      dstD3D11Texture->GetDeviceTexture(),
      dstSubresource,
      dstX,
      dstY,
      dstZ,
      srcD3D11Texture->GetDeviceTexture(),
      srcSubresource,
      region);
  }
}

void D3D11CommandList::GenerateMipmapsCore(RHITexture* texture)
{
  RHITextureView* fullTexView = texture->GetFullTextureView(GraphicsDevice);
  D3D11TextureView* d3d11View = Util::AssertSubtype<RHITextureView, D3D11TextureView>(fullTexView);
  ID3D11ShaderResourceView* srv = d3d11View->GetShaderResourceView();
  Context->GenerateMips(srv);
}

void D3D11CommandList::PushDebugGroupCore(ezString name)
{
  ezStringWChar wName(name);
  if (UDA != nullptr)
    UDA->BeginEvent(wName);
}

void D3D11CommandList::PopDebugGroupCore()
{
  if (UDA != nullptr)
    UDA->EndEvent();
}

void D3D11CommandList::InsertDebugMarkerCore(ezString name)
{
  ezStringWChar wName(name);
  if (UDA != nullptr)
    UDA->SetMarker(wName);
}

D3D11CommandList::D3D11CommandList(D3D11GraphicsDevice* graphicsDevice, const RHICommandListDescription& description)
  : RHICommandList(description, graphicsDevice->GetFeatures(), graphicsDevice->GetUniformBufferMinOffsetAlignment(), graphicsDevice->GetStructuredBufferMinOffsetAlignment())
{
  GraphicsDevice = graphicsDevice;
  HRESULT hr = GraphicsDevice->GetDevice()->CreateDeferredContext(0, &Context);

  hr = Context->QueryInterface<ID3D11DeviceContext1>(&Context1);

  hr = Context->QueryInterface<ID3DUserDefinedAnnotation>(&UDA);
}

ID3D11CommandList* D3D11CommandList::GetDeviceCommandList() const
{
  return CommandList;
}

//TODO: private friend

ID3D11DeviceContext* D3D11CommandList::GetDeviceContext() const
{
  return Context;
}

void D3D11CommandList::Reset()
{
  if (CommandList != nullptr)
  {
    CommandList->Release();
    CommandList = nullptr;
  }
  else if (Begun)
  {
    Context->ClearState();
    Context->FinishCommandList(false, &CommandList);
    CommandList->Release();
    CommandList = nullptr;
  }

  ResetManagedState();
  Begun = false;
}

D3D11Framebuffer* D3D11CommandList::GetD3D11Framebuffer() const
{
  D3D11Framebuffer* fb = Util::AssertSubtype<RHIFramebuffer, D3D11Framebuffer>(m_pFramebuffer);
  return fb;
}

void D3D11CommandList::ClearState()
{
  ClearCachedState();
  Context->ClearState();
  ResetManagedState();
}

void D3D11CommandList::ResetManagedState()
{
  // TODO: ensure Clear does not remove array elements and just set them to unitialized instead
  NumVertexBindings = 0;

  Util::ClearArray(VertexBindings);
  Util::ClearArray(VertexStrides);
  Util::ClearArray(VertexOffsets);

  m_pFramebuffer = nullptr;

  Util::ClearArray(Viewports);
  Util::ClearArray(Scissors);

  ViewportsChanged = false;
  ScissorRectsChanged = false;

  _ib = nullptr;
  GraphicsPipeline = nullptr;
  BlendState = nullptr;
  DepthStencilState = nullptr;
  RasterizerState = nullptr;
  PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
  InputLayout = nullptr;
  VertexShader = nullptr;
  GeometryShader = nullptr;
  HullShader = nullptr;
  DomainShader = nullptr;
  PixelShader = nullptr;

  ClearSets(GraphicsResourceSets);

  Util::ClearArray(VertexBoundUniformBuffers);
  Util::ClearArray(VertexBoundTextureViews);
  Util::ClearArray(VertexBoundSamplers);

  Util::ClearArray(FragmentBoundUniformBuffers);
  Util::ClearArray(FragmentBoundTextureViews);
  Util::ClearArray(FragmentBoundSamplers);

  ComputePipeline = nullptr;
  ClearSets(ComputeResourceSets);

  for (auto kvp : BoundSRVs)
  {
    ezDynamicArray<BoundTextureInfo> list = kvp.Value();
    list.Clear();
    PoolBoundTextureList(list);
  }
  BoundSRVs.Clear();

  for (auto kvp : BoundUAVs)
  {
    ezDynamicArray<BoundTextureInfo> list = kvp.Value();
    list.Clear();
    PoolBoundTextureList(list);
  }
  BoundUAVs.Clear();
}

void D3D11CommandList::ClearSets(ezDynamicArray<RHIBoundResourceSetInfo>& boundSets)
{
  for (RHIBoundResourceSetInfo& boundSetInfo : boundSets)
  {
    // TODO
    ezUInt32 offsetsCount = boundSetInfo.Offsets.GetCount();
    boundSetInfo.Offsets.Clear();
    boundSetInfo.Offsets.SetCountUninitialized(offsetsCount);
  }
  Util::ClearArray(boundSets);
}

void D3D11CommandList::UnbindUAVTexture(RHITexture* target)
{
  ezDynamicArray<BoundTextureInfo> btis;

  if (BoundUAVs.TryGetValue(target, btis))
  {
    for (BoundTextureInfo bti : btis)
    {
      BindUnorderedAccessView(nullptr, nullptr, nullptr, bti.Slot, bti.Stages, bti.ResourceSet);
      if ((bti.Stages & RHIShaderStages::Compute) == RHIShaderStages::Compute)
      {
        InvalidatedComputeResourceSets[bti.ResourceSet] = true;
      }
      else
      {
        InvalidatedGraphicsResourceSets[bti.ResourceSet] = true;
      }
    }

    bool result = BoundUAVs.Remove(target);
    EZ_ASSERT_DEBUG(result, "Failed to remove bound UAV");

    btis.Clear();
    PoolBoundTextureList(btis);
  }
}

void D3D11CommandList::UnbindUAVBuffer(RHIBuffer* buffer)
{
  UnbindUAVBufferIndividual(buffer, false);
  UnbindUAVBufferIndividual(buffer, true);
}

void D3D11CommandList::UnbindUAVBufferIndividual(RHIBuffer* buffer, bool compute)
{
  ezDynamicArray<std::tuple<RHIBuffer*, ezInt32>> list = compute ? BoundComputeUAVBuffers : BoundOMUAVBuffers;
  for (ezUInt32 i = 0; i < list.GetCount(); i++)
  {
    if (std::get<0>(list[i]) == buffer)
    {
      ezUInt32 slot = std::get<1>(list[i]);
      if (compute)
      {
        Context->CSSetUnorderedAccessViews(slot, 1, nullptr, nullptr);
      }
      else
      {
        Context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, slot, 1, nullptr, nullptr);
      }

      list.RemoveAndCopy(list[i]);
      i -= 1;
    }
  }
}

void D3D11CommandList::TrackBoundUAVBuffer(RHIBuffer* buffer, ezUInt32 slot, bool compute)
{
  ezDynamicArray<std::tuple<RHIBuffer*, ezInt32>> list = compute ? BoundComputeUAVBuffers : BoundOMUAVBuffers;

  list.PushBack(std::make_tuple(buffer, slot));
}

void D3D11CommandList::BindUnorderedAccessView(RHITexture* texture, RHIBuffer* buffer, ID3D11UnorderedAccessView* uav, ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezUInt32 resourceSet)
{
  bool compute = stages == RHIShaderStages::Compute;
  EZ_ASSERT_DEBUG(compute || ((stages & RHIShaderStages::Compute) == 0), "Problem");
  EZ_ASSERT_DEBUG(texture == nullptr || buffer == nullptr, "Problem");

  if (texture != nullptr && uav != nullptr)
  {
    ezDynamicArray<BoundTextureInfo> list;
    if (!BoundUAVs.TryGetValue(texture, list))
    {
      list = GetNewOrCachedBoundTextureInfoList();
      BoundUAVs.Insert(texture, list);
    }
    list.PushBack(BoundTextureInfo(slot, stages, resourceSet));
  }

  ezUInt32 baseSlot = 0;
  if (!compute && FragmentBoundSamplers.GetCount() > 0)
  {
    baseSlot = m_pFramebuffer->GetColorTargets().GetCount();
  }
  int actualSlot = baseSlot + slot;

  if (buffer != nullptr)
  {
    TrackBoundUAVBuffer(buffer, actualSlot, compute);
  }

  if (compute)
  {
    Context->CSSetUnorderedAccessViews(actualSlot, 1, &uav, nullptr);
  }
  else
  {
    Context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, actualSlot, 1, &uav, nullptr);
  }
}

void D3D11CommandList::PackRangeParams(D3D11BufferRange* range)
{
  CBOut[0] = range->Buffer->GetBuffer();
  FirstConstRef[0] = range->Offset / 16;
  ezUInt32 roundedSize = range->Size < 256 ? 256u : range->Size;
  NumConstsRef[0] = roundedSize / 16;
}

void D3D11CommandList::BindStorageBufferView(D3D11BufferRange* range, int slot, ezBitflags<RHIShaderStages> stages)
{
  bool compute = (stages & RHIShaderStages::Compute) != 0;
  UnbindUAVBuffer(range->Buffer);

  ID3D11ShaderResourceView* srv = range->Buffer->GetShaderResourceView(range->Offset, range->Size);

  if ((stages & RHIShaderStages::Vertex) == RHIShaderStages::Vertex)
  {
    Context->VSSetShaderResources(slot, 1, &srv);
  }
  if ((stages & RHIShaderStages::Geometry) == RHIShaderStages::Geometry)
  {
    Context->GSSetShaderResources(slot, 1, &srv);
  }
  if ((stages & RHIShaderStages::TessellationControl) == RHIShaderStages::TessellationControl)
  {
    Context->HSSetShaderResources(slot, 1, &srv);
  }
  if ((stages & RHIShaderStages::TessellationEvaluation) == RHIShaderStages::TessellationEvaluation)
  {
    Context->DSSetShaderResources(slot, 1, &srv);
  }
  if ((stages & RHIShaderStages::Fragment) == RHIShaderStages::Fragment)
  {
    Context->PSSetShaderResources(slot, 1, &srv);
  }
  if (compute)
  {
    Context->CSSetShaderResources(slot, 1, &srv);
  }
}

void D3D11CommandList::BindUniformBuffer(D3D11BufferRange* range, ezUInt32 slot, ezBitflags<RHIShaderStages> stages)
{
  if ((stages & RHIShaderStages::Vertex) == RHIShaderStages::Vertex)
  {
    bool bind = false;
    if (slot < MaxCachedUniformBuffers)
    {
      if (!(*VertexBoundUniformBuffers[slot] == *range))
      {
        VertexBoundUniformBuffers[slot] = range;
        bind = true;
      }
    }
    else
    {
      bind = true;
    }
    if (bind)
    {
      if (range->IsFullRange())
      {
        ID3D11Buffer* buffer = range->Buffer->GetBuffer();
        Context->VSSetConstantBuffers(slot, 1, &buffer);
      }
      else
      {
        PackRangeParams(range);
        if (!GraphicsDevice->SupportsCommandLists())
        {
          Context->VSSetConstantBuffers(slot, 1, nullptr);
        }
        Context1->VSSetConstantBuffers1(slot, 1, CBOut.GetData(), FirstConstRef.GetData(), NumConstsRef.GetData());
      }
    }
  }
  if ((stages & RHIShaderStages::Geometry) == RHIShaderStages::Geometry)
  {
    if (range->IsFullRange())
    {
      ID3D11Buffer* buffer = range->Buffer->GetBuffer();
      Context->GSSetConstantBuffers(slot, 1, &buffer);
    }
    else
    {
      PackRangeParams(range);
      if (!GraphicsDevice->SupportsCommandLists())
      {
        Context->GSSetConstantBuffers(slot, 1, nullptr);
      }
      Context1->GSSetConstantBuffers1(slot, 1, CBOut.GetData(), FirstConstRef.GetData(), NumConstsRef.GetData()); // todo: check vortice
    }
  }
  if ((stages & RHIShaderStages::TessellationControl) == RHIShaderStages::TessellationControl)
  {
    if (range->IsFullRange())
    {
      ID3D11Buffer* buffer = range->Buffer->GetBuffer();
      Context->HSSetConstantBuffers(slot, 1, &buffer);
    }
    else
    {
      PackRangeParams(range);
      if (!GraphicsDevice->SupportsCommandLists())
      {
        Context->HSSetConstantBuffers(slot, 1, nullptr);
      }
      Context1->HSSetConstantBuffers1(slot, 1, CBOut.GetData(), FirstConstRef.GetData(), NumConstsRef.GetData());
    }
  }
  if ((stages & RHIShaderStages::TessellationEvaluation) == RHIShaderStages::TessellationEvaluation)
  {
    if (range->IsFullRange())
    {
      ID3D11Buffer* buffer = range->Buffer->GetBuffer();
      Context->DSSetConstantBuffers(slot, 1, &buffer);
    }
    else
    {
      PackRangeParams(range);
      if (!GraphicsDevice->SupportsCommandLists())
      {
        Context->DSSetConstantBuffers(slot, 1, nullptr);
      }
      Context1->DSSetConstantBuffers1(slot, 1, CBOut.GetData(), FirstConstRef.GetData(), NumConstsRef.GetData());
    }
  }
  if ((stages & RHIShaderStages::Fragment) == RHIShaderStages::Fragment)
  {
    bool bind = false;
    if (slot < MaxCachedUniformBuffers)
    {
      if (!(*FragmentBoundUniformBuffers[slot] == *range))
      {
        FragmentBoundUniformBuffers[slot] = range;
        bind = true;
      }
    }
    else
    {
      bind = true;
    }
    if (bind)
    {
      if (range->IsFullRange())
      {
        ID3D11Buffer* buffer = range->Buffer->GetBuffer();
        Context->PSSetConstantBuffers(slot, 1, &buffer);
      }
      else
      {
        PackRangeParams(range);
        if (!GraphicsDevice->SupportsCommandLists())
        {
          Context->PSSetConstantBuffers(slot, 1, nullptr);
        }
        Context1->PSSetConstantBuffers1(slot, 1, CBOut.GetData(), FirstConstRef.GetData(), NumConstsRef.GetData());
      }
    }
  }
  if ((stages & RHIShaderStages::Compute) == RHIShaderStages::Compute)
  {
    if (range->IsFullRange())
    {
      ID3D11Buffer* buffer = range->Buffer->GetBuffer();
      Context->CSSetConstantBuffers(slot, 1, &buffer);
    }
    else
    {
      PackRangeParams(range);
      if (!GraphicsDevice->SupportsCommandLists())
      {
        Context->CSSetConstantBuffers(slot, 1, nullptr);
      }
      Context1->CSSetConstantBuffers1(slot, 1, CBOut.GetData(), FirstConstRef.GetData(), NumConstsRef.GetData());
    }
  }
}

void D3D11CommandList::BindSampler(D3D11Sampler* sampler, ezUInt32 slot, ezBitflags<RHIShaderStages> stages)
{
  if ((stages & RHIShaderStages::Vertex) == RHIShaderStages::Vertex)
  {
    bool bind = false;
    if (slot < MaxCachedSamplers)
    {
      if (VertexBoundSamplers[slot] != sampler)
      {
        VertexBoundSamplers[slot] = sampler;
        bind = true;
      }
    }
    else
    {
      bind = true;
    }
    if (bind)
    {
      ID3D11SamplerState* samplerState = sampler->GetDeviceSampler();
      Context->VSSetSamplers(slot, 1, &samplerState);
    }
  }
  if ((stages & RHIShaderStages::Geometry) == RHIShaderStages::Geometry)
  {
    ID3D11SamplerState* samplerState = sampler->GetDeviceSampler();
    Context->GSSetSamplers(slot, 1, &samplerState);
  }
  if ((stages & RHIShaderStages::TessellationControl) == RHIShaderStages::TessellationControl)
  {
    ID3D11SamplerState* samplerState = sampler->GetDeviceSampler();
    Context->HSSetSamplers(slot, 1, &samplerState);
  }
  if ((stages & RHIShaderStages::TessellationEvaluation) == RHIShaderStages::TessellationEvaluation)
  {
    ID3D11SamplerState* samplerState = sampler->GetDeviceSampler();
    Context->DSSetSamplers(slot, 1, &samplerState);
  }
  if ((stages & RHIShaderStages::Fragment) == RHIShaderStages::Fragment)
  {
    bool bind = false;
    if (slot < MaxCachedSamplers)
    {
      if (FragmentBoundSamplers[slot] != sampler)
      {
        FragmentBoundSamplers[slot] = sampler;
        bind = true;
      }
    }
    else
    {
      bind = true;
    }
    if (bind)
    {
      ID3D11SamplerState* samplerState = sampler->GetDeviceSampler();
      Context->PSSetSamplers(slot, 1, &samplerState);
    }
  }
}

void D3D11CommandList::BindTextureView(D3D11TextureView* texView, ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezUInt32 resourceSet)
{
  ID3D11ShaderResourceView* srv = texView != nullptr ? texView->GetShaderResourceView() : nullptr;

  if (srv != nullptr)
  {
    ezDynamicArray<BoundTextureInfo> list;
    if (!BoundSRVs.TryGetValue(texView->GetTarget(), list))
    {
      list = GetNewOrCachedBoundTextureInfoList();
      BoundSRVs.Insert(texView->GetTarget(), list);
    }
    list.PushBack(BoundTextureInfo(slot, stages, resourceSet));
  }

  if ((stages & RHIShaderStages::Vertex) == RHIShaderStages::Vertex)
  {
    bool bind = false;
    if (slot < MaxCachedUniformBuffers)
    {
      if (VertexBoundTextureViews[slot] != texView)
      {
        VertexBoundTextureViews[slot] = texView;
        bind = true;
      }
    }
    else
    {
      bind = true;
    }
    if (bind)
    {
      Context->VSSetShaderResources(slot, 1, &srv);
    }
  }
  if ((stages & RHIShaderStages::Geometry) == RHIShaderStages::Geometry)
  {
    Context->GSSetShaderResources(slot, 1, &srv);
  }
  if ((stages & RHIShaderStages::TessellationControl) == RHIShaderStages::TessellationControl)
  {
    Context->HSSetShaderResources(slot, 1, &srv);
  }
  if ((stages & RHIShaderStages::TessellationEvaluation) == RHIShaderStages::TessellationEvaluation)
  {
    Context->DSSetShaderResources(slot, 1, &srv);
  }
  if ((stages & RHIShaderStages::Fragment) == RHIShaderStages::Fragment)
  {
    bool bind = false;
    if (slot < MaxCachedUniformBuffers)
    {
      if (FragmentBoundTextureViews[slot] != texView)
      {
        FragmentBoundTextureViews[slot] = texView;
        bind = true;
      }
    }
    else
    {
      bind = true;
    }
    if (bind)
    {
      Context->PSSetShaderResources(slot, 1, &srv);
    }
  }
  if ((stages & RHIShaderStages::Compute) == RHIShaderStages::Compute)
  {
    Context->CSSetShaderResources(slot, 1, &srv);
  }
}

void D3D11CommandList::UpdateSubresource_Workaround(ID3D11Resource* resource, int subresource, RawRect region, ezUInt8* data)
{
  bool needWorkaround = !GraphicsDevice->SupportsCommandLists();
  void* pAdjustedSrcData = data;
  if (needWorkaround)
  {
    EZ_ASSERT_DEBUG(region.top == 0 && region.front == 0, "Region Top and Front are not 0.");
    pAdjustedSrcData = (byte*)data - region.left;
  }

  D3D11_BOX* subregion = new D3D11_BOX;
  subregion->left = region.left;
  subregion->right = region.right;
  subregion->bottom = region.bottom;
  subregion->back = region.back;
  subregion->front = region.front;
  subregion->top = region.top;

  Context->UpdateSubresource(resource, subresource, subregion, pAdjustedSrcData, 0, 0);
}

D3D11DeviceBuffer* D3D11CommandList::GetFreeStagingBuffer(ezUInt32 size)
{
  for (D3D11DeviceBuffer* buffer : AvailableStagingBuffers)
  {
    if (buffer->GetSize() >= size)
    {
      AvailableStagingBuffers.RemoveAndCopy(buffer);
      return buffer;
    }
  }

  RHIBuffer* staging = GraphicsDevice->GetResourceFactory()->CreateBuffer(
    RHIBufferDescription(size, RHIBufferUsage::Staging));

  return Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(staging);
}

ezDynamicArray<D3D11CommandList::BoundTextureInfo> D3D11CommandList::GetNewOrCachedBoundTextureInfoList()
{
  if (BoundTextureInfoPool.GetCount() > 0)
  {
    ezUInt32 index = BoundTextureInfoPool.GetCount() - 1;
    ezDynamicArray<BoundTextureInfo> ret = BoundTextureInfoPool[index];
    BoundTextureInfoPool.RemoveAndCopy(ret);

    return ret;
  }

  return ezDynamicArray<BoundTextureInfo>();
}

void D3D11CommandList::OnCompleted()
{
  CommandList->Release();
  CommandList = nullptr;

  for (D3D11Swapchain* sc : ReferencedSwapchains)
  {
    sc->RemoveCommandListReference(this);
  }
  ReferencedSwapchains.Clear();

  for (D3D11DeviceBuffer* buffer : SubmittedStagingBuffers)
  {
    AvailableStagingBuffers.PushBack(buffer);
  }

  SubmittedStagingBuffers.Clear();
}

void D3D11CommandList::FlushVertexBindings()
{
  if (VertexBindingsChanged)
  {
    Context->IASetVertexBuffers(
      0, NumVertexBindings,
      VertexBindings.GetData(),
      VertexStrides.GetData(),
      VertexOffsets.GetData());

    VertexBindingsChanged = false;
  }
}

void D3D11CommandList::FlushScissorRects()
{
  if (ScissorRectsChanged)
  {
    ScissorRectsChanged = false;
    if (Scissors.GetCount() > 0)
    {
      // Because this array is resized using Util.EnsureMinimumArraySize, this might set more scissor rectangles
      // than are actually needed, but this is okay -- extras are essentially ignored and should be harmless.

      ezDynamicArray<D3D11_RECT*> d3dRects;
      d3dRects.SetCountUninitialized(Scissors.GetCount());

      for (auto rect : Scissors)
      {
        D3D11_RECT d3dRect;
        d3dRect.left = rect.left;
        d3dRect.top = rect.top;
        d3dRect.right = rect.right;
        d3dRect.bottom = rect.bottom;

        d3dRects.PushBack(&d3dRect);
      }

      Context->RSSetScissorRects(Scissors.GetCount(), *d3dRects.GetData());
    }
  }
}

void D3D11CommandList::FlushViewports()
{
  if (ViewportsChanged)
  {
    ViewportsChanged = false;

    std::vector<D3D11_VIEWPORT> vps;
    for (auto viewport : Viewports)
    {
      D3D11_VIEWPORT vp;
      vp.TopLeftX = viewport.X;
      vp.TopLeftY = viewport.Y;
      vp.Width = viewport.Width;
      vp.Height = viewport.Height;
      vp.MinDepth = viewport.MinDepth;
      vp.MaxDepth = viewport.MaxDepth;

      vps.push_back(vp);
    }

    //vp.TopLeftX = viewp
    Context->RSSetViewports(Viewports.GetCount(), vps.data());
  }
}

void D3D11CommandList::PreDispatchCommand()
{
  ezUInt32 computeResourceCount = ComputePipeline->GetResourceLayouts().GetCount();
  for (ezUInt32 i = 0; i < computeResourceCount; i++)
  {
    if (InvalidatedComputeResourceSets[i])
    {
      InvalidatedComputeResourceSets[i] = false;
      ActivateResourceSet(i, ComputeResourceSets[i], false);
    }
  }
}

void D3D11CommandList::PreDrawCommand()
{
  FlushViewports();
  FlushScissorRects();
  FlushVertexBindings();

  ezUInt32 graphicsResourceCount = GraphicsPipeline->GetResourceLayouts().GetCount();
  for (ezUInt32 i = 0; i < graphicsResourceCount; i++)
  {
    if (InvalidatedGraphicsResourceSets[i])
    {
      InvalidatedGraphicsResourceSets[i] = false;
      ActivateResourceSet(i, GraphicsResourceSets[i], true);
    }
  }
}

void D3D11CommandList::ActivateResourceSet(ezUInt32 slot, RHIBoundResourceSetInfo brsi, bool graphics)
{
  D3D11ResourceSet* d3d11RS = Util::AssertSubtype<RHIResourceSet, D3D11ResourceSet>(brsi.Set);

  int cbBase = GetConstantBufferBase(slot, graphics);
  int uaBase = GetUnorderedAccessBase(slot, graphics);
  int textureBase = GetTextureBase(slot, graphics);
  int samplerBase = GetSamplerBase(slot, graphics);

  D3D11ResourceLayout* layout = d3d11RS->GetLayout();
  ezDynamicArray<RHIResource*> resources = d3d11RS->GetResources();
  ezUInt32 dynamicOffsetIndex = 0;
  for (ezUInt32 i = 0; i < resources.GetCount(); i++)
  {
    RHIResource* resource = resources[i];
    ezUInt32 bufferOffset = 0;
    if (layout->IsDynamicBuffer(i))
    {
      bufferOffset = brsi.Offsets[dynamicOffsetIndex];
      dynamicOffsetIndex += 1;
    }
    D3D11ResourceLayout::ResourceBindingInfo rbi = layout->GetDeviceSlotIndex(i);
    switch (rbi.Kind)
    {
      case RHIResourceKind::UniformBuffer:
      {
        D3D11BufferRange* range = GetBufferRange(resource, bufferOffset);
        BindUniformBuffer(range, cbBase + rbi.Slot, rbi.Stages);
        break;
      }
      case RHIResourceKind::StructuredBufferReadOnly:
      {
        D3D11BufferRange* range = GetBufferRange(resource, bufferOffset);
        BindStorageBufferView(range, textureBase + rbi.Slot, rbi.Stages);
        break;
      }
      case RHIResourceKind::StructuredBufferReadWrite:
      {
        D3D11BufferRange* range = GetBufferRange(resource, bufferOffset);
        ID3D11UnorderedAccessView* uav = range->Buffer->GetUnorderedAccessView(range->Offset, range->Size);
        BindUnorderedAccessView(nullptr, range->Buffer, uav, uaBase + rbi.Slot, rbi.Stages, slot);
      }
      break;
      case RHIResourceKind::TextureReadOnly:
      {
        RHITextureView* texView = Util::GetTextureView(GraphicsDevice, resource);
        D3D11TextureView* d3d11TexView = Util::AssertSubtype<RHITextureView, D3D11TextureView>(texView);
        UnbindUAVTexture(d3d11TexView->GetTarget());
        BindTextureView(d3d11TexView, textureBase + rbi.Slot, rbi.Stages, slot);
      }
      break;
      case RHIResourceKind::TextureReadWrite:
      {
        RHITextureView* rwTexView = Util::GetTextureView(GraphicsDevice, resource);
        D3D11TextureView* d3d11RWTexView = Util::AssertSubtype<RHITextureView, D3D11TextureView>(rwTexView);
        UnbindSRVTexture(d3d11RWTexView->GetTarget());
        BindUnorderedAccessView(d3d11RWTexView->GetTarget(), nullptr, d3d11RWTexView->GetUnorderedAccessView(), uaBase + rbi.Slot, rbi.Stages, slot);
        break;
      }
      case RHIResourceKind::Sampler:
      {

        D3D11Sampler* sampler = Util::AssertSubtype<RHIResource, D3D11Sampler>(resource);
        BindSampler(sampler, samplerBase + rbi.Slot, rbi.Stages);
        break;
      }
      default:
        EZ_REPORT_FAILURE("Invalid RHIResourceKind");
    }
  }
}

D3D11CommandList::D3D11BufferRange* D3D11CommandList::GetBufferRange(RHIResource* resource, ezUInt32 additionalOffset)
{
  D3D11DeviceBuffer* d3d11Buff = dynamic_cast<D3D11DeviceBuffer*>(resource);
  RHIBufferRange* range = dynamic_cast<RHIBufferRange*>(resource);
  if (d3d11Buff)
  {
    return new D3D11BufferRange(d3d11Buff, additionalOffset, d3d11Buff->GetSize());
  }
  else if (range)
  {
    return new D3D11BufferRange(
      Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(range->GetBuffer()),
      range->GetOffset() + additionalOffset,
      range->GetSize());
  }
  else
  {
    EZ_REPORT_FAILURE("Unexpected resource type used in a buffer type slot: {resource.GetType().Name}");
    return nullptr;
  }
}

void D3D11CommandList::UnbindSRVTexture(RHITexture* target)
{
  ezDynamicArray<BoundTextureInfo> btis;
  if (BoundSRVs.TryGetValue(target, btis))
  {
    for (BoundTextureInfo& bti : btis)
    {
      BindTextureView(nullptr, bti.Slot, bti.Stages, 0);

      if ((bti.Stages & RHIShaderStages::Compute) == RHIShaderStages::Compute)
      {
        InvalidatedComputeResourceSets[bti.ResourceSet] = true;
      }
      else
      {
        InvalidatedGraphicsResourceSets[bti.ResourceSet] = true;
      }
    }

    bool result = BoundSRVs.Remove(target);
    EZ_ASSERT_DEBUG(result, "Failed to remove target from Bound SRVs");

    btis.Clear();
    PoolBoundTextureList(btis);
  }
}

ezUInt32 D3D11CommandList::GetConstantBufferBase(ezUInt32 slot, bool graphics)
{
  ezDynamicArray<D3D11ResourceLayout*> layouts = graphics ? GraphicsPipeline->GetResourceLayouts() : ComputePipeline->GetResourceLayouts();
  ezUInt32 ret = 0;
  for (ezUInt32 i = 0; i < slot; i++)
  {
    EZ_ASSERT_DEBUG(layouts[i] != nullptr, "Problem");
    ret += layouts[i]->GetUniformBufferCount();
  }

  return ret;
}

ezUInt32 D3D11CommandList::GetUnorderedAccessBase(ezUInt32 slot, bool graphics)
{
  ezDynamicArray<D3D11ResourceLayout*> layouts = graphics ? GraphicsPipeline->GetResourceLayouts() : ComputePipeline->GetResourceLayouts();
  ezUInt32 ret = 0;
  for (ezUInt32 i = 0; i < slot; i++)
  {
    EZ_ASSERT_DEBUG(layouts[i] != nullptr, "Problem");
    ret += layouts[i]->GetStorageBufferCount();
  }

  return ret;
}

ezUInt32 D3D11CommandList::GetTextureBase(ezUInt32 slot, bool graphics)
{
  ezDynamicArray<D3D11ResourceLayout*> layouts = graphics ? GraphicsPipeline->GetResourceLayouts() : ComputePipeline->GetResourceLayouts();
  ezUInt32 ret = 0;
  for (ezUInt32 i = 0; i < slot; i++)
  {
    EZ_ASSERT_DEBUG(layouts[i] != nullptr, "Problem");
    ret += layouts[i]->GetTextureCount();
  }

  return ret;
}

ezUInt32 D3D11CommandList::GetSamplerBase(ezUInt32 slot, bool graphics)
{
  ezDynamicArray<D3D11ResourceLayout*> layouts = graphics ? GraphicsPipeline->GetResourceLayouts() : ComputePipeline->GetResourceLayouts();
  ezUInt32 ret = 0;
  for (ezUInt32 i = 0; i < slot; i++)
  {
    EZ_ASSERT_DEBUG(layouts[i] != nullptr, "Problem");
    ret += layouts[i]->GetSamplerCount();
  }

  return ret;
}
