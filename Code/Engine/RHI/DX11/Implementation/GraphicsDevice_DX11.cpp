#include <RHI/DX11/GraphicsDevice_DX11.h>

#ifdef EZ_RHI_D3D11_SUPPORTED
#  include <Foundation/Basics/Platform/Win/HResultUtils.h>
#  include <Foundation/Logging/Log.h>

#include <RHI/DX11/DX11_Internal.h>

#  include <algorithm>

#include <RHI/FormatHelpers.h>

using namespace Microsoft::WRL;

using namespace DX11_Internal;

void GraphicsDevice_DX11::pso_validate(CommandList cmd)
{
  if (!dirty_pso[cmd])
    return;

  const PipelineState* pso = active_pso[cmd];
  const PipelineStateDesc& desc = pso != nullptr ? pso->GetDesc() : PipelineStateDesc();

  ID3D11VertexShader* vs = desc.vs == nullptr ? nullptr : static_cast<VertexShader_DX11*>(desc.vs->internal_state.Borrow())->resource.Get();
  if (vs != prev_vs[cmd])
  {
    deviceContexts[cmd]->VSSetShader(vs, nullptr, 0);
    prev_vs[cmd] = vs;
  }
  ID3D11PixelShader* ps = desc.ps == nullptr ? nullptr : static_cast<PixelShader_DX11*>(desc.ps->internal_state.Borrow())->resource.Get();
  if (ps != prev_ps[cmd])
  {
    deviceContexts[cmd]->PSSetShader(ps, nullptr, 0);
    prev_ps[cmd] = ps;
  }
  ID3D11HullShader* hs = desc.hs == nullptr ? nullptr : static_cast<HullShader_DX11*>(desc.hs->internal_state.Borrow())->resource.Get();
  if (hs != prev_hs[cmd])
  {
    deviceContexts[cmd]->HSSetShader(hs, nullptr, 0);
    prev_hs[cmd] = hs;
  }
  ID3D11DomainShader* ds = desc.ds == nullptr ? nullptr : static_cast<DomainShader_DX11*>(desc.ds->internal_state.Borrow())->resource.Get();
  if (ds != prev_ds[cmd])
  {
    deviceContexts[cmd]->DSSetShader(ds, nullptr, 0);
    prev_ds[cmd] = ds;
  }
  ID3D11GeometryShader* gs = desc.gs == nullptr ? nullptr : static_cast<GeometryShader_DX11*>(desc.gs->internal_state.Borrow())->resource.Get();
  if (gs != prev_gs[cmd])
  {
    deviceContexts[cmd]->GSSetShader(gs, nullptr, 0);
    prev_gs[cmd] = gs;
  }

  ID3D11BlendState* bs = desc.bs == nullptr ? nullptr : to_internal(desc.bs)->resource.Get();
  if (bs != prev_bs[cmd] || desc.sampleMask != prev_samplemask[cmd] ||
      blendFactor[cmd].x != prev_blendfactor[cmd].x ||
      blendFactor[cmd].y != prev_blendfactor[cmd].y ||
      blendFactor[cmd].z != prev_blendfactor[cmd].z ||
      blendFactor[cmd].w != prev_blendfactor[cmd].w)
  {
    const float fact[4] = {blendFactor[cmd].x, blendFactor[cmd].y, blendFactor[cmd].z, blendFactor[cmd].w};
    deviceContexts[cmd]->OMSetBlendState(bs, fact, desc.sampleMask);
    prev_bs[cmd] = bs;
    prev_blendfactor[cmd] = blendFactor[cmd];
    prev_samplemask[cmd] = desc.sampleMask;
  }

  ID3D11RasterizerState* rs = desc.rs == nullptr ? nullptr : to_internal(desc.rs)->resource.Get();
  if (rs != prev_rs[cmd])
  {
    deviceContexts[cmd]->RSSetState(rs);
    prev_rs[cmd] = rs;
  }

  ID3D11DepthStencilState* dss = desc.dss == nullptr ? nullptr : to_internal(desc.dss)->resource.Get();
  if (dss != prev_dss[cmd] || stencilRef[cmd] != prev_stencilRef[cmd])
  {
    deviceContexts[cmd]->OMSetDepthStencilState(dss, stencilRef[cmd]);
    prev_dss[cmd] = dss;
    prev_stencilRef[cmd] = stencilRef[cmd];
  }

  ID3D11InputLayout* il = desc.il == nullptr ? nullptr : to_internal(desc.il)->resource.Get();
  if (il != prev_il[cmd])
  {
    deviceContexts[cmd]->IASetInputLayout(il);
    prev_il[cmd] = il;
  }

  if (prev_pt[cmd] != desc.pt)
  {
    D3D11_PRIMITIVE_TOPOLOGY d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    switch (desc.pt)
    {
      case ezRHIPrimitiveTopology::TriangleList:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
      case ezRHIPrimitiveTopology::TriangleStrip:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
      case ezRHIPrimitiveTopology::PointList:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
      case ezRHIPrimitiveTopology::LineList:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
      case ezRHIPrimitiveTopology::LineStrip:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        break;
      case ezRHIPrimitiveTopology::PatchList:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
        break;
      default:
        d3dType = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        break;
    };
    deviceContexts[cmd]->IASetPrimitiveTopology(d3dType);

    prev_pt[cmd] = desc.pt;
  }
}

// Engine functions
GraphicsDevice_DX11::GraphicsDevice_DX11(RHIWindowType window, bool fullscreen, bool debuglayer)
{
  DEBUGDEVICE = debuglayer;
  FULLSCREEN = fullscreen;

#  ifndef PLATFORM_UWP
  RECT rect = RECT();
  GetClientRect(window, &rect);
  RESOLUTIONWIDTH = rect.right - rect.left;
  RESOLUTIONHEIGHT = rect.bottom - rect.top;
#  else PLATFORM_UWP
  float dpiscale = wiPlatform::GetDPIScaling();
  RESOLUTIONWIDTH = int(window->Bounds.Width * dpiscale);
  RESOLUTIONHEIGHT = int(window->Bounds.Height * dpiscale);
#  endif

  HRESULT hr = E_FAIL;

  ezUInt32 createDeviceFlags = 0;

  if (debuglayer)
  {
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  }

  D3D_DRIVER_TYPE driverTypes[] =
    {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_WARP,
      D3D_DRIVER_TYPE_REFERENCE,
    };
  ezUInt32 numDriverTypes = EZ_ARRAY_SIZE(driverTypes);

  D3D_FEATURE_LEVEL featureLevels[] =
    {
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
    };
  ezUInt32 numFeatureLevels = EZ_ARRAY_SIZE(featureLevels);

  for (ezUInt32 driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
  {
    driverType = driverTypes[driverTypeIndex];
    hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &device, &featureLevel, &immediateContext);

    if (SUCCEEDED(hr))
      break;
  }
  if (FAILED(hr))
  {
    EZ_REPORT_FAILURE("Failed to create the graphics device! ERROR: {0}", ezHRESULTtoString(hr) /*ez TODO: std::hex*/);
  }

  ComPtr<IDXGIDevice2> pDXGIDevice;
  hr = device.As(&pDXGIDevice);

  ComPtr<IDXGIAdapter> pDXGIAdapter;
  hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);

  ComPtr<IDXGIFactory2> pIDXGIFactory;
  pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&pIDXGIFactory);


  DXGI_SWAP_CHAIN_DESC1 sd = {0};
  sd.Width = RESOLUTIONWIDTH;
  sd.Height = RESOLUTIONHEIGHT;
  sd.Format = _ConvertFormat(GetBackBufferFormat());
  sd.Stereo = false;
  sd.SampleDesc.Count = 1; // Don't use multi-sampling.
  sd.SampleDesc.Quality = 0;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.BufferCount = 2; // Use double-buffering to minimize latency.
  sd.Flags = 0;
  sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

#  ifndef PLATFORM_UWP
  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  sd.Scaling = DXGI_SCALING_STRETCH;

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
  fullscreenDesc.RefreshRate.Numerator = 60;
  fullscreenDesc.RefreshRate.Denominator = 1;
  fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // needs to be unspecified for correct fullscreen scaling!
  fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
  fullscreenDesc.Windowed = !fullscreen;
  hr = pIDXGIFactory->CreateSwapChainForHwnd(device.Get(), window, &sd, &fullscreenDesc, nullptr, &swapChain);
#  else
  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
  sd.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;

  hr = pIDXGIFactory->CreateSwapChainForCoreWindow(device.Get(), reinterpret_cast<IUnknown*>(window.Get()), &sd, nullptr, &swapChain);
#  endif

  if (FAILED(hr))
  {
    EZ_REPORT_FAILURE("Failed to create a swapchain for the graphics device!");
  }

  // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
  // ensures that the application will only render after each VSync, minimizing power consumption.
  hr = pDXGIDevice->SetMaximumFrameLatency(1);


  D3D_FEATURE_LEVEL aquiredFeatureLevel = device->GetFeatureLevel();
  TESSELLATION = ((aquiredFeatureLevel >= D3D_FEATURE_LEVEL_11_0) ? true : false);

  //D3D11_FEATURE_DATA_D3D11_OPTIONS features_0;
  //hr = device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS, &features_0, sizeof(features_0));

  //D3D11_FEATURE_DATA_D3D11_OPTIONS1 features_1;
  //hr = device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &features_1, sizeof(features_1));

  D3D11_FEATURE_DATA_D3D11_OPTIONS2 features_2;
  hr = device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS2, &features_2, sizeof(features_2));
  CONSERVATIVE_RASTERIZATION = features_2.ConservativeRasterizationTier >= D3D11_CONSERVATIVE_RASTERIZATION_TIER_1;
  RASTERIZER_ORDERED_VIEWS = features_2.ROVsSupported == TRUE;

  if (features_2.TypedUAVLoadAdditionalFormats)
  {
    // More info about UAV format load support: https://docs.microsoft.com/en-us/windows/win32/direct3d12/typed-unordered-access-view-loads
    UAV_LOAD_FORMAT_COMMON = true;

    D3D11_FEATURE_DATA_FORMAT_SUPPORT2 FormatSupport = {};
    FormatSupport.InFormat = DXGI_FORMAT_R11G11B10_FLOAT;
    hr = device->CheckFeatureSupport(D3D11_FEATURE_FORMAT_SUPPORT2, &FormatSupport, sizeof(FormatSupport));
    if (SUCCEEDED(hr) && (FormatSupport.OutFormatSupport2 & D3D11_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
    {
      UAV_LOAD_FORMAT_R11G11B10_FLOAT = true;
    }
  }

  D3D11_FEATURE_DATA_D3D11_OPTIONS3 features_3;
  hr = device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS3, &features_3, sizeof(features_3));
  RENDERTARGET_AND_VIEWPORT_ARRAYINDEX_WITHOUT_GS = features_3.VPAndRTArrayIndexFromAnyShaderFeedingRasterizer == TRUE;

  CreateBackBufferResources();

  emptyresource = EZ_DEFAULT_NEW(EmptyResourceHandle);

  ezLog::Info("Created GraphicsDevice_DX11");
}

void GraphicsDevice_DX11::CreateBackBufferResources()
{
  HRESULT hr;

  hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
  if (FAILED(hr))
  {
    EZ_REPORT_FAILURE("BackBuffer creation Failed!");
  }

  hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
  if (FAILED(hr))
  {
    EZ_REPORT_FAILURE("Main Rendertarget creation Failed!");
  }
}

void GraphicsDevice_DX11::SetResolution(int width, int height)
{
  if ((width != RESOLUTIONWIDTH || height != RESOLUTIONHEIGHT) && width > 0 && height > 0)
  {
    RESOLUTIONWIDTH = width;
    RESOLUTIONHEIGHT = height;

    backBuffer.Reset();
    renderTargetView.Reset();

    HRESULT hr = swapChain->ResizeBuffers(GetBackBufferCount(), width, height, _ConvertFormat(GetBackBufferFormat()), 0);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to resize swapchain.");

    CreateBackBufferResources();
  }
}

Texture GraphicsDevice_DX11::GetBackBuffer()
{
  auto internal_state = EZ_DEFAULT_NEW(Texture_DX11);
  internal_state->resource = backBuffer;

  Texture result;
  result.internal_state = internal_state;
  result.type = GPUResource::GPU_RESOURCE_TYPE::TEXTURE;

  D3D11_TEXTURE2D_DESC desc;
  backBuffer->GetDesc(&desc);
  result.desc = _ConvertTextureDesc_Inv(&desc);

  return result;
}

bool GraphicsDevice_DX11::CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer)
{
  auto internal_state = EZ_DEFAULT_NEW(Resource_DX11);
  pBuffer->internal_state = internal_state;
  pBuffer->type = GPUResource::GPU_RESOURCE_TYPE::BUFFER;

  D3D11_BUFFER_DESC desc;
  desc.ByteWidth = pDesc->ByteWidth;
  desc.Usage = _ConvertUsage(pDesc->Usage);
  desc.BindFlags = _ParseBindFlags(pDesc->BindFlags);
  desc.CPUAccessFlags = _ParseCPUAccessFlags(pDesc->CPUAccessFlags);
  desc.MiscFlags = _ParseResourceMiscFlags(pDesc->MiscFlags);
  desc.StructureByteStride = pDesc->StructureByteStride;

  D3D11_SUBRESOURCE_DATA data;
  if (pInitialData != nullptr)
  {
    data = _ConvertSubresourceData(*pInitialData);
  }

  pBuffer->desc = *pDesc;
  HRESULT hr = device->CreateBuffer(&desc, pInitialData == nullptr ? nullptr : &data, (ID3D11Buffer**)internal_state->resource.ReleaseAndGetAddressOf());
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "GPUBuffer creation failed!");

  if (SUCCEEDED(hr))
  {
    // Create resource views if needed
    if (pDesc->BindFlags & BIND_SHADER_RESOURCE)
    {
      CreateSubresource(pBuffer, SRV, 0);
    }
    if (pDesc->BindFlags & BIND_UNORDERED_ACCESS)
    {
      CreateSubresource(pBuffer, UAV, 0);
    }
  }

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX11::CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, Texture* pTexture)
{
  auto internal_state = EZ_DEFAULT_NEW(Texture_DX11);
  pTexture->internal_state = internal_state;
  pTexture->type = GPUResource::GPU_RESOURCE_TYPE::TEXTURE;

  pTexture->desc = *pDesc;

  ezDynamicArray<D3D11_SUBRESOURCE_DATA> data;
  if (pInitialData != nullptr)
  {
    ezUInt32 dataCount = pDesc->ArraySize * ezMath::Max(1u, pDesc->MipLevels);
    data.SetCount(dataCount);
    for (ezUInt32 slice = 0; slice < dataCount; ++slice)
    {
      data[slice] = _ConvertSubresourceData(pInitialData[slice]);
    }
  }

  HRESULT hr = S_OK;

  switch (pTexture->desc.type)
  {
    case TextureDesc::TEXTURE_1D:
    {
      D3D11_TEXTURE1D_DESC desc = _ConvertTextureDesc1D(&pTexture->desc);
      hr = device->CreateTexture1D(&desc, data.GetData(), (ID3D11Texture1D**)internal_state->resource.ReleaseAndGetAddressOf());
    }
    break;
    case TextureDesc::TEXTURE_2D:
    {
      D3D11_TEXTURE2D_DESC desc = _ConvertTextureDesc2D(&pTexture->desc);
      hr = device->CreateTexture2D(&desc, data.GetData(), (ID3D11Texture2D**)internal_state->resource.ReleaseAndGetAddressOf());
    }
    break;
    case TextureDesc::TEXTURE_3D:
    {
      D3D11_TEXTURE3D_DESC desc = _ConvertTextureDesc3D(&pTexture->desc);
      hr = device->CreateTexture3D(&desc, data.GetData(), (ID3D11Texture3D**)internal_state->resource.ReleaseAndGetAddressOf());
    }
    break;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      break;
  }

  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to create texture.");
  if (FAILED(hr)) //wtf?
    return SUCCEEDED(hr);

  if (pTexture->desc.MipLevels == 0)
  {
    pTexture->desc.MipLevels = (ezUInt32)log2(ezMath::Max(pTexture->desc.Width, pTexture->desc.Height)) + 1;
  }

  if (pTexture->desc.BindFlags & BIND_RENDER_TARGET)
  {
    CreateSubresource(pTexture, RTV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_DEPTH_STENCIL)
  {
    CreateSubresource(pTexture, DSV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_SHADER_RESOURCE)
  {
    CreateSubresource(pTexture, SRV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_UNORDERED_ACCESS)
  {
    CreateSubresource(pTexture, UAV, 0, -1, 0, -1);
  }

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX11::CreateInputLayout(const InputLayoutDesc* pInputElementDescs, ezUInt32 NumElements, const Shader* shader, InputLayout* pInputLayout)
{
  auto internal_state = EZ_DEFAULT_NEW(InputLayout_DX11);
  pInputLayout->internal_state = internal_state;

  pInputLayout->desc.Reserve(NumElements);

  ezDynamicArray<D3D11_INPUT_ELEMENT_DESC> desc;
  desc.SetCount(NumElements);
  for (ezUInt32 i = 0; i < NumElements; ++i)
  {
    desc[i].SemanticName = pInputElementDescs[i].SemanticName.GetData();
    desc[i].SemanticIndex = pInputElementDescs[i].SemanticIndex;
    desc[i].Format = _ConvertFormat(pInputElementDescs[i].Format);
    desc[i].InputSlot = pInputElementDescs[i].InputSlot;
    desc[i].AlignedByteOffset = pInputElementDescs[i].AlignedByteOffset;
    if (desc[i].AlignedByteOffset == InputLayoutDesc::APPEND_ALIGNED_ELEMENT)
      desc[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    desc[i].InputSlotClass = _ConvertInputClassification(pInputElementDescs[i].InputSlotClass);
    desc[i].InstanceDataStepRate = pInputElementDescs[i].InstanceDataStepRate;

    pInputLayout->desc.PushBack(pInputElementDescs[i]);
  }

  HRESULT hr = device->CreateInputLayout(desc.GetData(), NumElements, shader->code.GetData(), shader->code.GetCount(), &internal_state->resource);

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX11::CreateShader(ezEnum<ezRHIShaderStage> stage, const void* pShaderBytecode, size_t BytecodeLength, Shader* pShader)
{
  pShader->code.SetCount(BytecodeLength);
  //std::memcpy(pShader->code.GetData(), pShaderBytecode, BytecodeLength);
  ezMemoryUtils::Copy(pShader->code.GetData(), const_cast<ezUInt8*>(reinterpret_cast<const ezUInt8*>(pShaderBytecode)), BytecodeLength);
  pShader->stage = stage;

  HRESULT hr = E_FAIL;

  switch (stage)
  {
    case ezRHIShaderStage::VertexShader:
    {
      auto internal_state = EZ_DEFAULT_NEW(VertexShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreateVertexShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
    case ezRHIShaderStage::HullShader:
    {
      auto internal_state = EZ_DEFAULT_NEW(HullShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreateHullShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
    case ezRHIShaderStage::DomainShader:
    {
      auto internal_state = EZ_DEFAULT_NEW(DomainShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreateDomainShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
    case ezRHIShaderStage::GeometryShader:
    {
      auto internal_state = EZ_DEFAULT_NEW(GeometryShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreateGeometryShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
    case ezRHIShaderStage::PixelShader:
    {
      auto internal_state = EZ_DEFAULT_NEW(PixelShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreatePixelShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
    case ezRHIShaderStage::ComputeShader:
    {
      auto internal_state = EZ_DEFAULT_NEW(ComputeShader_DX11);
      pShader->internal_state = internal_state;
      hr = device->CreateComputeShader(pShaderBytecode, BytecodeLength, nullptr, &internal_state->resource);
    }
    break;
  }

  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to create shader.");

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX11::CreateBlendState(const BlendStateDesc* pBlendStateDesc, BlendState* pBlendState)
{
  auto internal_state = EZ_DEFAULT_NEW(BlendState_DX11);
  pBlendState->internal_state = internal_state;

  D3D11_BLEND_DESC desc;
  desc.AlphaToCoverageEnable = pBlendStateDesc->AlphaToCoverageEnable;
  desc.IndependentBlendEnable = pBlendStateDesc->IndependentBlendEnable;
  for (int i = 0; i < 8; ++i)
  {
    desc.RenderTarget[i].BlendEnable = pBlendStateDesc->RenderTarget[i].BlendEnable;
    desc.RenderTarget[i].SrcBlend = _ConvertBlend(pBlendStateDesc->RenderTarget[i].SrcBlend);
    desc.RenderTarget[i].DestBlend = _ConvertBlend(pBlendStateDesc->RenderTarget[i].DestBlend);
    desc.RenderTarget[i].BlendOp = _ConvertBlendOp(pBlendStateDesc->RenderTarget[i].BlendOp);
    desc.RenderTarget[i].SrcBlendAlpha = _ConvertBlend(pBlendStateDesc->RenderTarget[i].SrcBlendAlpha);
    desc.RenderTarget[i].DestBlendAlpha = _ConvertBlend(pBlendStateDesc->RenderTarget[i].DestBlendAlpha);
    desc.RenderTarget[i].BlendOpAlpha = _ConvertBlendOp(pBlendStateDesc->RenderTarget[i].BlendOpAlpha);
    desc.RenderTarget[i].RenderTargetWriteMask = (UINT8)_ParseColorWriteMask(pBlendStateDesc->RenderTarget[i].RenderTargetWriteMask);
  }

  pBlendState->desc = *pBlendStateDesc;
  HRESULT hr = device->CreateBlendState(&desc, &internal_state->resource);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to crate blend state.");

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX11::CreateDepthStencilState(const DepthStencilStateDesc* pDepthStencilStateDesc, DepthStencilState* pDepthStencilState)
{
  auto internal_state = EZ_DEFAULT_NEW(DepthStencilState_DX11);
  pDepthStencilState->internal_state = internal_state;

  D3D11_DEPTH_STENCIL_DESC desc;
  desc.DepthEnable = pDepthStencilStateDesc->DepthEnable;
  desc.DepthWriteMask = _ConvertDepthWriteMask(pDepthStencilStateDesc->DepthWriteMask);
  desc.DepthFunc = _ConvertComparisonFunc(pDepthStencilStateDesc->DepthFunc);
  desc.StencilEnable = pDepthStencilStateDesc->StencilEnable;
  desc.StencilReadMask = pDepthStencilStateDesc->StencilReadMask;
  desc.StencilWriteMask = pDepthStencilStateDesc->StencilWriteMask;
  desc.FrontFace.StencilDepthFailOp = _ConvertStencilOp(pDepthStencilStateDesc->FrontFace.StencilDepthFailOp);
  desc.FrontFace.StencilFailOp = _ConvertStencilOp(pDepthStencilStateDesc->FrontFace.StencilFailOp);
  desc.FrontFace.StencilFunc = _ConvertComparisonFunc(pDepthStencilStateDesc->FrontFace.StencilFunc);
  desc.FrontFace.StencilPassOp = _ConvertStencilOp(pDepthStencilStateDesc->FrontFace.StencilPassOp);
  desc.BackFace.StencilDepthFailOp = _ConvertStencilOp(pDepthStencilStateDesc->BackFace.StencilDepthFailOp);
  desc.BackFace.StencilFailOp = _ConvertStencilOp(pDepthStencilStateDesc->BackFace.StencilFailOp);
  desc.BackFace.StencilFunc = _ConvertComparisonFunc(pDepthStencilStateDesc->BackFace.StencilFunc);
  desc.BackFace.StencilPassOp = _ConvertStencilOp(pDepthStencilStateDesc->BackFace.StencilPassOp);

  pDepthStencilState->desc = *pDepthStencilStateDesc;
  HRESULT hr = device->CreateDepthStencilState(&desc, &internal_state->resource);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to create depth stencil state.");

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX11::CreateRasterizerState(const RasterizerStateDesc* pRasterizerStateDesc, RasterizerState* pRasterizerState)
{
  auto internal_state = EZ_DEFAULT_NEW(RasterizerState_DX11);
  pRasterizerState->internal_state = internal_state;

  pRasterizerState->desc = *pRasterizerStateDesc;

  D3D11_RASTERIZER_DESC desc;
  desc.FillMode = _ConvertFillMode(pRasterizerStateDesc->FillMode);
  desc.CullMode = _ConvertCullMode(pRasterizerStateDesc->CullMode);
  desc.FrontCounterClockwise = pRasterizerStateDesc->FrontCounterClockwise;
  desc.DepthBias = pRasterizerStateDesc->DepthBias;
  desc.DepthBiasClamp = pRasterizerStateDesc->DepthBiasClamp;
  desc.SlopeScaledDepthBias = pRasterizerStateDesc->SlopeScaledDepthBias;
  desc.DepthClipEnable = pRasterizerStateDesc->DepthClipEnable;
  desc.ScissorEnable = true;
  desc.MultisampleEnable = pRasterizerStateDesc->MultisampleEnable;
  desc.AntialiasedLineEnable = pRasterizerStateDesc->AntialiasedLineEnable;


  if (CONSERVATIVE_RASTERIZATION && pRasterizerStateDesc->ConservativeRasterizationEnable == TRUE)
  {
    ComPtr<ID3D11Device3> device3;
    if (SUCCEEDED(device.As(&device3)))
    {
      D3D11_RASTERIZER_DESC2 desc2;
      desc2.FillMode = desc.FillMode;
      desc2.CullMode = desc.CullMode;
      desc2.FrontCounterClockwise = desc.FrontCounterClockwise;
      desc2.DepthBias = desc.DepthBias;
      desc2.DepthBiasClamp = desc.DepthBiasClamp;
      desc2.SlopeScaledDepthBias = desc.SlopeScaledDepthBias;
      desc2.DepthClipEnable = desc.DepthClipEnable;
      desc2.ScissorEnable = desc.ScissorEnable;
      desc2.MultisampleEnable = desc.MultisampleEnable;
      desc2.AntialiasedLineEnable = desc.AntialiasedLineEnable;
      desc2.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_ON;
      desc2.ForcedSampleCount = (RASTERIZER_ORDERED_VIEWS ? pRasterizerStateDesc->ForcedSampleCount : 0);

      pRasterizerState->desc = *pRasterizerStateDesc;

      ComPtr<ID3D11RasterizerState2> rasterizer2;
      HRESULT hr = device3->CreateRasterizerState2(&desc2, &rasterizer2);
      EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to create rasterizer state.");

      internal_state->resource = rasterizer2;

      return SUCCEEDED(hr);
    }
  }
  else if (RASTERIZER_ORDERED_VIEWS && pRasterizerStateDesc->ForcedSampleCount > 0)
  {
    ComPtr<ID3D11Device1> device1;
    if (SUCCEEDED(device.As(&device1)))
    {
      D3D11_RASTERIZER_DESC1 desc1;
      desc1.FillMode = desc.FillMode;
      desc1.CullMode = desc.CullMode;
      desc1.FrontCounterClockwise = desc.FrontCounterClockwise;
      desc1.DepthBias = desc.DepthBias;
      desc1.DepthBiasClamp = desc.DepthBiasClamp;
      desc1.SlopeScaledDepthBias = desc.SlopeScaledDepthBias;
      desc1.DepthClipEnable = desc.DepthClipEnable;
      desc1.ScissorEnable = desc.ScissorEnable;
      desc1.MultisampleEnable = desc.MultisampleEnable;
      desc1.AntialiasedLineEnable = desc.AntialiasedLineEnable;
      desc1.ForcedSampleCount = pRasterizerStateDesc->ForcedSampleCount;

      pRasterizerState->desc = *pRasterizerStateDesc;

      ComPtr<ID3D11RasterizerState1> rasterizer1;
      HRESULT hr = device1->CreateRasterizerState1(&desc1, &rasterizer1);
      EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to create rasterizer state.");

      internal_state->resource = rasterizer1;

      return SUCCEEDED(hr);
    }
  }

  HRESULT hr = device->CreateRasterizerState(&desc, &internal_state->resource);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to create rasterizer state.");

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX11::CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState)
{
  auto internal_state = EZ_DEFAULT_NEW(Sampler_DX11);
  pSamplerState->internal_state = internal_state;

  D3D11_SAMPLER_DESC desc;
  desc.Filter = _ConvertFilter(pSamplerDesc->Filter);
  desc.AddressU = _ConvertTextureAddressMode(pSamplerDesc->AddressU);
  desc.AddressV = _ConvertTextureAddressMode(pSamplerDesc->AddressV);
  desc.AddressW = _ConvertTextureAddressMode(pSamplerDesc->AddressW);
  desc.MipLODBias = pSamplerDesc->MipLODBias;
  desc.MaxAnisotropy = pSamplerDesc->MaxAnisotropy;
  desc.ComparisonFunc = _ConvertComparisonFunc(pSamplerDesc->ComparisonFunc);
  desc.BorderColor[0] = pSamplerDesc->BorderColor[0];
  desc.BorderColor[1] = pSamplerDesc->BorderColor[1];
  desc.BorderColor[2] = pSamplerDesc->BorderColor[2];
  desc.BorderColor[3] = pSamplerDesc->BorderColor[3];
  desc.MinLOD = pSamplerDesc->MinLOD;
  desc.MaxLOD = pSamplerDesc->MaxLOD;

  pSamplerState->desc = *pSamplerDesc;
  HRESULT hr = device->CreateSamplerState(&desc, &internal_state->resource);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to create sampler state.");

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX11::CreateQuery(const GPUQueryDesc* pDesc, GPUQuery* pQuery)
{
  auto internal_state = EZ_DEFAULT_NEW(Query_DX11);
  pQuery->internal_state = internal_state;

  pQuery->desc = *pDesc;

  D3D11_QUERY_DESC desc;
  desc.MiscFlags = 0;
  desc.Query = D3D11_QUERY_EVENT;
  if (pDesc->Type == ezRHIGPUQueryType::GPU_QUERY_TYPE_EVENT)
  {
    desc.Query = D3D11_QUERY_EVENT;
  }
  else if (pDesc->Type == ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION)
  {
    desc.Query = D3D11_QUERY_OCCLUSION;
  }
  else if (pDesc->Type == ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION_PREDICATE)
  {
    desc.Query = D3D11_QUERY_OCCLUSION_PREDICATE;
  }
  else if (pDesc->Type == ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP)
  {
    desc.Query = D3D11_QUERY_TIMESTAMP;
  }
  else if (pDesc->Type == ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP_DISJOINT)
  {
    desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
  }

  HRESULT hr = device->CreateQuery(&desc, &internal_state->resource);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to create query.");

  return SUCCEEDED(hr);
}
bool GraphicsDevice_DX11::CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso)
{
  pso->internal_state = emptyresource;

  pso->desc = *pDesc;

  return true;
}
bool GraphicsDevice_DX11::CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass)
{
  renderpass->internal_state = emptyresource;

  renderpass->desc = *pDesc;

  return true;
}

int GraphicsDevice_DX11::CreateSubresource(Texture* texture, SUBRESOURCE_TYPE type, ezUInt32 firstSlice, ezUInt32 sliceCount, ezUInt32 firstMip, ezUInt32 mipCount)
{
  auto internal_state = to_internal(texture);

  switch (type)
  {
    case SRV:
    {
      D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case ezRHIFormat::R16_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R16_UNORM;
          break;
        case ezRHIFormat::R32_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
          break;
        case ezRHIFormat::R24G8_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          break;
        case ezRHIFormat::R32G8X24_TYPELESS:
          srv_desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
          break;
        default:
          srv_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
          srv_desc.Texture1DArray.FirstArraySlice = firstSlice;
          srv_desc.Texture1DArray.ArraySize = sliceCount;
          srv_desc.Texture1DArray.MostDetailedMip = firstMip;
          srv_desc.Texture1DArray.MipLevels = mipCount;
        }
        else
        {
          srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
          srv_desc.Texture1D.MostDetailedMip = firstMip;
          srv_desc.Texture1D.MipLevels = mipCount;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          if (texture->desc.MiscFlags & RESOURCE_MISC_TEXTURECUBE)
          {
            if (texture->desc.ArraySize > 6)
            {
              srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
              srv_desc.TextureCubeArray.First2DArrayFace = firstSlice;
              srv_desc.TextureCubeArray.NumCubes = ezMath::Min(texture->desc.ArraySize, sliceCount) / 6;
              srv_desc.TextureCubeArray.MostDetailedMip = firstMip;
              srv_desc.TextureCubeArray.MipLevels = mipCount;
            }
            else
            {
              srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
              srv_desc.TextureCube.MostDetailedMip = firstMip;
              srv_desc.TextureCube.MipLevels = mipCount;
            }
          }
          else
          {
            if (texture->desc.SampleCount > 1)
            {
              srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
              srv_desc.Texture2DMSArray.FirstArraySlice = firstSlice;
              srv_desc.Texture2DMSArray.ArraySize = sliceCount;
            }
            else
            {
              srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
              srv_desc.Texture2DArray.FirstArraySlice = firstSlice;
              srv_desc.Texture2DArray.ArraySize = sliceCount;
              srv_desc.Texture2DArray.MostDetailedMip = firstMip;
              srv_desc.Texture2DArray.MipLevels = mipCount;
            }
          }
        }
        else
        {
          if (texture->desc.SampleCount > 1)
          {
            srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
          }
          else
          {
            srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srv_desc.Texture2D.MostDetailedMip = firstMip;
            srv_desc.Texture2D.MipLevels = mipCount;
          }
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_3D)
      {
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        srv_desc.Texture3D.MostDetailedMip = firstMip;
        srv_desc.Texture3D.MipLevels = mipCount;
      }

      ComPtr<ID3D11ShaderResourceView> srv;
      HRESULT hr = device->CreateShaderResourceView(internal_state->resource.Get(), &srv_desc, &srv);
      if (SUCCEEDED(hr))
      {
        if (!internal_state->srv)
        {
          internal_state->srv = srv;
          return -1;
        }
        internal_state->subresources_srv.PushBack(srv);
        return int(internal_state->subresources_srv.GetCount() - 1);
      }
      else
      {
        EZ_REPORT_FAILURE("Failed to create shader resource view.");
      }
    }
    break;
    case UAV:
    {
      D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case ezRHIFormat::R16_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R16_UNORM;
          break;
        case ezRHIFormat::R32_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R32_FLOAT;
          break;
        case ezRHIFormat::R24G8_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          break;
        case ezRHIFormat::R32G8X24_TYPELESS:
          uav_desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
          break;
        default:
          uav_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
          uav_desc.Texture1DArray.FirstArraySlice = firstSlice;
          uav_desc.Texture1DArray.ArraySize = sliceCount;
          uav_desc.Texture1DArray.MipSlice = firstMip;
        }
        else
        {
          uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
          uav_desc.Texture1D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
          uav_desc.Texture2DArray.FirstArraySlice = firstSlice;
          uav_desc.Texture2DArray.ArraySize = sliceCount;
          uav_desc.Texture2DArray.MipSlice = firstMip;
        }
        else
        {
          uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
          uav_desc.Texture2D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_3D)
      {
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
        uav_desc.Texture3D.MipSlice = firstMip;
        uav_desc.Texture3D.FirstWSlice = 0;
        uav_desc.Texture3D.WSize = -1;
      }

      ComPtr<ID3D11UnorderedAccessView> uav;
      HRESULT hr = device->CreateUnorderedAccessView(internal_state->resource.Get(), &uav_desc, &uav);
      if (SUCCEEDED(hr))
      {
        if (!internal_state->uav)
        {
          internal_state->uav = uav;
          return -1;
        }
        internal_state->subresources_uav.PushBack(uav);
        return int(internal_state->subresources_uav.GetCount() - 1);
      }
      else
      {
        EZ_REPORT_FAILURE("Failed to create unordered access view.");
      }
    }
    break;
    case RTV:
    {
      D3D11_RENDER_TARGET_VIEW_DESC rtv_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case ezRHIFormat::R16_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R16_UNORM;
          break;
        case ezRHIFormat::R32_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R32_FLOAT;
          break;
        case ezRHIFormat::R24G8_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
          break;
        case ezRHIFormat::R32G8X24_TYPELESS:
          rtv_desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
          break;
        default:
          rtv_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
          rtv_desc.Texture1DArray.FirstArraySlice = firstSlice;
          rtv_desc.Texture1DArray.ArraySize = sliceCount;
          rtv_desc.Texture1DArray.MipSlice = firstMip;
        }
        else
        {
          rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
          rtv_desc.Texture1D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          if (texture->desc.SampleCount > 1)
          {
            rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
            rtv_desc.Texture2DMSArray.FirstArraySlice = firstSlice;
            rtv_desc.Texture2DMSArray.ArraySize = sliceCount;
          }
          else
          {
            rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtv_desc.Texture2DArray.FirstArraySlice = firstSlice;
            rtv_desc.Texture2DArray.ArraySize = sliceCount;
            rtv_desc.Texture2DArray.MipSlice = firstMip;
          }
        }
        else
        {
          if (texture->desc.SampleCount > 1)
          {
            rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
          }
          else
          {
            rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            rtv_desc.Texture2D.MipSlice = firstMip;
          }
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_3D)
      {
        rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
        rtv_desc.Texture3D.MipSlice = firstMip;
        rtv_desc.Texture3D.FirstWSlice = 0;
        rtv_desc.Texture3D.WSize = -1;
      }

      ComPtr<ID3D11RenderTargetView> rtv;
      HRESULT hr = device->CreateRenderTargetView(internal_state->resource.Get(), &rtv_desc, &rtv);
      if (SUCCEEDED(hr))
      {
        if (!internal_state->rtv)
        {
          internal_state->rtv = rtv;
          return -1;
        }
        internal_state->subresources_rtv.PushBack(rtv);
        return int(internal_state->subresources_rtv.GetCount() - 1);
      }
      else
      {
        EZ_REPORT_FAILURE("Failed to create render target view.");
      }
    }
    break;
    case DSV:
    {
      D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};

      // Try to resolve resource format:
      switch (texture->desc.Format)
      {
        case ezRHIFormat::R16_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D16_UNORM;
          break;
        case ezRHIFormat::R32_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
          break;
        case ezRHIFormat::R24G8_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
          break;
        case ezRHIFormat::R32G8X24_TYPELESS:
          dsv_desc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
          break;
        default:
          dsv_desc.Format = _ConvertFormat(texture->desc.Format);
          break;
      }

      if (texture->desc.type == TextureDesc::TEXTURE_1D)
      {
        if (texture->desc.ArraySize > 1)
        {
          dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
          dsv_desc.Texture1DArray.FirstArraySlice = firstSlice;
          dsv_desc.Texture1DArray.ArraySize = sliceCount;
          dsv_desc.Texture1DArray.MipSlice = firstMip;
        }
        else
        {
          dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
          dsv_desc.Texture1D.MipSlice = firstMip;
        }
      }
      else if (texture->desc.type == TextureDesc::TEXTURE_2D)
      {
        if (texture->desc.ArraySize > 1)
        {
          if (texture->desc.SampleCount > 1)
          {
            dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
            dsv_desc.Texture2DMSArray.FirstArraySlice = firstSlice;
            dsv_desc.Texture2DMSArray.ArraySize = sliceCount;
          }
          else
          {
            dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            dsv_desc.Texture2DArray.FirstArraySlice = firstSlice;
            dsv_desc.Texture2DArray.ArraySize = sliceCount;
            dsv_desc.Texture2DArray.MipSlice = firstMip;
          }
        }
        else
        {
          if (texture->desc.SampleCount > 1)
          {
            dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
          }
          else
          {
            dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            dsv_desc.Texture2D.MipSlice = firstMip;
          }
        }
      }

      ComPtr<ID3D11DepthStencilView> dsv;
      HRESULT hr = device->CreateDepthStencilView(internal_state->resource.Get(), &dsv_desc, &dsv);
      if (SUCCEEDED(hr))
      {
        if (!internal_state->dsv)
        {
          internal_state->dsv = dsv;
          return -1;
        }
        internal_state->subresources_dsv.PushBack(dsv);
        return int(internal_state->subresources_dsv.GetCount() - 1);
      }
      else
      {
        EZ_REPORT_FAILURE("Failed to create depth stencil view.");
      }
    }
    break;
    default:
      break;
  }
  return -1;
}
int GraphicsDevice_DX11::CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, ezUInt64 offset, ezUInt64 size)
{
  auto internal_state = to_internal(buffer);
  const GPUBufferDesc& desc = buffer->GetDesc();
  HRESULT hr = E_FAIL;

  switch (type)
  {
    case SRV:
    {
      D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

      if (desc.MiscFlags & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
      {
        // This is a Raw Buffer
        srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        srv_desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
        srv_desc.BufferEx.FirstElement = (UINT)offset / sizeof(ezUInt32);
        srv_desc.BufferEx.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / sizeof(ezUInt32);
      }
      else if (desc.MiscFlags & RESOURCE_MISC_BUFFER_STRUCTURED)
      {
        // This is a Structured Buffer
        srv_desc.Format = DXGI_FORMAT_UNKNOWN;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        srv_desc.BufferEx.FirstElement = (UINT)offset / desc.StructureByteStride;
        srv_desc.BufferEx.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / desc.StructureByteStride;
      }
      else
      {
        // This is a Typed Buffer
        ezUInt32 stride = FormatHelpers::GetFormatStride(desc.Format);
        srv_desc.Format = _ConvertFormat(desc.Format);
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srv_desc.Buffer.FirstElement = (UINT)offset / stride;
        srv_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / stride;
      }

      ComPtr<ID3D11ShaderResourceView> srv;
      hr = device->CreateShaderResourceView(internal_state->resource.Get(), &srv_desc, &srv);

      if (SUCCEEDED(hr))
      {
        if (internal_state->srv == nullptr)
        {
          internal_state->srv = srv;
          return -1;
        }
        else
        {
          internal_state->subresources_srv.PushBack(srv);
          return int(internal_state->subresources_srv.GetCount() - 1);
        }
      }
      else
      {
        EZ_REPORT_FAILURE("Failed to create shader resource view.");
      }
    }
    break;
    case UAV:
    {
      D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
      uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

      if (desc.MiscFlags & RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
      {
        // This is a Raw Buffer
        uav_desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
        uav_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        uav_desc.Buffer.FirstElement = (UINT)offset / sizeof(ezUInt32);
        uav_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / sizeof(ezUInt32);
      }
      else if (desc.MiscFlags & RESOURCE_MISC_BUFFER_STRUCTURED)
      {
        // This is a Structured Buffer
        uav_desc.Format = DXGI_FORMAT_UNKNOWN; // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
        uav_desc.Buffer.FirstElement = (UINT)offset / desc.StructureByteStride;
        uav_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / desc.StructureByteStride;
      }
      else
      {
        // This is a Typed Buffer
        ezUInt32 stride = FormatHelpers::GetFormatStride(desc.Format);
        uav_desc.Format = _ConvertFormat(desc.Format);
        uav_desc.Buffer.FirstElement = (UINT)offset / stride;
        uav_desc.Buffer.NumElements = ezMath::Min((UINT)size, desc.ByteWidth - (UINT)offset) / stride;
      }

      ComPtr<ID3D11UnorderedAccessView> uav;
      hr = device->CreateUnorderedAccessView(internal_state->resource.Get(), &uav_desc, &uav);

      if (SUCCEEDED(hr))
      {
        if (internal_state->uav == nullptr)
        {
          internal_state->uav = uav;
          return -1;
        }
        else
        {
          internal_state->subresources_uav.PushBack(uav);
          return int(internal_state->subresources_uav.GetCount() - 1);
        }
      }
      else
      {
        EZ_REPORT_FAILURE("Failed to create unordered access view.");
      }
    }
    break;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      break;
  }
  return -1;
}

void GraphicsDevice_DX11::Map(const GPUResource* resource, Mapping* mapping)
{
  auto internal_state = to_internal(resource);

  D3D11_MAPPED_SUBRESOURCE map_result = {};
  D3D11_MAP map_type = D3D11_MAP_READ_WRITE;
  if (mapping->_flags & Mapping::FLAG_READ)
  {
    if (mapping->_flags & Mapping::FLAG_WRITE)
    {
      map_type = D3D11_MAP_READ_WRITE;
    }
    else
    {
      map_type = D3D11_MAP_READ;
    }
  }
  else if (mapping->_flags & Mapping::FLAG_WRITE)
  {
    map_type = D3D11_MAP_WRITE_NO_OVERWRITE;
  }
  HRESULT hr = immediateContext->Map(internal_state->resource.Get(), 0, map_type, D3D11_MAP_FLAG_DO_NOT_WAIT, &map_result);
  if (SUCCEEDED(hr))
  {
    mapping->data = map_result.pData;
    mapping->rowpitch = map_result.RowPitch;
  }
  else
  {
    EZ_REPORT_FAILURE("Failed to map resource.");
    mapping->data = nullptr;
    mapping->rowpitch = 0;
  }
}
void GraphicsDevice_DX11::Unmap(const GPUResource* resource)
{
  auto internal_state = to_internal(resource);
  immediateContext->Unmap(internal_state->resource.Get(), 0);
}
bool GraphicsDevice_DX11::QueryRead(const GPUQuery* query, GPUQueryResult* result)
{
  const ezUInt32 _flags = D3D11_ASYNC_GETDATA_DONOTFLUSH;

  auto internal_state = to_internal(query);
  ID3D11Query* QUERY = internal_state->resource.Get();

  HRESULT hr = S_OK;
  switch (query->desc.Type)
  {
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP:
      hr = immediateContext->GetData(QUERY, &result->result_timestamp, sizeof(ezUInt64), _flags);
      break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP_DISJOINT:
    {
      D3D11_QUERY_DATA_TIMESTAMP_DISJOINT _temp;
      hr = immediateContext->GetData(QUERY, &_temp, sizeof(_temp), _flags);
      result->result_timestamp_frequency = _temp.Frequency;
    }
    break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_EVENT:
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION:
      hr = immediateContext->GetData(QUERY, &result->result_passed_sample_count, sizeof(ezUInt64), _flags);
      break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
    {
      BOOL passed = FALSE;
      hr = immediateContext->GetData(QUERY, &passed, sizeof(BOOL), _flags);
      result->result_passed_sample_count = (ezUInt64)passed;
      break;
    }
  }

  return hr != S_FALSE;
}

void GraphicsDevice_DX11::SetName(GPUResource* pResource, const char* name)
{
  auto internal_state = to_internal(pResource);
  internal_state->resource->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
}

void GraphicsDevice_DX11::PresentBegin(CommandList cmd)
{
  ID3D11RenderTargetView* RTV = renderTargetView.Get();
  deviceContexts[cmd]->OMSetRenderTargets(1, &RTV, 0);
  float ClearColor[4] = {0, 0, 0, 1.0f}; // red,green,blue,alpha
  deviceContexts[cmd]->ClearRenderTargetView(RTV, ClearColor);
}
void GraphicsDevice_DX11::PresentEnd(CommandList cmd)
{
  SubmitCommandLists();

  swapChain->Present(VSYNC, 0);
}


CommandList GraphicsDevice_DX11::BeginCommandList()
{
  CommandList cmd = cmd_count.fetch_add(1);
  if (deviceContexts[cmd] == nullptr)
  {
    // need to create one more command list:
    EZ_ASSERT_ALWAYS(cmd < COMMANDLIST_COUNT, "Need to create one more command list.");

    HRESULT hr = device->CreateDeferredContext(0, &deviceContexts[cmd]);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to create deferred context.");

    hr = deviceContexts[cmd].As(&userDefinedAnnotations[cmd]);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "Failed to get ID3DUserDefinedAnnotation.");

    // Temporary allocations will use the following buffer type:
    GPUBufferDesc frameAllocatorDesc;
    frameAllocatorDesc.ByteWidth = 1024 * 1024; // 1 MB starting size
    frameAllocatorDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_INDEX_BUFFER | BIND_VERTEX_BUFFER;
    frameAllocatorDesc.Usage = ezRHIUsage::Dynamic;
    frameAllocatorDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
    frameAllocatorDesc.MiscFlags = RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    bool success = CreateBuffer(&frameAllocatorDesc, nullptr, &frame_allocators[cmd].buffer);
    EZ_ASSERT_ALWAYS(success, "Failed to create buffer.");
    SetName(&frame_allocators[cmd].buffer, "frame_allocator");
  }

  BindPipelineState(nullptr, cmd);
  BindComputeShader(nullptr, cmd);

  D3D11_VIEWPORT vp = {};
  vp.Width = (float)RESOLUTIONWIDTH;
  vp.Height = (float)RESOLUTIONHEIGHT;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  deviceContexts[cmd]->RSSetViewports(1, &vp);

  D3D11_RECT pRects[8];
  for (ezUInt32 i = 0; i < 8; ++i)
  {
    pRects[i].bottom = INT32_MAX;
    pRects[i].left = INT32_MIN;
    pRects[i].right = INT32_MAX;
    pRects[i].top = INT32_MIN;
  }
  deviceContexts[cmd]->RSSetScissorRects(8, pRects);

  stencilRef[cmd] = 0;
  blendFactor[cmd] = ezVec4(1, 1, 1, 1);

  prev_vs[cmd] = {};
  prev_ps[cmd] = {};
  prev_hs[cmd] = {};
  prev_ds[cmd] = {};
  prev_gs[cmd] = {};
  prev_cs[cmd] = {};
  prev_blendfactor[cmd] = {};
  prev_samplemask[cmd] = {};
  prev_bs[cmd] = {};
  prev_rs[cmd] = {};
  prev_stencilRef[cmd] = {};
  prev_dss[cmd] = {};
  prev_il[cmd] = {};
  prev_pt[cmd] = {};

  memset(raster_uavs[cmd], 0, sizeof(raster_uavs[cmd]));
  raster_uavs_slot[cmd] = {};
  raster_uavs_count[cmd] = {};

  active_pso[cmd] = nullptr;
  dirty_pso[cmd] = false;
  active_renderpass[cmd] = nullptr;

  return cmd;
}
void GraphicsDevice_DX11::SubmitCommandLists()
{
  // Execute deferred command lists:
  {
    CommandList cmd_last = cmd_count.load();
    cmd_count.store(0);
    for (CommandList cmd = 0; cmd < cmd_last; ++cmd)
    {
      deviceContexts[cmd]->FinishCommandList(false, &commandLists[cmd]);
      immediateContext->ExecuteCommandList(commandLists[cmd].Get(), false);
      commandLists[cmd].Reset();
    }
  }
  immediateContext->ClearState();

  FRAMECOUNT++;
}

void GraphicsDevice_DX11::WaitForGPU()
{
  immediateContext->Flush();

  GPUQuery query;
  GPUQueryDesc desc;
  desc.Type = ezRHIGPUQueryType::GPU_QUERY_TYPE_EVENT;
  bool success = CreateQuery(&desc, &query);
  EZ_ASSERT_ALWAYS(success, "Failed to create query.");
  auto internal_state = to_internal(&query);
  immediateContext->End(internal_state->resource.Get());
  BOOL result;
  while (immediateContext->GetData(internal_state->resource.Get(), &result, sizeof(result), 0) == S_FALSE)
    ;
  EZ_ASSERT_ALWAYS(result == TRUE, "result must be true at this point.");
}


void GraphicsDevice_DX11::commit_allocations(CommandList cmd)
{
  // DX11 needs to unmap allocations before it can execute safely

  if (frame_allocators[cmd].dirty)
  {
    auto internal_state = to_internal(&frame_allocators[cmd].buffer);
    deviceContexts[cmd]->Unmap(internal_state->resource.Get(), 0);
    frame_allocators[cmd].dirty = false;
  }
}


void GraphicsDevice_DX11::RenderPassBegin(const RenderPass* renderpass, CommandList cmd)
{
  active_renderpass[cmd] = renderpass;
  const RenderPassDesc& desc = renderpass->GetDesc();

  ezUInt32 rt_count = 0;
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
        deviceContexts[cmd]->ClearRenderTargetView(RTVs[rt_count], texture->desc.clear.color);
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
        ezUInt32 _flags = D3D11_CLEAR_DEPTH;
        if (FormatHelpers::IsFormatStencilSupport(texture->desc.Format))
          _flags |= D3D11_CLEAR_STENCIL;
        deviceContexts[cmd]->ClearDepthStencilView(DSV, _flags, texture->desc.clear.depthstencil.depth, texture->desc.clear.depthstencil.stencil);
      }
    }
  }

  if (raster_uavs_count[cmd] > 0)
  {
    // UAVs:
    const ezUInt32 count = raster_uavs_count[cmd];
    const ezUInt32 slot = raster_uavs_slot[cmd];

    deviceContexts[cmd]->OMSetRenderTargetsAndUnorderedAccessViews(rt_count, RTVs, DSV, slot, count, &raster_uavs[cmd][slot], nullptr);

    raster_uavs_count[cmd] = 0;
    raster_uavs_slot[cmd] = 8;
  }
  else
  {
    deviceContexts[cmd]->OMSetRenderTargets(rt_count, RTVs, DSV);
  }
}
void GraphicsDevice_DX11::RenderPassEnd(CommandList cmd)
{
  deviceContexts[cmd]->OMSetRenderTargets(0, nullptr, nullptr);

  // Perform resolves:
  int dst_counter = 0;
  for (auto& attachment : active_renderpass[cmd]->desc.attachments)
  {
    if (attachment.type == RenderPassAttachment::RESOLVE)
    {
      if (attachment.texture != nullptr)
      {
        auto dst_internal = to_internal(attachment.texture);

        int src_counter = 0;
        for (auto& src : active_renderpass[cmd]->desc.attachments)
        {
          if (src.type == RenderPassAttachment::RENDERTARGET && src.texture != nullptr)
          {
            if (src_counter == dst_counter)
            {
              auto src_internal = to_internal(src.texture);
              deviceContexts[cmd]->ResolveSubresource(dst_internal->resource.Get(), 0, src_internal->resource.Get(), 0, _ConvertFormat(attachment.texture->desc.Format));
              break;
            }
            src_counter++;
          }
        }
      }

      dst_counter++;
    }
  }
  active_renderpass[cmd] = nullptr;
}
void GraphicsDevice_DX11::BindScissorRects(ezUInt32 numRects, const Rect* rects, CommandList cmd)
{
  EZ_ASSERT_ALWAYS(rects != nullptr, "rects should not be null.");
  EZ_ASSERT_ALWAYS(numRects <= 8, "numRects must be less then or equal to 8.");
  D3D11_RECT pRects[8];
  for (ezUInt32 i = 0; i < numRects; ++i)
  {
    pRects[i].bottom = (LONG)rects[i].bottom;
    pRects[i].left = (LONG)rects[i].left;
    pRects[i].right = (LONG)rects[i].right;
    pRects[i].top = (LONG)rects[i].top;
  }
  deviceContexts[cmd]->RSSetScissorRects(numRects, pRects);
}
void GraphicsDevice_DX11::BindViewports(ezUInt32 NumViewports, const Viewport* pViewports, CommandList cmd)
{
  EZ_ASSERT_ALWAYS(NumViewports <= 6, "NumViewports must be less then or equal to 6.");
  D3D11_VIEWPORT d3dViewPorts[6];
  for (ezUInt32 i = 0; i < NumViewports; ++i)
  {
    d3dViewPorts[i].TopLeftX = pViewports[i].TopLeftX;
    d3dViewPorts[i].TopLeftY = pViewports[i].TopLeftY;
    d3dViewPorts[i].Width = pViewports[i].Width;
    d3dViewPorts[i].Height = pViewports[i].Height;
    d3dViewPorts[i].MinDepth = pViewports[i].MinDepth;
    d3dViewPorts[i].MaxDepth = pViewports[i].MaxDepth;
  }
  deviceContexts[cmd]->RSSetViewports(NumViewports, d3dViewPorts);
}
void GraphicsDevice_DX11::BindResource(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource)
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
      case ezRHIShaderStage::VertexShader:
        deviceContexts[cmd]->VSSetShaderResources(slot, 1, &SRV);
        break;
      case ezRHIShaderStage::HullShader:
        deviceContexts[cmd]->HSSetShaderResources(slot, 1, &SRV);
        break;
      case ezRHIShaderStage::DomainShader:
        deviceContexts[cmd]->DSSetShaderResources(slot, 1, &SRV);
        break;
      case ezRHIShaderStage::GeometryShader:
        deviceContexts[cmd]->GSSetShaderResources(slot, 1, &SRV);
        break;
      case ezRHIShaderStage::PixelShader:
        deviceContexts[cmd]->PSSetShaderResources(slot, 1, &SRV);
        break;
      case ezRHIShaderStage::ComputeShader:
        deviceContexts[cmd]->CSSetShaderResources(slot, 1, &SRV);
        break;
      default:
        EZ_ASSERT_NOT_IMPLEMENTED;
        break;
    }
  }
}
void GraphicsDevice_DX11::BindResources(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd)
{
  EZ_ASSERT_ALWAYS(count <= 16, "count must be less than or equal to 16.");
  ID3D11ShaderResourceView* srvs[16];
  for (ezUInt32 i = 0; i < count; ++i)
  {
    srvs[i] = resources[i] != nullptr && resources[i]->IsValid() ? to_internal(resources[i])->srv.Get() : nullptr;
  }

  switch (stage)
  {
    case ezRHIShaderStage::VertexShader:
      deviceContexts[cmd]->VSSetShaderResources(slot, count, srvs);
      break;
    case ezRHIShaderStage::HullShader:
      deviceContexts[cmd]->HSSetShaderResources(slot, count, srvs);
      break;
    case ezRHIShaderStage::DomainShader:
      deviceContexts[cmd]->DSSetShaderResources(slot, count, srvs);
      break;
    case ezRHIShaderStage::GeometryShader:
      deviceContexts[cmd]->GSSetShaderResources(slot, count, srvs);
      break;
    case ezRHIShaderStage::PixelShader:
      deviceContexts[cmd]->PSSetShaderResources(slot, count, srvs);
      break;
    case ezRHIShaderStage::ComputeShader:
      deviceContexts[cmd]->CSSetShaderResources(slot, count, srvs);
      break;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      break;
  }
}
void GraphicsDevice_DX11::BindUAV(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource)
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

    if (stage == ezRHIShaderStage::ComputeShader)
    {
      deviceContexts[cmd]->CSSetUnorderedAccessViews(slot, 1, &UAV, nullptr);
    }
    else
    {
      raster_uavs[cmd][slot] = UAV;
      raster_uavs_slot[cmd] = ezMath::Min(raster_uavs_slot[cmd], ezUInt8(slot));
      raster_uavs_count[cmd] = ezMath::Max(raster_uavs_count[cmd], ezUInt8(1));
    }
  }
}
void GraphicsDevice_DX11::BindUAVs(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd)
{
  EZ_ASSERT_ALWAYS(slot + count <= 8, "slot + count must be less then or equal to 8.");
  ID3D11UnorderedAccessView* uavs[8];
  for (ezUInt32 i = 0; i < count; ++i)
  {
    uavs[i] = resources[i] != nullptr && resources[i]->IsValid() ? to_internal(resources[i])->uav.Get() : nullptr;

    if (stage != ezRHIShaderStage::ComputeShader)
    {
      raster_uavs[cmd][slot + i] = uavs[i];
    }
  }

  if (stage == ezRHIShaderStage::ComputeShader)
  {
    deviceContexts[cmd]->CSSetUnorderedAccessViews(static_cast<ezUInt32>(slot), static_cast<ezUInt32>(count), uavs, nullptr);
  }
  else
  {
    raster_uavs_slot[cmd] = ezMath::Min(raster_uavs_slot[cmd], ezUInt8(slot));
    raster_uavs_count[cmd] = ezMath::Max(raster_uavs_count[cmd], ezUInt8(count));
  }
}
void GraphicsDevice_DX11::UnbindResources(ezUInt32 slot, ezUInt32 num, CommandList cmd)
{
  EZ_ASSERT_ALWAYS(num <= EZ_ARRAY_SIZE(__nullBlob), "Extend nullBlob to support more resource unbinding!");
  deviceContexts[cmd]->PSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
  deviceContexts[cmd]->VSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
  deviceContexts[cmd]->GSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
  deviceContexts[cmd]->HSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
  deviceContexts[cmd]->DSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
  deviceContexts[cmd]->CSSetShaderResources(slot, num, (ID3D11ShaderResourceView**)__nullBlob);
}
void GraphicsDevice_DX11::UnbindUAVs(ezUInt32 slot, ezUInt32 num, CommandList cmd)
{
  EZ_ASSERT_ALWAYS(num <= EZ_ARRAY_SIZE(__nullBlob), "Extend nullBlob to support more resource unbinding!");
  deviceContexts[cmd]->CSSetUnorderedAccessViews(slot, num, (ID3D11UnorderedAccessView**)__nullBlob, 0);

  raster_uavs_count[cmd] = 0;
  raster_uavs_slot[cmd] = 8;
}
void GraphicsDevice_DX11::BindSampler(ezEnum<ezRHIShaderStage> stage, const Sampler* sampler, ezUInt32 slot, CommandList cmd)
{
  if (sampler != nullptr && sampler->IsValid())
  {
    auto internal_state = to_internal(sampler);
    ID3D11SamplerState* SAM = internal_state->resource.Get();

    switch (stage)
    {
      case ezRHIShaderStage::VertexShader:
        deviceContexts[cmd]->VSSetSamplers(slot, 1, &SAM);
        break;
      case ezRHIShaderStage::HullShader:
        deviceContexts[cmd]->HSSetSamplers(slot, 1, &SAM);
        break;
      case ezRHIShaderStage::DomainShader:
        deviceContexts[cmd]->DSSetSamplers(slot, 1, &SAM);
        break;
      case ezRHIShaderStage::GeometryShader:
        deviceContexts[cmd]->GSSetSamplers(slot, 1, &SAM);
        break;
      case ezRHIShaderStage::PixelShader:
        deviceContexts[cmd]->PSSetSamplers(slot, 1, &SAM);
        break;
      case ezRHIShaderStage::ComputeShader:
        deviceContexts[cmd]->CSSetSamplers(slot, 1, &SAM);
        break;
      case ezRHIShaderStage::MeshShader:
      case ezRHIShaderStage::AmplificationShader:
        break;
      default:
        EZ_ASSERT_NOT_IMPLEMENTED;
        break;
    }
  }
}
void GraphicsDevice_DX11::BindConstantBuffer(ezEnum<ezRHIShaderStage> stage, const GPUBuffer* buffer, ezUInt32 slot, CommandList cmd)
{
  ID3D11Buffer* res = buffer != nullptr && buffer->IsValid() ? (ID3D11Buffer*)to_internal(buffer)->resource.Get() : nullptr;
  switch (stage)
  {
    case ezRHIShaderStage::VertexShader:
      deviceContexts[cmd]->VSSetConstantBuffers(slot, 1, &res);
      break;
    case ezRHIShaderStage::HullShader:
      deviceContexts[cmd]->HSSetConstantBuffers(slot, 1, &res);
      break;
    case ezRHIShaderStage::DomainShader:
      deviceContexts[cmd]->DSSetConstantBuffers(slot, 1, &res);
      break;
    case ezRHIShaderStage::GeometryShader:
      deviceContexts[cmd]->GSSetConstantBuffers(slot, 1, &res);
      break;
    case ezRHIShaderStage::PixelShader:
      deviceContexts[cmd]->PSSetConstantBuffers(slot, 1, &res);
      break;
    case ezRHIShaderStage::ComputeShader:
      deviceContexts[cmd]->CSSetConstantBuffers(slot, 1, &res);
      break;
    case ezRHIShaderStage::MeshShader:
    case ezRHIShaderStage::AmplificationShader:
      break;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      break;
  }
}
void GraphicsDevice_DX11::BindVertexBuffers(const GPUBuffer* const* vertexBuffers, ezUInt32 slot, ezUInt32 count, const ezUInt32* strides, const ezUInt32* offsets, CommandList cmd)
{
  EZ_ASSERT_ALWAYS(count <= 8, "count must be less then or equal to 8.");
  ID3D11Buffer* res[8] = {0};
  for (ezUInt32 i = 0; i < count; ++i)
  {
    res[i] = vertexBuffers[i] != nullptr && vertexBuffers[i]->IsValid() ? (ID3D11Buffer*)to_internal(vertexBuffers[i])->resource.Get() : nullptr;
  }
  deviceContexts[cmd]->IASetVertexBuffers(slot, count, res, strides, (offsets != nullptr ? offsets : reinterpret_cast<const ezUInt32*>(__nullBlob)));
}
void GraphicsDevice_DX11::BindIndexBuffer(const GPUBuffer* indexBuffer, const ezEnum<ezRHIIndexBufferFormat> format, ezUInt32 offset, CommandList cmd)
{
  ID3D11Buffer* res = indexBuffer != nullptr && indexBuffer->IsValid() ? (ID3D11Buffer*)to_internal(indexBuffer)->resource.Get() : nullptr;
  deviceContexts[cmd]->IASetIndexBuffer(res, (format == ezRHIIndexBufferFormat::UInt16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT), offset);
}
void GraphicsDevice_DX11::BindStencilRef(ezUInt32 value, CommandList cmd)
{
  stencilRef[cmd] = value;
}
void GraphicsDevice_DX11::BindBlendFactor(float r, float g, float b, float a, CommandList cmd)
{
  blendFactor[cmd].x = r;
  blendFactor[cmd].y = g;
  blendFactor[cmd].z = b;
  blendFactor[cmd].w = a;
}
void GraphicsDevice_DX11::BindPipelineState(const PipelineState* pso, CommandList cmd)
{
  if (active_pso[cmd] == pso)
    return;

  active_pso[cmd] = pso;
  dirty_pso[cmd] = true;
}
void GraphicsDevice_DX11::BindComputeShader(const Shader* cs, CommandList cmd)
{
  ID3D11ComputeShader* _cs = cs == nullptr ? nullptr : static_cast<ComputeShader_DX11*>(cs->internal_state.Borrow())->resource.Get();
  if (_cs != prev_cs[cmd])
  {
    deviceContexts[cmd]->CSSetShader(_cs, nullptr, 0);
    prev_cs[cmd] = _cs;
  }
}
void GraphicsDevice_DX11::Draw(ezUInt32 vertexCount, ezUInt32 startVertexLocation, CommandList cmd)
{
  pso_validate(cmd);
  commit_allocations(cmd);

  deviceContexts[cmd]->Draw(vertexCount, startVertexLocation);
}
void GraphicsDevice_DX11::DrawIndexed(ezUInt32 indexCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, CommandList cmd)
{
  pso_validate(cmd);
  commit_allocations(cmd);

  deviceContexts[cmd]->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}
void GraphicsDevice_DX11::DrawInstanced(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 startVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd)
{
  pso_validate(cmd);
  commit_allocations(cmd);

  deviceContexts[cmd]->DrawInstanced(vertexCount, instanceCount, startVertexLocation, startInstanceLocation);
}
void GraphicsDevice_DX11::DrawIndexedInstanced(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd)
{
  pso_validate(cmd);
  commit_allocations(cmd);

  deviceContexts[cmd]->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}
void GraphicsDevice_DX11::DrawInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  pso_validate(cmd);
  commit_allocations(cmd);

  deviceContexts[cmd]->DrawInstancedIndirect((ID3D11Buffer*)to_internal(args)->resource.Get(), args_offset);
}
void GraphicsDevice_DX11::DrawIndexedInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  pso_validate(cmd);
  commit_allocations(cmd);

  deviceContexts[cmd]->DrawIndexedInstancedIndirect((ID3D11Buffer*)to_internal(args)->resource.Get(), args_offset);
}
void GraphicsDevice_DX11::Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ, CommandList cmd)
{
  commit_allocations(cmd);

  deviceContexts[cmd]->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}
void GraphicsDevice_DX11::DispatchIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  commit_allocations(cmd);

  deviceContexts[cmd]->DispatchIndirect((ID3D11Buffer*)to_internal(args)->resource.Get(), args_offset);
}
void GraphicsDevice_DX11::CopyResource(const GPUResource* pDst, const GPUResource* pSrc, CommandList cmd)
{
  EZ_ASSERT_ALWAYS(pDst != nullptr && pSrc != nullptr, "pDst and pSrc cannot be null.");
  auto internal_state_src = to_internal(pSrc);
  auto internal_state_dst = to_internal(pDst);
  deviceContexts[cmd]->CopyResource(internal_state_dst->resource.Get(), internal_state_src->resource.Get());
}
void GraphicsDevice_DX11::UpdateBuffer(const GPUBuffer* buffer, const void* data, CommandList cmd, int dataSize)
{
  EZ_ASSERT_ALWAYS(buffer->desc.Usage != ezRHIUsage::Immutable, "Cannot update IMMUTABLE GPUBuffer!");
  EZ_ASSERT_ALWAYS((int)buffer->desc.ByteWidth >= dataSize || dataSize < 0, "Data size is too big!");

  if (dataSize == 0)
  {
    return;
  }

  auto internal_state = to_internal(buffer);

  dataSize = ezMath::Min((int)buffer->desc.ByteWidth, dataSize);

  if (buffer->desc.Usage == ezRHIUsage::Dynamic)
  {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = deviceContexts[cmd]->Map(internal_state->resource.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "GPUBuffer mapping failed!");
    memcpy(mappedResource.pData, data, (dataSize >= 0 ? dataSize : buffer->desc.ByteWidth));
    deviceContexts[cmd]->Unmap(internal_state->resource.Get(), 0);
  }
  else if (buffer->desc.BindFlags & BIND_CONSTANT_BUFFER || dataSize < 0)
  {
    deviceContexts[cmd]->UpdateSubresource(internal_state->resource.Get(), 0, nullptr, data, 0, 0);
  }
  else
  {
    D3D11_BOX box = {};
    box.left = 0;
    box.right = static_cast<ezUInt32>(dataSize);
    box.top = 0;
    box.bottom = 1;
    box.front = 0;
    box.back = 1;
    deviceContexts[cmd]->UpdateSubresource(internal_state->resource.Get(), 0, &box, data, 0, 0);
  }
}
void GraphicsDevice_DX11::QueryBegin(const GPUQuery* query, CommandList cmd)
{
  auto internal_state = to_internal(query);
  deviceContexts[cmd]->Begin(internal_state->resource.Get());
}
void GraphicsDevice_DX11::QueryEnd(const GPUQuery* query, CommandList cmd)
{
  auto internal_state = to_internal(query);
  deviceContexts[cmd]->End(internal_state->resource.Get());
}

GraphicsDevice::GPUAllocation GraphicsDevice_DX11::AllocateGPU(size_t dataSize, CommandList cmd)
{
  GPUAllocation result;
  if (dataSize == 0)
  {
    return result;
  }

  GPUAllocator& allocator = frame_allocators[cmd];
  if (allocator.buffer.desc.ByteWidth <= dataSize)
  {
    // If allocation too large, grow the allocator:
    allocator.buffer.desc.ByteWidth = ezUInt32((dataSize + 1) * 2);
    bool success = CreateBuffer(&allocator.buffer.desc, nullptr, &allocator.buffer);
    EZ_ASSERT_ALWAYS(success, "Failed to create buffer.");
    SetName(&allocator.buffer, "frame_allocator");
    allocator.byteOffset = 0;
  }

  auto internal_state = to_internal(&allocator.buffer);

  allocator.dirty = true;

  size_t position = allocator.byteOffset;
  bool wrap = position == 0 || position + dataSize > allocator.buffer.desc.ByteWidth || allocator.residentFrame != FRAMECOUNT;
  position = wrap ? 0 : position;

  // Issue buffer rename (realloc) on wrap, otherwise just append data:
  D3D11_MAP mapping = wrap ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  HRESULT hr = deviceContexts[cmd]->Map(internal_state->resource.Get(), 0, mapping, 0, &mappedResource);
  EZ_ASSERT_ALWAYS(SUCCEEDED(hr), "GPUBuffer mapping failed!");

  allocator.byteOffset = position + dataSize;
  allocator.residentFrame = FRAMECOUNT;

  result.buffer = &allocator.buffer;
  result.offset = (ezUInt32)position;
  result.data = (void*)((size_t)mappedResource.pData + position);
  return result;
}

void GraphicsDevice_DX11::EventBegin(const char* name, CommandList cmd)
{
  ezStringWChar text(name);
  if (text.GetElementCount() > 0)
  {
    userDefinedAnnotations[cmd]->BeginEvent(text);
  }
}
void GraphicsDevice_DX11::EventEnd(CommandList cmd)
{
  userDefinedAnnotations[cmd]->EndEvent();
}
void GraphicsDevice_DX11::SetMarker(const char* name, CommandList cmd)
{
  ezStringWChar text(name);
  if (text.GetElementCount() > 0)
  {
    userDefinedAnnotations[cmd]->SetMarker(text);
  }
}

#endif // WICKEDENGINE_BUILD_DX11
