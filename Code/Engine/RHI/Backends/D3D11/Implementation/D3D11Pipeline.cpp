#include <RHI/Backends/D3D11/D3D11Pipeline.h>
#include <RHI/Backends/D3D11/D3D11Shader.h>

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

  ID3D11BlendState* blendState;
  ID3D11DepthStencilState* depthStencilState;
  ID3D11RasterizerState* rasterizerState;
  ID3D11InputLayout* inputLayout;

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
  PrimitiveTopology = D3D11FormatUtils::RHIToD3D11PrimitiveTopology(description.PrimitiveTopology);

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
