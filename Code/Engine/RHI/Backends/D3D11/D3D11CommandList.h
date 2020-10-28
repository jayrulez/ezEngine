#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/BoundResourceSetInfo.h>
#include <RHI/Resources/CommandList.h>
#include <d3d11_1.h>

class D3D11CommandList : public RHICommandList
{

private:
  struct D3D11BufferRange : public ezHashableStruct<D3D11BufferRange>
  {
    D3D11DeviceBuffer* Buffer;
    ezUInt32 Offset;
    ezUInt32 Size;

    bool IsFullRange()
    {
      return Offset == 0 && Size == Buffer->GetSize();
    }

    D3D11BufferRange(D3D11DeviceBuffer* buffer, ezUInt32 offset, ezUInt32 size)
    {
      Buffer = buffer;
      Offset = offset;
      Size = size;
    }

    bool operator==(const D3D11BufferRange& other)
    {
      return Buffer == other.Buffer && Offset == other.Offset && Size == other.Size;
    }
  };

  struct BoundTextureInfo
  {
    ezUInt32 Slot = 0;
    ezBitflags<RHIShaderStages> Stages;
    ezUInt32 ResourceSet = 0;

    BoundTextureInfo() = default;
    BoundTextureInfo(ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezUInt32 resourceSet)
    {
      Slot = slot;
      Stages = stages;
      ResourceSet = resourceSet;
    }
  };

private:
  D3D11GraphicsDevice* GraphicsDevice = nullptr;
  ID3D11DeviceContext* Context = nullptr;
  ID3D11DeviceContext1* Context1 = nullptr;
  ID3DUserDefinedAnnotation* UDA = nullptr;
  bool Begun = false;
  ID3D11CommandList* CommandList = nullptr;

  ezDynamicArray<D3D11_VIEWPORT> Viewports; // = new Vortice.Mathematics.Viewport[0];
  ezDynamicArray<D3D11_RECT> Scissors;
  bool ViewportsChanged = false;
  bool ScissorRectsChanged = false;

  ezUInt32 NumVertexBindings = 0;
  ezStaticArray<ID3D11Buffer*, 1> VertexBindings;
  ezDynamicArray<ezUInt32> VertexStrides;
  ezStaticArray<ezUInt32, 1> VertexOffsets;

  // Cached pipeline State
  RHIDeviceBuffer* _ib = nullptr;
  ezUInt32 _ibOffset = 0;
  ID3D11BlendState* BlendState = nullptr;
  ID3D11DepthStencilState* DepthStencilState = nullptr;
  ezUInt32 StencilReference = 0;
  ID3D11RasterizerState* RasterizerState = nullptr;
  D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
  ID3D11InputLayout* InputLayout = nullptr;
  ID3D11VertexShader* VertexShader = nullptr;
  ID3D11GeometryShader* GeometryShader = nullptr;
  ID3D11HullShader* HullShader = nullptr;
  ID3D11DomainShader* DomainShader = nullptr;
  ID3D11PixelShader* PixelShader = nullptr;

  D3D11Pipeline* GraphicsPipeline = nullptr;
  ezHybridArray<RHIBoundResourceSetInfo, 1> GraphicsResourceSets;
  // Resource sets are invalidated when a new resource set is bound with an incompatible SRV or UAV.
  ezHybridArray<bool, 1> InvalidatedGraphicsResourceSets;

  D3D11Pipeline* ComputePipeline = nullptr;
  ezHybridArray<RHIBoundResourceSetInfo, 1> ComputeResourceSets;
  // Resource sets are invalidated when a new resource set is bound with an incompatible SRV or UAV.
  ezStaticArray<bool, 1> InvalidatedComputeResourceSets;
  bool VertexBindingsChanged = false;
  ezStaticArray<ID3D11Buffer*, 1> CBOut;
  ezStaticArray<ezUInt32, 1> FirstConstRef;
  ezStaticArray<ezUInt32, 1> NumConstsRef;

  // Cached resources
  static constexpr ezUInt32 MaxCachedUniformBuffers = 15;
  ezStaticArray<D3D11BufferRange*, MaxCachedUniformBuffers> VertexBoundUniformBuffers;
  ezStaticArray<D3D11BufferRange*, MaxCachedUniformBuffers> FragmentBoundUniformBuffers;
  static constexpr ezUInt32 MaxCachedTextureViews = 16;
  ezStaticArray<D3D11TextureView*, MaxCachedTextureViews> VertexBoundTextureViews;
  ezStaticArray<D3D11TextureView*, MaxCachedTextureViews> FragmentBoundTextureViews;
  static constexpr ezUInt32 MaxCachedSamplers = 4;
  ezStaticArray<D3D11Sampler*, MaxCachedSamplers> VertexBoundSamplers;
  ezStaticArray<D3D11Sampler*, MaxCachedSamplers> FragmentBoundSamplers;

  ezHashTable<RHITexture*, ezDynamicArray<BoundTextureInfo>> BoundSRVs;
  ezHashTable<RHITexture*, ezDynamicArray<BoundTextureInfo>> BoundUAVs;
  ezHybridArray<ezDynamicArray<BoundTextureInfo>, 20> BoundTextureInfoPool;

  static constexpr ezUInt32 MaxUAVs = 8;
  ezHybridArray<std::tuple<RHIDeviceBuffer*, int>, MaxUAVs> BoundComputeUAVBuffers;
  ezHybridArray<std::tuple<RHIDeviceBuffer*, int>, MaxUAVs> BoundOMUAVBuffers;

  ezDynamicArray<D3D11DeviceBuffer*> AvailableStagingBuffers;
  ezDynamicArray<D3D11DeviceBuffer*> SubmittedStagingBuffers;

  ezDynamicArray<D3D11Swapchain*> ReferencedSwapchains;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override { return Name; }

  virtual void SetName(const ezString& name) override
  {
    Name = name;

    if (Context != nullptr)
      Context->SetPrivateData(WKPDID_D3DDebugObjectName, name.GetCharacterCount(), name.GetData());
  }
  virtual bool IsDisposed() const override { return Disposed; }
  virtual void Dispose() override
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

protected:
  virtual void BeginCore() override
  {
    if (CommandList != nullptr)
    {
      CommandList->Release();
      CommandList = nullptr;
      ClearState();
      Begun = true;
    }
  }
  virtual void EndCore() override
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

  virtual void SetPipelineCore(RHIPipeline* pipeline) override
  {
    if (!pipeline->IsComputePipeline() && GraphicsPipeline != pipeline)
    {
      D3D11Pipeline* d3dPipeline = RHIUtils::AssertSubtype<RHIPipeline, D3D11Pipeline>(pipeline);

      GraphicsPipeline = d3dPipeline;
      ClearSets(GraphicsResourceSets); // Invalidate resource set bindings -- they may be invalid.
      RHIUtils::ClearArray(InvalidatedGraphicsResourceSets);

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

      D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = d3dPipeline->GetPrimitiveTopology();
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
      D3D11Pipeline* d3dPipeline = RHIUtils::AssertSubtype<RHIPipeline, D3D11Pipeline>(pipeline);
      ComputePipeline = d3dPipeline;
      ClearSets(ComputeResourceSets); // Invalidate resource set bindings -- they may be invalid.
      RHIUtils::ClearArray(InvalidatedComputeResourceSets);

      ID3D11ComputeShader* computeShader = d3dPipeline->GetComputeShader();
      Context->CSSetShader(computeShader, nullptr, 0);
      ComputeResourceSets.EnsureCount(d3dPipeline->GetResourceLayouts().GetCount());
      InvalidatedComputeResourceSets.EnsureCount(d3dPipeline->GetResourceLayouts().GetCount());
    }
  }
  virtual void SetVertexBufferCore(ezUInt32 index, RHIDeviceBuffer* buffer, ezUInt32 offset) override
  {
    D3D11DeviceBuffer* d3d11Buffer = RHIUtils::AssertSubtype<RHIDeviceBuffer, D3D11DeviceBuffer>(buffer);
    if (VertexBindings[index] != d3d11Buffer->GetBuffer() || VertexOffsets[index] != offset)
    {
      VertexBindingsChanged = true;
      UnbindUAVBuffer(buffer);
      VertexBindings[index] = d3d11Buffer->GetBuffer();
      VertexOffsets[index] = (int)offset;
      NumVertexBindings = ezMath::Max((index + 1), NumVertexBindings);
    }
  }

  virtual void SetIndexBufferCore(RHIDeviceBuffer* buffer, ezEnum<RHIIndexFormat> format, ezUInt32 offset) override
  {
    if (_ib != buffer || _ibOffset != offset)
    {
      _ib = buffer;
      _ibOffset = offset;
      D3D11DeviceBuffer* d3d11Buffer = RHIUtils::AssertSubtype<RHIDeviceBuffer, D3D11DeviceBuffer>(buffer);

      UnbindUAVBuffer(buffer);
      Context->IASetIndexBuffer(d3d11Buffer->GetBuffer(), D3D11FormatUtils::ToDxgiFormat(format), offset);
    }
  }

  virtual void SetGraphicsResourceSetCore(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets) override
  {
    if (GraphicsResourceSets[slot].Equals(resourceSet, dynamicOffsetsCount, dynamicOffsets))
    {
      return;
    }

    GraphicsResourceSets[slot].Offsets.Clear();
    GraphicsResourceSets[slot] = RHIBoundResourceSetInfo(resourceSet /* TODO:, dynamicOffsetsCount*/, dynamicOffsets);
    ActivateResourceSet(slot, GraphicsResourceSets[slot], true);
  }

  virtual void SetComputeResourceSetCore(ezUInt32 slot, RHIResourceSet* resourceSet, ezUInt32 dynamicOffsetsCount, const ezDynamicArray<ezUInt32>& dynamicOffsets) override
  {
    if (ComputeResourceSets[slot].Equals(resourceSet, dynamicOffsetsCount, dynamicOffsets))
    {
      return;
    }

    ComputeResourceSets[slot].Offsets.Clear();
    ComputeResourceSets[slot] = RHIBoundResourceSetInfo(resourceSet, /*TODO dynamicOffsetsCount,*/ dynamicOffsets);
    ActivateResourceSet(slot, ComputeResourceSets[slot], false);
  }

  virtual void SetFramebufferCore(RHIFramebuffer* framebuffer) override
  {
    D3D11Framebuffer* d3dFB = RHIUtils::AssertSubtype<RHIFramebuffer, D3D11Framebuffer>(framebuffer);
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

  virtual void ClearColorTargetCore(ezUInt32 index, ezColor clearColor) override
  {
    float color[4] = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
    Context->ClearRenderTargetView(GetD3D11Framebuffer()->GetRenderTargetViews()[index], color);
  }

  virtual void ClearDepthStencilCore(float depth, ezUInt8 stencil) override
  {
    Context->ClearDepthStencilView(GetD3D11Framebuffer()->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
  }
  virtual void SetViewportCore(ezUInt32 index, const RHIViewport& viewport) override
  {
    ViewportsChanged = true;
    Viewports.EnsureCount(index + 1);

    D3D11_VIEWPORT vp;
    vp.TopLeftX = viewport.X;
    vp.TopLeftY = viewport.Y;
    vp.Width = viewport.Width;
    vp.Height = viewport.Height;
    vp.MinDepth = viewport.MinDepth;
    vp.MaxDepth = viewport.MaxDepth;

    Viewports[index] = vp;
  }
  virtual void SetScissorRectCore(ezUInt32 index, ezUInt32 x, ezUInt32 y, ezUInt32 width, ezUInt32 height) override
  {
    ScissorRectsChanged = true;
    Scissors.EnsureCount(index + 1);
    Scissors[index] = {(ezInt32)x, (ezInt32)y, (ezInt32)(x + width), (ezInt32)(y + height)};
  }
  virtual void DrawCore(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 vertexStart, ezUInt32 instanceStart) override
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

  virtual void DrawIndexedCore(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 indexStart, ezInt32 vertexOffset, ezUInt32 instanceStart) override
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
  virtual void DrawIndirectCore(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride) override
  {
    PreDrawCommand();

    D3D11DeviceBuffer* d3d11Buffer = RHIUtils::AssertSubtype<RHIDeviceBuffer, D3D11DeviceBuffer>(indirectBuffer);
    ezUInt32 currentOffset = offset;
    for (ezUInt32 i = 0; i < drawCount; i++)
    {
      Context->DrawInstancedIndirect(d3d11Buffer->GetBuffer(), currentOffset);
      currentOffset += (int)stride;
    }
  }

  virtual void DrawIndexedIndirectCore(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset, ezUInt32 drawCount, ezUInt32 stride) override
  {
    PreDrawCommand();

    D3D11DeviceBuffer* d3d11Buffer = RHIUtils::AssertSubtype<RHIDeviceBuffer, D3D11DeviceBuffer>(indirectBuffer);
    ezUInt32 currentOffset = offset;
    for (ezUInt32 i = 0; i < drawCount; i++)
    {
      Context->DrawIndexedInstancedIndirect(d3d11Buffer->GetBuffer(), currentOffset);
      currentOffset += (int)stride;
    }
  }

  virtual void DispatchCore(ezUInt32 groupCountX, ezUInt32 groupCountY, ezUInt32 groupCountZ) override
  {
    PreDispatchCommand();

    Context->Dispatch((int)groupCountX, (int)groupCountY, (int)groupCountZ);
  }

  virtual void DispatchIndirectCore(RHIDeviceBuffer* indirectBuffer, ezUInt32 offset) override
  {
    PreDispatchCommand();
    D3D11DeviceBuffer* d3d11Buffer = RHIUtils::AssertSubtype<RHIDeviceBuffer, D3D11DeviceBuffer>(indirectBuffer);
    Context->DispatchIndirect(d3d11Buffer->GetBuffer(), offset);
  }

  virtual void ResolveTextureCore(RHITexture* source, RHITexture* destination) override
  {
    D3D11Texture* d3d11Source = RHIUtils::AssertSubtype<RHITexture, D3D11Texture>(source);
    D3D11Texture* d3d11Destination = RHIUtils::AssertSubtype<RHITexture, D3D11Texture>(destination);
    Context->ResolveSubresource(
      d3d11Destination->GetDeviceTexture(),
      0,
      d3d11Source->GetDeviceTexture(),
      0,
      d3d11Destination->GetDxgiFormat());
  }

  virtual void UpdateBufferCore(RHIDeviceBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size) override
  {
    D3D11DeviceBuffer* d3dBuffer = RHIUtils::AssertSubtype<RHIDeviceBuffer, D3D11DeviceBuffer>(buffer);
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
        UpdateSubresource_Workaround(d3dBuffer->GetBuffer(), 0, *subregion, source);
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
        ezMemoryUtils::CopyOverlapped(msb.pData, reinterpret_cast<void*>(source), size);
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

  virtual void CopyBufferCore(RHIDeviceBuffer* source, ezUInt32 sourceOffset, RHIDeviceBuffer* destination, ezUInt32 destinationOffset, ezUInt32 size) override
  {
    D3D11DeviceBuffer* srcD3D11Buffer = RHIUtils::AssertSubtype<RHIDeviceBuffer, D3D11DeviceBuffer>(source);
    D3D11DeviceBuffer* dstD3D11Buffer = RHIUtils::AssertSubtype<RHIDeviceBuffer, D3D11DeviceBuffer>(destination);

    D3D11_BOX region = {sourceOffset, 0, 0, (sourceOffset + size), 1, 1};

    Context->CopySubresourceRegion(dstD3D11Buffer->GetBuffer(), 0, destinationOffset, 0, 0, srcD3D11Buffer->GetBuffer(), 0, &region);
  }

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
    ezUInt32 layerCount) override
  {
    D3D11Texture* srcD3D11Texture = RHIUtils::AssertSubtype<RHITexture, D3D11Texture>(source);
    D3D11Texture* dstD3D11Texture = RHIUtils::AssertSubtype<RHITexture, D3D11Texture>(destination);

    ezUInt32 blockSize = RHIFormatUtils::IsCompressedFormat(source->GetFormat()) ? 4u : 1u;
    ezUInt32 clampedWidth = ezMath::Max(blockSize, width);
    ezUInt32 clampedHeight = ezMath::Max(blockSize, height);

    D3D11_BOX* region = nullptr;
    if (srcX != 0 || srcY != 0 || srcZ != 0 || clampedWidth != source->GetWidth() || clampedHeight != source->GetHeight() || depth != source->GetDepth())
    {
      D3D11_BOX box{
        (int)srcX,
        (int)srcY,
        (int)srcZ,
        (int)(srcX + clampedWidth),
        (int)(srcY + clampedHeight),
        (int)(srcZ + depth)};
      region = &box;
    }

    for (ezUInt32 i = 0; i < layerCount; i++)
    {
      int srcSubresource = D3D11Util::ComputeSubresource(srcMipLevel, source->GetMipLevels(), srcBaseArrayLayer + i);
      int dstSubresource = D3D11Util::ComputeSubresource(dstMipLevel, destination->GetMipLevels(), dstBaseArrayLayer + i);

      Context->CopySubresourceRegion(
        dstD3D11Texture->GetDeviceTexture(),
        dstSubresource,
        (int)dstX,
        (int)dstY,
        (int)dstZ,
        srcD3D11Texture->GetDeviceTexture(),
        srcSubresource,
        region);
    }
  }

  virtual void GenerateMipmapsCore(RHITexture* texture) override
  {
    RHITextureView* fullTexView = texture->GetFullTextureView(GraphicsDevice);
    D3D11TextureView* d3d11View = RHIUtils::AssertSubtype<RHITextureView, D3D11TextureView>(fullTexView);
    ID3D11ShaderResourceView* srv = d3d11View->GetShaderResourceView();
    Context->GenerateMips(srv);
  }

  virtual void PushDebugGroupCore(ezString name) override
  {
    ezStringWChar wName(name);
    if (UDA != nullptr)
      UDA->BeginEvent(wName);
  }
  virtual void PopDebugGroupCore() override
  {
    if (UDA != nullptr)
      UDA->EndEvent();
  }
  virtual void InsertDebugMarkerCore(ezString name) override
  {
    ezStringWChar wName(name);
    if (UDA != nullptr)
      UDA->SetMarker(wName);
  }

public:
  D3D11CommandList(D3D11GraphicsDevice* graphicsDevice, const RHICommandListDescription& description)
    : RHICommandList(description, graphicsDevice->GetFeatures(), graphicsDevice->GetUniformBufferMinOffsetAlignment(), graphicsDevice->GetStructuredBufferMinOffsetAlignment())
  {
    GraphicsDevice = graphicsDevice;
    HRESULT hr = GraphicsDevice->GetDevice()->CreateDeferredContext(0, &Context);

    hr = Context->QueryInterface<ID3D11DeviceContext1>(&Context1);

    hr = Context->QueryInterface<ID3DUserDefinedAnnotation>(&UDA);
  }

  ID3D11CommandList* GetDeviceCommandList() const
  {
    return CommandList;
  }

  //TODO: private friend
  ID3D11DeviceContext* GetDeviceContext() const
  {
    return Context;
  }

  void Reset()
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

private:
  D3D11Framebuffer* GetD3D11Framebuffer() const
  {
    D3D11Framebuffer* fb = RHIUtils::AssertSubtype<RHIFramebuffer, D3D11Framebuffer>(m_pFramebuffer);
    return fb;
  }

  void ClearState()
  {
    ClearCachedState();
    Context->ClearState();
    ResetManagedState();
  }

  void ResetManagedState()
  {
    // TODO: ensure Clear does not remove array elements and just set them to unitialized instead
    NumVertexBindings = 0;

    RHIUtils::ClearArray(VertexBindings);
    RHIUtils::ClearArray(VertexStrides);
    RHIUtils::ClearArray(VertexOffsets);

    m_pFramebuffer = nullptr;

    RHIUtils::ClearArray(Viewports);
    RHIUtils::ClearArray(Scissors);

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

    RHIUtils::ClearArray(VertexBoundUniformBuffers);
    RHIUtils::ClearArray(VertexBoundTextureViews);
    RHIUtils::ClearArray(VertexBoundSamplers);

    RHIUtils::ClearArray(FragmentBoundUniformBuffers);
    RHIUtils::ClearArray(FragmentBoundTextureViews);
    RHIUtils::ClearArray(FragmentBoundSamplers);

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

  void ClearSets(ezDynamicArray<RHIBoundResourceSetInfo>& boundSets)
  {
    for (RHIBoundResourceSetInfo& boundSetInfo : boundSets)
    {
      // TODO
      ezUInt32 offsetsCount = boundSetInfo.Offsets.GetCount();
      boundSetInfo.Offsets.Clear();
      boundSetInfo.Offsets.SetCountUninitialized(offsetsCount);
    }
    RHIUtils::ClearArray(boundSets);
  }

  void UnbindUAVTexture(RHITexture* target)
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

  void UnbindUAVBuffer(RHIDeviceBuffer* buffer)
  {
    UnbindUAVBufferIndividual(buffer, false);
    UnbindUAVBufferIndividual(buffer, true);
  }

  void UnbindUAVBufferIndividual(RHIDeviceBuffer* buffer, bool compute)
  {
    ezDynamicArray<std::tuple<RHIDeviceBuffer*, ezInt32>> list = compute ? BoundComputeUAVBuffers : BoundOMUAVBuffers;
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

  void TrackBoundUAVBuffer(RHIDeviceBuffer* buffer, ezUInt32 slot, bool compute)
  {
    ezDynamicArray<std::tuple<RHIDeviceBuffer*, ezInt32>> list = compute ? BoundComputeUAVBuffers : BoundOMUAVBuffers;

    list.PushBack(std::make_tuple(buffer, slot));
  }

  void BindUnorderedAccessView(
    RHITexture* texture,
    RHIDeviceBuffer* buffer,
    ID3D11UnorderedAccessView* uav,
    ezUInt32 slot,
    ezBitflags<RHIShaderStages> stages,
    ezUInt32 resourceSet)
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

  void PackRangeParams(D3D11BufferRange* range)
  {
    CBOut[0] = range->Buffer->GetBuffer();
    FirstConstRef[0] = range->Offset / 16;
    ezUInt32 roundedSize = range->Size < 256 ? 256u : range->Size;
    NumConstsRef[0] = roundedSize / 16;
  }

  void BindStorageBufferView(D3D11BufferRange* range, int slot, ezBitflags<RHIShaderStages> stages)
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

  void BindUniformBuffer(D3D11BufferRange* range, ezUInt32 slot, ezBitflags<RHIShaderStages> stages)
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

  void BindSampler(D3D11Sampler* sampler, ezUInt32 slot, ezBitflags<RHIShaderStages> stages)
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

  void BindTextureView(D3D11TextureView* texView, ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezUInt32 resourceSet)
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

  void UpdateSubresource_Workaround(
    ID3D11Resource* resource,
    int subresource,
    D3D11_BOX region,
    ezUInt8* data)
  {
    bool needWorkaround = !GraphicsDevice->SupportsCommandLists();
    void* pAdjustedSrcData = data;
    if (needWorkaround)
    {
      EZ_ASSERT_DEBUG(region.top == 0 && region.front == 0, "Region Top and Front are not 0.");
      pAdjustedSrcData = (byte*)data - region.left;
    }

    Context->UpdateSubresource(resource, subresource, &region, pAdjustedSrcData, 0, 0);
  }


  D3D11DeviceBuffer* GetFreeStagingBuffer(ezUInt32 size)
  {
    for (D3D11DeviceBuffer* buffer : AvailableStagingBuffers)
    {
      if (buffer->GetSize() >= size)
      {
        AvailableStagingBuffers.RemoveAndCopy(buffer);
        return buffer;
      }
    }

    RHIDeviceBuffer* staging = GraphicsDevice->GetResourceFactory()->CreateBuffer(
      RHIBufferDescription(size, RHIBufferUsage::Staging));

    return RHIUtils::AssertSubtype<RHIDeviceBuffer, D3D11DeviceBuffer>(staging);
  }

  ezDynamicArray<BoundTextureInfo> GetNewOrCachedBoundTextureInfoList()
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

  void OnCompleted()
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

  void FlushVertexBindings()
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

  void FlushScissorRects()
  {
    if (ScissorRectsChanged)
    {
      ScissorRectsChanged = false;
      if (Scissors.GetCount() > 0)
      {
        // Because this array is resized using Util.EnsureMinimumArraySize, this might set more scissor rectangles
        // than are actually needed, but this is okay -- extras are essentially ignored and should be harmless.
        Context->RSSetScissorRects(Scissors.GetCount(), Scissors.GetData());
      }
    }
  }

  void FlushViewports()
  {
    if (ViewportsChanged)
    {
      ViewportsChanged = false;
      Context->RSSetViewports(Viewports.GetCount(), Viewports.GetData());
    }
  }

  void PreDispatchCommand()
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

  void PreDrawCommand()
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

  void ActivateResourceSet(ezUInt32 slot, RHIBoundResourceSetInfo brsi, bool graphics)
  {
    D3D11ResourceSet* d3d11RS = RHIUtils::AssertSubtype<RHIResourceSet, D3D11ResourceSet>(brsi.Set);

    int cbBase = GetConstantBufferBase(slot, graphics);
    int uaBase = GetUnorderedAccessBase(slot, graphics);
    int textureBase = GetTextureBase(slot, graphics);
    int samplerBase = GetSamplerBase(slot, graphics);

    D3D11ResourceLayout* layout = d3d11RS->GetLayout();
    ezDynamicArray<RHIDeviceResource*> resources = d3d11RS->GetResources();
    ezUInt32 dynamicOffsetIndex = 0;
    for (ezUInt32 i = 0; i < resources.GetCount(); i++)
    {
      RHIDeviceResource* resource = resources[i];
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
          break;
        }
        case RHIResourceKind::TextureReadOnly:
          RHITextureView* texView = RHIUtils::GetTextureView(GraphicsDevice, resource);
          D3D11TextureView* d3d11TexView = RHIUtils::AssertSubtype<RHITextureView, D3D11TextureView>(texView);
          UnbindUAVTexture(d3d11TexView->GetTarget());
          BindTextureView(d3d11TexView, textureBase + rbi.Slot, rbi.Stages, slot);
          break;
        case RHIResourceKind::TextureReadWrite:
          RHITextureView* rwTexView = RHIUtils::GetTextureView(GraphicsDevice, resource);
          D3D11TextureView* d3d11RWTexView = RHIUtils::AssertSubtype<RHITextureView, D3D11TextureView>(rwTexView);
          UnbindSRVTexture(d3d11RWTexView->GetTarget());
          BindUnorderedAccessView(d3d11RWTexView->GetTarget(), nullptr, d3d11RWTexView->GetUnorderedAccessView(), uaBase + rbi.Slot, rbi.Stages, slot);
          break;
        case RHIResourceKind::Sampler:
          D3D11Sampler* sampler = RHIUtils::AssertSubtype<RHIDeviceResource, D3D11Sampler>(resource);
          BindSampler(sampler, samplerBase + rbi.Slot, rbi.Stages);
          break;
        default:
          EZ_REPORT_FAILURE("Invalid RHIResourceKind");
      }
    }
  }

  D3D11BufferRange* GetBufferRange(RHIDeviceResource* resource, ezUInt32 additionalOffset)
  {
    D3D11DeviceBuffer* d3d11Buff = dynamic_cast<D3D11DeviceBuffer*>(resource);
    RHIDeviceBufferRange* range = dynamic_cast<RHIDeviceBufferRange*>(resource);
    if (d3d11Buff)
    {
      return new D3D11BufferRange(d3d11Buff, additionalOffset, d3d11Buff->GetSize());
    }
    else if (range)
    {
      return new D3D11BufferRange(
        RHIUtils::AssertSubtype<RHIDeviceBuffer, D3D11DeviceBuffer>(range->Buffer),
        range->Offset + additionalOffset,
        range->Size);
    }
    else
    {
      EZ_REPORT_FAILURE("Unexpected resource type used in a buffer type slot: {resource.GetType().Name}");
      return nullptr;
    }
  }

  void UnbindSRVTexture(RHITexture* target)
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

  void PoolBoundTextureList(ezDynamicArray<BoundTextureInfo> btis)
  {
    BoundTextureInfoPool.PushBack(btis);
  }


  ezUInt32 GetConstantBufferBase(ezUInt32 slot, bool graphics)
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

  ezUInt32 GetUnorderedAccessBase(ezUInt32 slot, bool graphics)
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

  ezUInt32 GetTextureBase(ezUInt32 slot, bool graphics)
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

  ezUInt32 GetSamplerBase(ezUInt32 slot, bool graphics)
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
};
