#include <RHI/Backends/D3D11/D3D11ResourceCache.h>

D3D11ResourceCache::D3D11ResourceCache(ID3D11Device* device)
{
  Device = device;
}

void D3D11ResourceCache::GetPipelineResources(const RHIBlendStateDescription& blendDesc, const RHIDepthStencilStateDescription& dssDesc, const RHIRasterizerStateDescription& rasterDesc, bool multisample, ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts, ezDynamicArray<ezUInt8> vsBytecode, ID3D11BlendState* blendState, ID3D11DepthStencilState* depthState, ID3D11RasterizerState* rasterState, ID3D11InputLayout* inputLayout)
{
  ezLock lock(DeviceMutex);
  blendState = GetBlendState(blendDesc);
  depthState = GetDepthStencilState(dssDesc);
  rasterState = GetRasterizerState(rasterDesc, multisample);
  inputLayout = GetInputLayout(vertexLayouts, vsBytecode);
}

void D3D11ResourceCache::Dispose()
{
  for (auto it : BlendStates)
  {
    it.Value()->Release();
  }

  BlendStates.Clear();

  for (auto it : DepthStencilStates)
  {
    it.Value()->Release();
  }

  DepthStencilStates.Clear();

  for (auto it : RasterizerStates)
  {
    it.Value()->Release();
  }

  RasterizerStates.Clear();

  for (auto it : InputLayouts)
  {
    it.Value()->Release();
  }

  InputLayouts.Clear();
}

ID3D11BlendState* D3D11ResourceCache::GetBlendState(const RHIBlendStateDescription& description)
{
  EZ_ASSERT_DEBUG(DeviceMutex.IsLocked(), "");

  ID3D11BlendState* blendState = nullptr;

  if (!BlendStates.TryGetValue(description, blendState))
  {
    blendState = CreateNewBlendState(description);
    RHIBlendStateDescription key = description;

    key.AttachmentStates.SetCountUninitialized(description.AttachmentStates.GetCount());
    key.AttachmentStates.GetArrayPtr().CopyFrom(description.AttachmentStates.GetArrayPtr());

    BlendStates.Insert(key, blendState);
  }

  return blendState;
}

ID3D11BlendState* D3D11ResourceCache::CreateNewBlendState(const RHIBlendStateDescription& description)
{
  ezDynamicArray<RHIBlendAttachmentDescription> attachmentStates = description.AttachmentStates;
  D3D11_BLEND_DESC d3dBlendStateDesc;

  for (ezUInt32 i = 0; i < attachmentStates.GetCount(); i++)
  {
    RHIBlendAttachmentDescription state = attachmentStates[i];
    d3dBlendStateDesc.RenderTarget[i].BlendEnable = state.BlendEnabled;
    d3dBlendStateDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    d3dBlendStateDesc.RenderTarget[i].SrcBlend = D3D11FormatUtils::RHIToD3D11Blend(state.SourceColorFactor);
    d3dBlendStateDesc.RenderTarget[i].DestBlend = D3D11FormatUtils::RHIToD3D11Blend(state.DestinationColorFactor);
    d3dBlendStateDesc.RenderTarget[i].BlendOp = D3D11FormatUtils::RHIToD3D11BlendOperation(state.ColorFunction);
    d3dBlendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11FormatUtils::RHIToD3D11Blend(state.SourceAlphaFactor);
    d3dBlendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11FormatUtils::RHIToD3D11Blend(state.DestinationAlphaFactor);
    d3dBlendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11FormatUtils::RHIToD3D11BlendOperation(state.AlphaFunction);
  }

  d3dBlendStateDesc.AlphaToCoverageEnable = description.AlphaToCoverageEnabled;

  ID3D11BlendState* blendState;

  HRESULT hr = Device->CreateBlendState(&d3dBlendStateDesc, &blendState);

  return blendState;
}

ID3D11DepthStencilState* D3D11ResourceCache::GetDepthStencilState(const RHIDepthStencilStateDescription& description)
{
  EZ_ASSERT_DEBUG(DeviceMutex.IsLocked(), "");

  ID3D11DepthStencilState* dss;

  if (!DepthStencilStates.TryGetValue(description, dss))
  {
    dss = CreateNewDepthStencilState(description);
    RHIDepthStencilStateDescription key = description;
    DepthStencilStates.Insert(key, dss);
  }

  return dss;
}

ID3D11DepthStencilState* D3D11ResourceCache::CreateNewDepthStencilState(const RHIDepthStencilStateDescription& description)
{
  D3D11_DEPTH_STENCIL_DESC dssDesc;

  dssDesc.DepthFunc = D3D11FormatUtils::RHIToD3D11ComparisonFunc(description.DepthComparison);
  dssDesc.DepthEnable = description.DepthTestEnabled;
  dssDesc.DepthWriteMask = description.DepthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
  dssDesc.StencilEnable = description.StencilTestEnabled;
  dssDesc.FrontFace = ToD3D11StencilOpDesc(description.StencilFront);
  dssDesc.BackFace = ToD3D11StencilOpDesc(description.StencilBack);
  dssDesc.StencilReadMask = description.StencilReadMask;
  dssDesc.StencilWriteMask = description.StencilWriteMask;

  ID3D11DepthStencilState* dss;

  HRESULT hr = Device->CreateDepthStencilState(&dssDesc, &dss);

  return dss;
}

D3D11_DEPTH_STENCILOP_DESC D3D11ResourceCache::ToD3D11StencilOpDesc(RHIStencilBehaviorDescription sbd)
{
  D3D11_DEPTH_STENCILOP_DESC desc;
  desc.StencilFunc = D3D11FormatUtils::RHIToD3D11ComparisonFunc(sbd.Comparison);
  desc.StencilPassOp = D3D11FormatUtils::RHIToD3D11StencilOperation(sbd.Pass);
  desc.StencilFailOp = D3D11FormatUtils::RHIToD3D11StencilOperation(sbd.Fail);
  desc.StencilDepthFailOp = D3D11FormatUtils::RHIToD3D11StencilOperation(sbd.DepthFail);

  return desc;
}

ID3D11RasterizerState* D3D11ResourceCache::GetRasterizerState(const RHIRasterizerStateDescription& description, bool multisample)
{
  EZ_ASSERT_DEBUG(DeviceMutex.IsLocked(), "");

  D3D11RasterizerStateCacheKey key(description, multisample);
  ID3D11RasterizerState* rasterizerState;
  if (!RasterizerStates.TryGetValue(key, rasterizerState))
  {
    rasterizerState = CreateNewRasterizerState(key);
    RasterizerStates.Insert(key, rasterizerState);
  }

  return rasterizerState;
}

ID3D11RasterizerState* D3D11ResourceCache::CreateNewRasterizerState(const D3D11RasterizerStateCacheKey& key)
{
  D3D11_RASTERIZER_DESC rssDesc;

  rssDesc.CullMode = D3D11FormatUtils::RHIToD3D11CullMode(key.RHIDescription.CullMode);
  rssDesc.FillMode = D3D11FormatUtils::RHIToD3D11FillMode(key.RHIDescription.FillMode);
  rssDesc.DepthClipEnable = key.RHIDescription.DepthClipEnabled;
  rssDesc.ScissorEnable = key.RHIDescription.ScissorTestEnabled;
  rssDesc.FrontCounterClockwise = key.RHIDescription.FrontFace == RHIFrontFace::CounterClockwise;
  rssDesc.MultisampleEnable = key.Multisampled;

  ID3D11RasterizerState* rasterizerState;

  HRESULT hr = Device->CreateRasterizerState(&rssDesc, &rasterizerState);

  return rasterizerState;
}

ID3D11InputLayout* D3D11ResourceCache::GetInputLayout(ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts, ezDynamicArray<ezUInt8> vsBytecode)
{
  EZ_ASSERT_DEBUG(DeviceMutex.IsLocked(), "");

  if (vsBytecode.GetCount() == 0 || vertexLayouts.GetCount() == 0)
  {
    return nullptr;
  }

  InputLayoutCacheKey tempKey = InputLayoutCacheKey::CreateTempKey(vertexLayouts);

  ID3D11InputLayout* inputLayout;

  if (!InputLayouts.TryGetValue(tempKey, inputLayout))
  {
    inputLayout = CreateNewInputLayout(vertexLayouts, vsBytecode);
    InputLayoutCacheKey permanentKey = InputLayoutCacheKey::CreatePermanentKey(vertexLayouts);
    InputLayouts.Insert(permanentKey, inputLayout);
  }

  return inputLayout;
}

ID3D11InputLayout* D3D11ResourceCache::CreateNewInputLayout(ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts, ezDynamicArray<ezUInt8> vsBytecode)
{
  ezUInt32 totalCount = 0;
  for (ezUInt32 i = 0; i < vertexLayouts.GetCount(); i++)
  {
    totalCount += vertexLayouts[i].Elements.GetCount();
  }

  ezUInt32 element = 0; // Total element index across slots.
  ezDynamicArray<D3D11_INPUT_ELEMENT_DESC*> elements;
  elements.SetCountUninitialized(totalCount);

  SemanticIndices si;
  for (ezUInt32 slot = 0; slot < vertexLayouts.GetCount(); slot++)
  {
    ezDynamicArray<RHIVertexElementDescription> elementDescs = vertexLayouts[slot].Elements;
    ezUInt32 stepRate = vertexLayouts[slot].InstanceStepRate;
    ezUInt32 currentOffset = 0;
    for (ezUInt32 i = 0; i < elementDescs.GetCount(); i++)
    {
      RHIVertexElementDescription desc = elementDescs[i];
      D3D11_INPUT_ELEMENT_DESC elementDesc;
      elementDesc.SemanticName = GetSemanticString(desc.Semantic);
      elementDesc.SemanticIndex = SemanticIndices::GetAndIncrement(si, desc.Semantic);
      elementDesc.Format = D3D11FormatUtils::ToDxgiFormat(desc.Format);
      elementDesc.AlignedByteOffset = desc.Offset != 0 ? desc.Offset : currentOffset;
      elementDesc.InputSlot = slot;
      elementDesc.InputSlotClass = stepRate == 0 ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
      elementDesc.InstanceDataStepRate = stepRate;

      // TODO: check push back

      currentOffset += RHIFormatUtils::GetSize(desc.Format);
      element += 1;
    }
  }
  ID3D11InputLayout* layout = nullptr;

  HRESULT hr = Device->CreateInputLayout(*elements.GetData(), elements.GetCount(), vsBytecode.GetData(), vsBytecode.GetCount(), &layout);

  return layout;
}

ezString D3D11ResourceCache::GetSemanticString(ezEnum<RHIVertexElementSemantic> semantic)
{
  switch (semantic)
  {
    case RHIVertexElementSemantic::Position:
      return "POSITION";
    case RHIVertexElementSemantic::Normal:
      return "NORMAL";
    case RHIVertexElementSemantic::TextureCoordinate:
      return "TEXCOORD";
    case RHIVertexElementSemantic::Color:
      return "COLOR";
    default:
      EZ_REPORT_FAILURE("Invalid semantic");
      return ezString();
  }
}

InputLayoutCacheKey InputLayoutCacheKey::CreateTempKey(ezDynamicArray<RHIVertexLayoutDescription> original)
{
  return InputLayoutCacheKey{original};
}

InputLayoutCacheKey InputLayoutCacheKey::CreatePermanentKey(ezDynamicArray<RHIVertexLayoutDescription> original)
{
  ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts;
  vertexLayouts.SetCountUninitialized(original.GetCount());

  for (ezUInt32 i = 0; i < original.GetCount(); i++)
  {
    vertexLayouts[i].Stride = original[i].Stride;
    vertexLayouts[i].InstanceStepRate = original[i].InstanceStepRate;

    vertexLayouts[i].Elements.SetCountUninitialized(original[i].Elements.GetCount());

    vertexLayouts[i].Elements.GetArrayPtr().CopyFrom(original[i].Elements.GetArrayPtr());
  }

  return InputLayoutCacheKey{vertexLayouts};
}

int D3D11ResourceCache::SemanticIndices::GetAndIncrement(SemanticIndices& si, ezEnum<RHIVertexElementSemantic> type)
{
  switch (type)
  {
    case RHIVertexElementSemantic::Position:
      return si._position++;
    case RHIVertexElementSemantic::TextureCoordinate:
      return si._texCoord++;
    case RHIVertexElementSemantic::Normal:
      return si._normal++;
    case RHIVertexElementSemantic::Color:
      return si._color++;
    default:
      EZ_REPORT_FAILURE("Invalid Semantic");
      return 0;
  }
}

D3D11RasterizerStateCacheKey::D3D11RasterizerStateCacheKey(const RHIRasterizerStateDescription& rhiDescription, bool multisampled)
{
  RHIDescription = rhiDescription;
  Multisampled = multisampled;
}
