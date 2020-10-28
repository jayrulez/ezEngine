#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Resources/Pipeline.h>

#include <RHI/Backends/D3D11/D3D11ResourceLayout.h>
#include <RHI/Backends/D3D11/D3D11ResourceCache.h>

#include <d3d11_1.h>

class D3D11Pipeline : public RHIPipeline
{
private:
  bool ComputePipeline = false;
  ID3D11BlendState* BlendState = nullptr;
  ID3D11DepthStencilState* DepthStencilState = nullptr;
  ezUInt32 StencilReference = 0;
  ID3D11RasterizerState* RasterizerState = nullptr;
  D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
  ID3D11InputLayout* InputLayout = nullptr;
  ID3D11VertexShader* VertexShader = nullptr;
  ID3D11GeometryShader* GeometryShader = nullptr; // May be null.
  ID3D11HullShader* HullShader = nullptr;         // May be null.
  ID3D11DomainShader* DomainShader = nullptr;     // May be null.
  ID3D11PixelShader* PixelShader = nullptr;
  ID3D11ComputeShader* ComputeShader = nullptr;
  ezDynamicArray<D3D11ResourceLayout*> ResourceLayouts;
  ezDynamicArray<ezUInt32> VertexStrides;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override { return Name; }

  virtual void SetName(const ezString& name) override
  {
    Name = name;
  }
  virtual bool IsDisposed() const override { return Disposed; }
  virtual void Dispose() override
  {
    if (!Disposed)
    {
      Disposed = true;
    }
  }

  virtual bool IsComputePipeline() const override
  {
    return ComputePipeline;
  }

public:
  D3D11Pipeline(D3D11ResourceCache* cache, const RHIGraphicsPipelineDescription& description);
  D3D11Pipeline(D3D11ResourceCache* cache, const RHIComputePipelineDescription& description);

  ID3D11BlendState* GetBlendState() const { return BlendState; }
  ID3D11DepthStencilState* GetDepthStencilState() const { return DepthStencilState; }
  ezUInt32 GetStencilReference() const { return StencilReference; }
  ID3D11RasterizerState* GetRasterizerState() const { return RasterizerState; }
  D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return PrimitiveTopology; }
  ID3D11InputLayout* GetInputLayout() const { return InputLayout; }
  ID3D11VertexShader* GetVertexShader() const { return VertexShader; }
  ID3D11GeometryShader* GetGeometryShader() const { return GeometryShader; }
  ID3D11HullShader* GetHullShader() const { return HullShader; }
  ID3D11DomainShader* GetDomainShader() const { return DomainShader; }
  ID3D11PixelShader* GetPixelShader() const { return PixelShader; }
  ID3D11ComputeShader* GetComputeShader() const { return ComputeShader; }
  ezDynamicArray<D3D11ResourceLayout*> GetResourceLayouts() const { return ResourceLayouts; }
  ezDynamicArray<ezUInt32> GetVertexStrides() const { return VertexStrides; }
};
