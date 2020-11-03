#include <RHI/Backends/D3D11/D3D11Pipeline.h>
#include <RHI/Backends/D3D11/D3D11Shader.h>

void D3D11Pipeline::SetName(const ezString& name)
{
  Name = name;
}

void D3D11Pipeline::Dispose()
{
  if (!Disposed)
  {
    Disposed = true;
  }
}

bool D3D11Pipeline::IsComputePipeline() const
{
  return ComputePipeline;
}

D3D11Pipeline::D3D11Pipeline(D3D11ResourceCache* cache, const RHIGraphicsPipelineDescription& description)
  : RHIPipeline(description)
{
  ezDynamicArray<ezUInt8> vsBytecode;
  ezDynamicArray<RHIShader*> stages = description.ShaderSet.Shaders;
  for (ezUInt32 i = 0; i < description.ShaderSet.Shaders.GetCount(); i++)
  {

    //TODO: maybe reinterpret cast here instead of C-Style cast
    if (stages[i]->GetStage() == RHIShaderStages::Vertex)
    {
      D3D11Shader* d3d11VertexShader = reinterpret_cast<D3D11Shader*>(stages[i]);

      VertexShader = reinterpret_cast<ID3D11VertexShader*>(d3d11VertexShader->GetDeviceShader());
      vsBytecode = d3d11VertexShader->GetByteCode();
    }
    if (stages[i]->GetStage() == RHIShaderStages::Geometry)
    {
      GeometryShader = (ID3D11GeometryShader*)reinterpret_cast<D3D11Shader*>(stages[i])->GetDeviceShader();
    }
    if (stages[i]->GetStage() == RHIShaderStages::TessellationControl)
    {
      HullShader = (ID3D11HullShader*)reinterpret_cast<D3D11Shader*>(stages[i])->GetDeviceShader();
    }
    if (stages[i]->GetStage() == RHIShaderStages::TessellationEvaluation)
    {
      DomainShader = (ID3D11DomainShader*)reinterpret_cast<D3D11Shader*>(stages[i])->GetDeviceShader();
    }
    if (stages[i]->GetStage() == RHIShaderStages::Fragment)
    {
      PixelShader = (ID3D11PixelShader*)reinterpret_cast<D3D11Shader*>(stages[i])->GetDeviceShader();
    }
    if (stages[i]->GetStage() == RHIShaderStages::Compute)
    {
      ComputeShader = (ID3D11ComputeShader*)reinterpret_cast<D3D11Shader*>(stages[i])->GetDeviceShader();
    }
  }

  ID3D11BlendState* blendState = nullptr;
  ID3D11DepthStencilState* depthStencilState = nullptr;
  ID3D11RasterizerState* rasterizerState = nullptr;
  ID3D11InputLayout* inputLayout = nullptr;

  cache->GetPipelineResources(
    description.BlendState,
    description.DepthStencilState,
    description.RasterizerState,
    description.Outputs.SampleCount != RHITextureSampleCount::Count1,
    description.ShaderSet.VertexLayouts,
    vsBytecode,
    blendState,
    depthStencilState,
    rasterizerState,
    inputLayout);

  BlendState = blendState;
  DepthStencilState = depthStencilState;
  StencilReference = description.DepthStencilState.StencilReference;
  RasterizerState = rasterizerState;
  PrimitiveTopology = D3D11Formats::RHIToD3D11PrimitiveTopology(description.PrimitiveTopology);

  ezDynamicArray<RHIResourceLayout*> genericLayouts = description.ResourceLayouts;
  ResourceLayouts.SetCountUninitialized(genericLayouts.GetCount());
  for (ezUInt32 i = 0; i < ResourceLayouts.GetCount(); i++)
  {
    ResourceLayouts[i] = reinterpret_cast<D3D11ResourceLayout*>(genericLayouts[i]);
  }

  EZ_ASSERT_DEBUG(vsBytecode.GetCount() > 0 || ComputeShader != nullptr, "");
  if (vsBytecode.GetCount() > 0 && description.ShaderSet.VertexLayouts.GetCount() > 0)
  {
    InputLayout = inputLayout;
    ezUInt32 numVertexBuffers = description.ShaderSet.VertexLayouts.GetCount();
    VertexStrides.SetCountUninitialized(numVertexBuffers);
    for (ezUInt32 i = 0; i < numVertexBuffers; i++)
    {
      VertexStrides[i] = description.ShaderSet.VertexLayouts[i].Stride;
    }
  }
  else
  {
    VertexStrides.Clear();
  }
}

D3D11Pipeline::D3D11Pipeline(D3D11ResourceCache* cache, const RHIComputePipelineDescription& description)
  : RHIPipeline(description)
{
  ComputePipeline = true;



  ComputeShader = (ID3D11ComputeShader*)(reinterpret_cast<D3D11Shader*>(description.ComputeShader))->GetDeviceShader();
  ezDynamicArray<RHIResourceLayout*> genericLayouts = description.ResourceLayouts;
  ResourceLayouts.SetCountUninitialized(genericLayouts.GetCount());
  for (ezUInt32 i = 0; i < ResourceLayouts.GetCount(); i++)
  {
    ResourceLayouts[i] = reinterpret_cast<D3D11ResourceLayout*>(genericLayouts[i]);
  }
}

ID3D11BlendState* D3D11Pipeline::GetBlendState() const
{
  return BlendState;
}

ID3D11DepthStencilState* D3D11Pipeline::GetDepthStencilState() const
{
  return DepthStencilState;
}

ezUInt32 D3D11Pipeline::GetStencilReference() const
{
  return StencilReference;
}

ID3D11RasterizerState* D3D11Pipeline::GetRasterizerState() const
{
  return RasterizerState;
}

D3D_PRIMITIVE_TOPOLOGY D3D11Pipeline::GetPrimitiveTopology() const
{
  return PrimitiveTopology;
}

ID3D11InputLayout* D3D11Pipeline::GetInputLayout() const
{
  return InputLayout;
}

ID3D11VertexShader* D3D11Pipeline::GetVertexShader() const
{
  return VertexShader;
}

ID3D11GeometryShader* D3D11Pipeline::GetGeometryShader() const
{
  return GeometryShader;
}

ID3D11HullShader* D3D11Pipeline::GetHullShader() const
{
  return HullShader;
}

ID3D11DomainShader* D3D11Pipeline::GetDomainShader() const
{
  return DomainShader;
}

ID3D11PixelShader* D3D11Pipeline::GetPixelShader() const
{
  return PixelShader;
}

ID3D11ComputeShader* D3D11Pipeline::GetComputeShader() const
{
  return ComputeShader;
}

ezDynamicArray<D3D11ResourceLayout*> D3D11Pipeline::GetResourceLayouts() const
{
  return ResourceLayouts;
}

ezDynamicArray<ezUInt32> D3D11Pipeline::GetVertexStrides() const
{
  return VertexStrides;
}
