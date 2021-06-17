#include <RHIDX11PCH.h>

#include <RHIDX11/Device/CommandListDX11.h>
#include <RHIDX11/Device/GraphicsDeviceDX11.h>
#include <RHIDX11/Device/DX11Utils.h>

using namespace DX11_Internal;

ezRHICommandListDX11::ezRHICommandListDX11()
  : ezRHICommandList()
  , m_pDeviceDX11{nullptr}
{
}

void ezRHICommandListDX11::pso_validate()
{
  if (!dirty_pso)
    return;

  const PipelineState* pso = active_pso;
  const PipelineStateDesc& desc = pso != nullptr ? pso->GetDesc() : PipelineStateDesc();

  auto internal_state = to_internal(pso);

  ID3D11VertexShader* vs = desc.vs == nullptr ? nullptr : static_cast<VertexShader_DX11*>(desc.vs->internal_state.Borrow())->resource.Get();
  if (vs != prev_vs)
  {
    deviceContext->VSSetShader(vs, nullptr, 0);
    prev_vs = vs;

    if (desc.vs != nullptr)
    {
      for (auto& x : desc.vs->auto_samplers)
      {
        BindSampler(VS, &x.sampler, x.slot);
      }
    }
  }
  ID3D11PixelShader* ps = desc.ps == nullptr ? nullptr : static_cast<PixelShader_DX11*>(desc.ps->internal_state.Borrow())->resource.Get();
  if (ps != prev_ps)
  {
    deviceContext->PSSetShader(ps, nullptr, 0);
    prev_ps = ps;

    if (desc.ps != nullptr)
    {
      for (auto& x : desc.ps->auto_samplers)
      {
        BindSampler(PS, &x.sampler, x.slot);
      }
    }
  }
  ID3D11HullShader* hs = desc.hs == nullptr ? nullptr : static_cast<HullShader_DX11*>(desc.hs->internal_state.Borrow())->resource.Get();
  if (hs != prev_hs)
  {
    deviceContext->HSSetShader(hs, nullptr, 0);
    prev_hs = hs;

    if (desc.hs != nullptr)
    {
      for (auto& x : desc.hs->auto_samplers)
      {
        BindSampler(HS, &x.sampler, x.slot);
      }
    }
  }
  ID3D11DomainShader* ds = desc.ds == nullptr ? nullptr : static_cast<DomainShader_DX11*>(desc.ds->internal_state.Borrow())->resource.Get();
  if (ds != prev_ds)
  {
    deviceContext->DSSetShader(ds, nullptr, 0);
    prev_ds = ds;

    if (desc.ds != nullptr)
    {
      for (auto& x : desc.ds->auto_samplers)
      {
        BindSampler(DS, &x.sampler, x.slot);
      }
    }
  }
  ID3D11GeometryShader* gs = desc.gs == nullptr ? nullptr : static_cast<GeometryShader_DX11*>(desc.gs->internal_state.Borrow())->resource.Get();
  if (gs != prev_gs)
  {
    deviceContext->GSSetShader(gs, nullptr, 0);
    prev_gs = gs;

    if (desc.gs != nullptr)
    {
      for (auto& x : desc.gs->auto_samplers)
      {
        BindSampler(GS, &x.sampler, x.slot);
      }
    }
  }

  ID3D11BlendState* bs = desc.bs == nullptr ? nullptr : internal_state->bs.Get();
  if (desc.bs != prev_bs || desc.sampleMask != prev_samplemask ||
      blendFactor.r != prev_blendfactor.r ||
      blendFactor.g != prev_blendfactor.g ||
      blendFactor.b != prev_blendfactor.b ||
      blendFactor.a != prev_blendfactor.a)
  {
    const float fact[4] = {blendFactor.r, blendFactor.g, blendFactor.b, blendFactor.a};
    deviceContext->OMSetBlendState(bs, fact, desc.sampleMask);
    prev_bs = desc.bs;
    prev_blendfactor = blendFactor;
    prev_samplemask = desc.sampleMask;
  }

  ID3D11RasterizerState* rs = desc.rs == nullptr ? nullptr : internal_state->rs.Get();
  if (desc.rs != prev_rs)
  {
    deviceContext->RSSetState(rs);
    prev_rs = desc.rs;
  }

  ID3D11DepthStencilState* dss = desc.dss == nullptr ? nullptr : internal_state->dss.Get();
  if (desc.dss != prev_dss || stencilRef != prev_stencilRef)
  {
    deviceContext->OMSetDepthStencilState(dss, stencilRef);
    prev_dss = desc.dss;
    prev_stencilRef = stencilRef;
  }

  ID3D11InputLayout* il = desc.il == nullptr ? nullptr : internal_state->il.Get();
  if (desc.il != prev_il)
  {
    deviceContext->IASetInputLayout(il);
    prev_il = desc.il;
  }

  if (prev_pt != desc.pt)
  {
    D3D11_PRIMITIVE_TOPOLOGY d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    switch (desc.pt)
    {
      case TRIANGLELIST:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
      case TRIANGLESTRIP:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
      case POINTLIST:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
      case LINELIST:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
      case LINESTRIP:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
      case PATCHLIST:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
        break;
      default:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        break;
    };
    deviceContext->IASetPrimitiveTopology(d3dType);

    prev_pt = desc.pt;
  }
}


void ezRHICommandListDX11::commit_allocations()
{
  // DX11 needs to unmap allocations before it can execute safely

  if (frame_allocator.dirty)
  {
    auto internal_state = to_internal(&frame_allocator.buffer);
    deviceContext->Unmap(internal_state->resource.Get(), 0);
    frame_allocator.dirty = false;
  }
}

void ezRHICommandListDX11::Initialize(ezRHIGraphicsDevice* pDevice)
{
  m_pDeviceDX11 = static_cast<ezRHIGraphicsDeviceDX11*>(pDevice);

  if (deviceContext == nullptr)
  {
    // need to create one more command list:

    HRESULT hr = m_pDeviceDX11->GetD3D11Device()->CreateDeferredContext(0, &deviceContext);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");

    hr = deviceContext.As(&userDefinedAnnotation);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");

    // Temporary allocations will use the following buffer type:
    GPUBufferDesc frameAllocatorDesc;
    frameAllocatorDesc.ByteWidth = 1024 * 1024; // 1 MB starting size
    frameAllocatorDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_INDEX_BUFFER | BIND_VERTEX_BUFFER;
    frameAllocatorDesc.Usage = USAGE_DYNAMIC;
    frameAllocatorDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
    frameAllocatorDesc.MiscFlags = RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    bool success = m_pDeviceDX11->CreateBuffer(&frameAllocatorDesc, nullptr, &frame_allocator.buffer);
    EZ_ASSERT_ALWAYS(success, "");
    m_pDeviceDX11->SetName(&frame_allocator.buffer, "frame_allocator");
  }

  BindPipelineState(nullptr);
  BindComputeShader(nullptr);

  const ezDynamicArray<StaticSampler>& common_samplers = m_pDeviceDX11->GetCommonSamplers();
  for (int stage = 0; stage < SHADERSTAGE_COUNT; ++stage)
  {
    for (auto& sam : common_samplers)
    {
      BindSampler((SHADERSTAGE)stage, &sam.sampler, sam.slot);
    }
  }

  D3D11_RECT pRects[8];
  for (uint32_t i = 0; i < 8; ++i)
  {
    pRects[i].bottom = INT32_MAX;
    pRects[i].left = INT32_MIN;
    pRects[i].right = INT32_MAX;
    pRects[i].top = INT32_MIN;
  }
  deviceContext->RSSetScissorRects(8, pRects);

  stencilRef = 0;
  blendFactor = ezColor(1, 1, 1, 1);

  prev_vs = {};
  prev_ps = {};
  prev_hs = {};
  prev_ds = {};
  prev_gs = {};
  prev_cs = {};
  prev_blendfactor = {};
  prev_samplemask = {};
  prev_bs = {};
  prev_rs = {};
  prev_stencilRef = {};
  prev_dss = {};
  prev_il = {};
  prev_pt = {};
  swapchains.Clear();

  memset(raster_uavs, 0, sizeof(raster_uavs));
  raster_uavs_slot = {};
  raster_uavs_count = {};

  active_pso = nullptr;
  dirty_pso = false;
  active_renderpass = nullptr;
}

void ezRHICommandListDX11::Destroy(ezRHIGraphicsDevice* pDevice)
{
}

void ezRHICommandListDX11::Submit(ezRHIGraphicsDevice* pDevice)
{
  m_pDeviceDX11 = static_cast<ezRHIGraphicsDeviceDX11*>(pDevice);

  HRESULT hr = deviceContext->FinishCommandList(false, &commandList);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "");
  m_pDeviceDX11->GetD3D11ImmediateContext()->ExecuteCommandList(commandList.Get(), false);
  commandList.Reset();

  for (const auto& swapchain : swapchains)
  {
    to_internal(swapchain)->swapChain->Present(swapchain->desc.vsync, 0);
  }
}


void ezRHICommandListDX11::RenderPassBegin(const SwapChain* swapchain)
{
  swapchains.PushBack(swapchain);
  active_renderpass = &dummyrenderpass;
  auto internal_state = to_internal(swapchain);

  ID3D11RenderTargetView* RTV = internal_state->renderTargetView.Get();
  deviceContext->OMSetRenderTargets(1, &RTV, 0);
  deviceContext->ClearRenderTargetView(RTV, swapchain->desc.clearcolor.GetData());
}
void ezRHICommandListDX11::RenderPassBegin(const RenderPass* renderpass)
{
  active_renderpass = renderpass;
  const RenderPassDesc& desc = renderpass->GetDesc();

  uint32_t rt_count = 0;
  ID3D11RenderTargetView* RTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
  ID3D11DepthStencilView* DSV = nullptr;
  for (auto& attachment : desc.attachments)
  {
    const Texture* texture = attachment.texture;
    int subresource = attachment.subresource;
    auto internal_state = to_internal(texture);

    if (attachment.type == RenderPassAttachment::RENDERTARGET)
    {
      if (subresource < 0 || internal_state->subresources_rtv.IsEmpty())
      {
        RTVs[rt_count] = internal_state->rtv.Get();
      }
      else
      {
        EZ_ASSERT_ALWAYS(internal_state->subresources_rtv.GetCount() > size_t(subresource), "Invalid RTV subresource!");
        RTVs[rt_count] = internal_state->subresources_rtv[subresource].Get();
      }

      if (attachment.loadop == RenderPassAttachment::LOADOP_CLEAR)
      {
        deviceContext->ClearRenderTargetView(RTVs[rt_count], texture->desc.clear.color);
      }

      rt_count++;
    }
    else if (attachment.type == RenderPassAttachment::DEPTH_STENCIL)
    {
      if (subresource < 0 || internal_state->subresources_dsv.IsEmpty())
      {
        DSV = internal_state->dsv.Get();
      }
      else
      {
        EZ_ASSERT_ALWAYS(internal_state->subresources_dsv.GetCount() > size_t(subresource), "Invalid DSV subresource!");
        DSV = internal_state->subresources_dsv[subresource].Get();
      }

      if (attachment.loadop == RenderPassAttachment::LOADOP_CLEAR)
      {
        uint32_t _flags = D3D11_CLEAR_DEPTH;
        if (m_pDeviceDX11->IsFormatStencilSupport(texture->desc.Format))
          _flags |= D3D11_CLEAR_STENCIL;
        deviceContext->ClearDepthStencilView(DSV, _flags, texture->desc.clear.depthstencil.depth, (ezUInt8)texture->desc.clear.depthstencil.stencil);
      }
    }
  }

  if (raster_uavs_count > 0)
  {
    // UAVs:
    const uint32_t count = raster_uavs_count;
    const uint32_t slot = raster_uavs_slot;

    deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(rt_count, RTVs, DSV, slot, count, &raster_uavs[slot], nullptr);

    raster_uavs_count = 0;
    raster_uavs_slot = 8;
  }
  else
  {
    deviceContext->OMSetRenderTargets(rt_count, RTVs, DSV);
  }
}
void ezRHICommandListDX11::RenderPassEnd()
{
  deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

  // Perform resolves:
  int dst_counter = 0;
  for (auto& attachment : active_renderpass->desc.attachments)
  {
    if (attachment.type == RenderPassAttachment::RESOLVE)
    {
      if (attachment.texture != nullptr)
      {
        auto dst_internal = to_internal(attachment.texture);

        int src_counter = 0;
        for (auto& src : active_renderpass->desc.attachments)
        {
          if (src.type == RenderPassAttachment::RENDERTARGET && src.texture != nullptr)
          {
            if (src_counter == dst_counter)
            {
              auto src_internal = to_internal(src.texture);
              deviceContext->ResolveSubresource(dst_internal->resource.Get(), 0, src_internal->resource.Get(), 0, _ConvertFormat(attachment.texture->desc.Format));
              break;
            }
            src_counter++;
          }
        }
      }

      dst_counter++;
    }
  }
  active_renderpass = nullptr;
}
void ezRHICommandListDX11::BindScissorRects(uint32_t numRects, const Rect* rects)
{
  EZ_ASSERT_ALWAYS(rects != nullptr, "");
  EZ_ASSERT_ALWAYS(numRects <= D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX, "");
  D3D11_RECT pRects[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
  for (uint32_t i = 0; i < numRects; ++i)
  {
    pRects[i].bottom = (LONG)rects[i].bottom;
    pRects[i].left = (LONG)rects[i].left;
    pRects[i].right = (LONG)rects[i].right;
    pRects[i].top = (LONG)rects[i].top;
  }
  deviceContext->RSSetScissorRects(numRects, pRects);
}
void ezRHICommandListDX11::BindViewports(uint32_t NumViewports, const Viewport* pViewports)
{
  EZ_ASSERT_ALWAYS(pViewports != nullptr, "");
  EZ_ASSERT_ALWAYS(NumViewports <= D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX, "");
  D3D11_VIEWPORT d3dViewPorts[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
  for (uint32_t i = 0; i < NumViewports; ++i)
  {
    d3dViewPorts[i].TopLeftX = pViewports[i].TopLeftX;
    d3dViewPorts[i].TopLeftY = pViewports[i].TopLeftY;
    d3dViewPorts[i].Width = pViewports[i].Width;
    d3dViewPorts[i].Height = pViewports[i].Height;
    d3dViewPorts[i].MinDepth = pViewports[i].MinDepth;
    d3dViewPorts[i].MaxDepth = pViewports[i].MaxDepth;
  }
  deviceContext->RSSetViewports(NumViewports, d3dViewPorts);
}
void ezRHICommandListDX11::BindResource(SHADERSTAGE stage, const GPUResource* resource, uint32_t slot, int subresource)
{
  if (resource != nullptr && resource->IsValid())
  {
    auto internal_state = to_internal(resource);
    ID3D11ShaderResourceView* SRV;

    if (subresource < 0)
    {
      SRV = internal_state->srv.Get();
    }
    else
    {
      EZ_ASSERT_ALWAYS(internal_state->subresources_srv.GetCount() > static_cast<size_t>(subresource), "Invalid subresource!");
      SRV = internal_state->subresources_srv[subresource].Get();
    }

    switch (stage)
    {
      case SHADERSTAGE::VS:
        deviceContext->VSSetShaderResources(slot, 1, &SRV);
        break;
      case SHADERSTAGE::HS:
        deviceContext->HSSetShaderResources(slot, 1, &SRV);
        break;
      case SHADERSTAGE::DS:
        deviceContext->DSSetShaderResources(slot, 1, &SRV);
        break;
      case SHADERSTAGE::GS:
        deviceContext->GSSetShaderResources(slot, 1, &SRV);
        break;
      case SHADERSTAGE::PS:
        deviceContext->PSSetShaderResources(slot, 1, &SRV);
        break;
      case SHADERSTAGE::CS:
        deviceContext->CSSetShaderResources(slot, 1, &SRV);
        break;
      default:
        break;
    }
  }
}
void ezRHICommandListDX11::BindResources(SHADERSTAGE stage, const GPUResource* const* resources, uint32_t slot, uint32_t count)
{
  EZ_ASSERT_ALWAYS(count <= 16, "");
  ID3D11ShaderResourceView* srvs[16];
  for (uint32_t i = 0; i < count; ++i)
  {
    srvs[i] = resources[i] != nullptr && resources[i]->IsValid() ? to_internal(resources[i])->srv.Get() : nullptr;
  }

  switch (stage)
  {
    case SHADERSTAGE::VS:
      deviceContext->VSSetShaderResources(slot, count, srvs);
      break;
    case SHADERSTAGE::HS:
      deviceContext->HSSetShaderResources(slot, count, srvs);
      break;
    case SHADERSTAGE::DS:
      deviceContext->DSSetShaderResources(slot, count, srvs);
      break;
    case SHADERSTAGE::GS:
      deviceContext->GSSetShaderResources(slot, count, srvs);
      break;
    case SHADERSTAGE::PS:
      deviceContext->PSSetShaderResources(slot, count, srvs);
      break;
    case SHADERSTAGE::CS:
      deviceContext->CSSetShaderResources(slot, count, srvs);
      break;
    default:
      break;
  }
}
void ezRHICommandListDX11::BindUAV(SHADERSTAGE stage, const GPUResource* resource, uint32_t slot, int subresource)
{
  if (resource != nullptr && resource->IsValid())
  {
    auto internal_state = to_internal(resource);
    ID3D11UnorderedAccessView* UAV;

    if (subresource < 0)
    {
      UAV = internal_state->uav.Get();
    }
    else
    {
      EZ_ASSERT_ALWAYS(internal_state->subresources_uav.GetCount() > static_cast<size_t>(subresource), "Invalid subresource!");
      UAV = internal_state->subresources_uav[subresource].Get();
    }

    if (stage == CS)
    {
      deviceContext->CSSetUnorderedAccessViews(slot, 1, &UAV, nullptr);
    }
    else
    {
      raster_uavs[slot] = UAV;
      raster_uavs_slot = ezMath::Min(raster_uavs_slot, uint8_t(slot));
      raster_uavs_count = ezMath::Max(raster_uavs_count, uint8_t(1));
    }
  }
}
void ezRHICommandListDX11::BindUAVs(SHADERSTAGE stage, const GPUResource* const* resources, uint32_t slot, uint32_t count)
{
  EZ_ASSERT_ALWAYS(slot + count <= 8, "");
  ID3D11UnorderedAccessView* uavs[8];
  for (uint32_t i = 0; i < count; ++i)
  {
    uavs[i] = resources[i] != nullptr && resources[i]->IsValid() ? to_internal(resources[i])->uav.Get() : nullptr;

    if (stage != CS)
    {
      raster_uavs[slot + i] = uavs[i];
    }
  }

  if (stage == CS)
  {
    deviceContext->CSSetUnorderedAccessViews(static_cast<uint32_t>(slot), static_cast<uint32_t>(count), uavs, nullptr);
  }
  else
  {
    raster_uavs_slot = ezMath::Min(raster_uavs_slot, uint8_t(slot));
    raster_uavs_count = ezMath::Max(raster_uavs_count, uint8_t(count));
  }
}
void ezRHICommandListDX11::UnbindResources(uint32_t slot, uint32_t num)
{
  EZ_ASSERT_ALWAYS(num <= EZ_ARRAY_SIZE(__nullBlob), "Extend nullBlob to support more resource unbinding!");
  deviceContext->PSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
  deviceContext->VSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
  deviceContext->GSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
  deviceContext->HSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
  deviceContext->DSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
  deviceContext->CSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
}
void ezRHICommandListDX11::UnbindUAVs(uint32_t slot, uint32_t num)
{
  EZ_ASSERT_ALWAYS(num <= EZ_ARRAY_SIZE(__nullBlob), "Extend nullBlob to support more resource unbinding!");
  deviceContext->CSSetUnorderedAccessViews(slot, num, (ID3D11UnorderedAccessView**)__nullBlob, 0);

  raster_uavs_count = 0;
  raster_uavs_slot = 8;
}
void ezRHICommandListDX11::BindSampler(SHADERSTAGE stage, const Sampler* sampler, uint32_t slot)
{
  if (sampler != nullptr && sampler->IsValid())
  {
    auto internal_state = to_internal(sampler);
    ID3D11SamplerState* SAM = internal_state->resource.Get();

    switch (stage)
    {
      case SHADERSTAGE::VS:
        deviceContext->VSSetSamplers(slot, 1, &SAM);
        break;
      case SHADERSTAGE::HS:
        deviceContext->HSSetSamplers(slot, 1, &SAM);
        break;
      case SHADERSTAGE::DS:
        deviceContext->DSSetSamplers(slot, 1, &SAM);
        break;
      case SHADERSTAGE::GS:
        deviceContext->GSSetSamplers(slot, 1, &SAM);
        break;
      case SHADERSTAGE::PS:
        deviceContext->PSSetSamplers(slot, 1, &SAM);
        break;
      case SHADERSTAGE::CS:
        deviceContext->CSSetSamplers(slot, 1, &SAM);
        break;
      default:
        break;
    }
  }
}
void ezRHICommandListDX11::BindConstantBuffer(SHADERSTAGE stage, const GPUBuffer* buffer, uint32_t slot)
{
  ID3D11Buffer* res = buffer != nullptr && buffer->IsValid() ? (ID3D11Buffer*)to_internal(buffer)->resource.Get() : nullptr;
  switch (stage)
  {
    case SHADERSTAGE::VS:
      deviceContext->VSSetConstantBuffers(slot, 1, &res);
      break;
    case SHADERSTAGE::HS:
      deviceContext->HSSetConstantBuffers(slot, 1, &res);
      break;
    case SHADERSTAGE::DS:
      deviceContext->DSSetConstantBuffers(slot, 1, &res);
      break;
    case SHADERSTAGE::GS:
      deviceContext->GSSetConstantBuffers(slot, 1, &res);
      break;
    case SHADERSTAGE::PS:
      deviceContext->PSSetConstantBuffers(slot, 1, &res);
      break;
    case SHADERSTAGE::CS:
      deviceContext->CSSetConstantBuffers(slot, 1, &res);
      break;
    default:
      break;
  }
}
void ezRHICommandListDX11::BindVertexBuffers(const GPUBuffer* const* vertexBuffers, uint32_t slot, uint32_t count, const uint32_t* strides, const uint32_t* offsets)
{
  EZ_ASSERT_ALWAYS(count <= 8, "");
  ID3D11Buffer* res[8] = {};
  for (uint32_t i = 0; i < count; ++i)
  {
    res[i] = vertexBuffers[i] != nullptr && vertexBuffers[i]->IsValid() ? (ID3D11Buffer*)to_internal(vertexBuffers[i])->resource.Get() : nullptr;
  }
  deviceContext->IASetVertexBuffers(slot, count, res, strides, (offsets != nullptr ? offsets : reinterpret_cast<const uint32_t*>(__nullBlob)));
}
void ezRHICommandListDX11::BindIndexBuffer(const GPUBuffer* indexBuffer, const INDEXBUFFER_FORMAT format, uint32_t offset)
{
  ID3D11Buffer* res = indexBuffer != nullptr && indexBuffer->IsValid() ? (ID3D11Buffer*)to_internal(indexBuffer)->resource.Get() : nullptr;
  deviceContext->IASetIndexBuffer(res, (format == INDEXBUFFER_FORMAT::INDEXFORMAT_16BIT ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT), offset);
}
void ezRHICommandListDX11::BindStencilRef(uint32_t value)
{
  stencilRef = value;
}
void ezRHICommandListDX11::BindBlendFactor(float r, float g, float b, float a)
{
  blendFactor.r = r;
  blendFactor.g = g;
  blendFactor.b = b;
  blendFactor.a = a;
}
void ezRHICommandListDX11::BindPipelineState(const PipelineState* pso)
{
  if (active_pso == pso)
    return;

  active_pso = pso;
  dirty_pso = true;
}
void ezRHICommandListDX11::BindComputeShader(const Shader* cs)
{
  ID3D11ComputeShader* _cs = cs == nullptr ? nullptr : static_cast<ComputeShader_DX11*>(cs->internal_state.Borrow())->resource.Get();
  if (_cs != prev_cs)
  {
    deviceContext->CSSetShader(_cs, nullptr, 0);
    prev_cs = _cs;

    if (cs != nullptr)
    {
      for (auto& x : cs->auto_samplers)
      {
        BindSampler(CS, &x.sampler, x.slot);
      }
    }
  }
}
void ezRHICommandListDX11::Draw(uint32_t vertexCount, uint32_t startVertexLocation)
{
  pso_validate();
  commit_allocations();

  deviceContext->Draw(vertexCount, startVertexLocation);
}
void ezRHICommandListDX11::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation)
{
  pso_validate();
  commit_allocations();

  deviceContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}
void ezRHICommandListDX11::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation)
{
  pso_validate();
  commit_allocations();

  deviceContext->DrawInstanced(vertexCount, instanceCount, startVertexLocation, startInstanceLocation);
}
void ezRHICommandListDX11::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation)
{
  pso_validate();
  commit_allocations();

  deviceContext->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}
void ezRHICommandListDX11::DrawInstancedIndirect(const GPUBuffer* args, uint32_t args_offset)
{
  pso_validate();
  commit_allocations();

  deviceContext->DrawInstancedIndirect((ID3D11Buffer*)to_internal(args)->resource.Get(), args_offset);
}
void ezRHICommandListDX11::DrawIndexedInstancedIndirect(const GPUBuffer* args, uint32_t args_offset)
{
  pso_validate();
  commit_allocations();

  deviceContext->DrawIndexedInstancedIndirect((ID3D11Buffer*)to_internal(args)->resource.Get(), args_offset);
}
void ezRHICommandListDX11::Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ)
{
  commit_allocations();

  deviceContext->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}
void ezRHICommandListDX11::DispatchIndirect(const GPUBuffer* args, uint32_t args_offset)
{
  commit_allocations();

  deviceContext->DispatchIndirect((ID3D11Buffer*)to_internal(args)->resource.Get(), args_offset);
}
void ezRHICommandListDX11::CopyResource(const GPUResource* pDst, const GPUResource* pSrc)
{
  EZ_ASSERT_ALWAYS(pDst != nullptr && pSrc != nullptr, "");
  auto internal_state_src = to_internal(pSrc);
  auto internal_state_dst = to_internal(pDst);
  deviceContext->CopyResource(internal_state_dst->resource.Get(), internal_state_src->resource.Get());
}
void ezRHICommandListDX11::UpdateBuffer(const GPUBuffer* buffer, const void* data, int dataSize)
{
  EZ_ASSERT_ALWAYS(buffer->desc.Usage != USAGE_IMMUTABLE, "Cannot update IMMUTABLE GPUBuffer!");
  EZ_ASSERT_ALWAYS((int)buffer->desc.ByteWidth >= dataSize || dataSize < 0, "Data size is too big!");

  if (dataSize == 0)
  {
    return;
  }

  auto internal_state = to_internal(buffer);

  dataSize = ezMath::Min((int)buffer->desc.ByteWidth, dataSize);

  if (buffer->desc.Usage == USAGE_DYNAMIC)
  {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = deviceContext->Map(internal_state->resource.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "GPUBuffer mapping failed!");
    memcpy(mappedResource.pData, data, (dataSize >= 0 ? dataSize : buffer->desc.ByteWidth));
    deviceContext->Unmap(internal_state->resource.Get(), 0);
  }
  else if (buffer->desc.BindFlags & BIND_CONSTANT_BUFFER || dataSize < 0)
  {
    deviceContext->UpdateSubresource(internal_state->resource.Get(), 0, nullptr, data, 0, 0);
  }
  else
  {
    D3D11_BOX box = {};
    box.left = 0;
    box.right = static_cast<uint32_t>(dataSize);
    box.top = 0;
    box.bottom = 1;
    box.front = 0;
    box.back = 1;
    deviceContext->UpdateSubresource(internal_state->resource.Get(), 0, &box, data, 0, 0);
  }
}
void ezRHICommandListDX11::QueryBegin(const GPUQueryHeap* heap, uint32_t index)
{
  auto internal_state = to_internal(heap);
  deviceContext->Begin(internal_state->resources[index].Get());
}
void ezRHICommandListDX11::QueryEnd(const GPUQueryHeap* heap, uint32_t index)
{
  auto internal_state = to_internal(heap);
  deviceContext->End(internal_state->resources[index].Get());
}

GPUAllocation ezRHICommandListDX11::AllocateGPU(size_t dataSize)
{
  GPUAllocation result;
  if (dataSize == 0)
  {
    return result;
  }

  GPUAllocator& allocator = frame_allocator;
  if (allocator.buffer.desc.ByteWidth <= dataSize)
  {
    // If allocation too large, grow the allocator:
    allocator.buffer.desc.ByteWidth = uint32_t((dataSize + 1) * 2);
    bool success = m_pDeviceDX11->CreateBuffer(&allocator.buffer.desc, nullptr, &allocator.buffer);
    EZ_ASSERT_ALWAYS(success, "");
    m_pDeviceDX11->SetName(&allocator.buffer, "frame_allocator");
    allocator.byteOffset = 0;
  }

  auto internal_state = to_internal(&allocator.buffer);

  allocator.dirty = true;

  size_t position = allocator.byteOffset;
  bool wrap = position == 0 || position + dataSize > allocator.buffer.desc.ByteWidth || allocator.residentFrame != m_pDeviceDX11->GetFrameCount();
  position = wrap ? 0 : position;

  // Issue buffer rename (realloc) on wrap, otherwise just append data:
  D3D11_MAP mapping = wrap ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  HRESULT hr = deviceContext->Map(internal_state->resource.Get(), 0, mapping, 0, &mappedResource);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "GPUBuffer mapping failed!");

  allocator.byteOffset = position + dataSize;
  allocator.residentFrame = m_pDeviceDX11->GetFrameCount();

  result.buffer = &allocator.buffer;
  result.offset = (uint32_t)position;
  result.data = (void*)((size_t)mappedResource.pData + position);
  return result;
}

void ezRHICommandListDX11::EventBegin(const char* name)
{
  ezStringWChar text(name);
  if (text.GetElementCount() > 0)
  {
    userDefinedAnnotation->BeginEvent(text);
  }
}
void ezRHICommandListDX11::EventEnd()
{
  userDefinedAnnotation->EndEvent();
}
void ezRHICommandListDX11::SetMarker(const char* name)
{
  ezStringWChar text(name);
  if (text.GetElementCount() > 0)
  {
    userDefinedAnnotation->SetMarker(text);
  }
}
