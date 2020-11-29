#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Resources/Pipeline.h>

#include <RHI/Backends/D3D11/D3D11ResourceCache.h>
#include <RHI/Backends/D3D11/D3D11ResourceLayout.h>

struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11GeometryShader;
struct ID3D11HullShader;
struct ID3D11DomainShader;
struct ID3D11PixelShader;
struct ID3D11ComputeShader;

class D3D11Pipeline : public RHIPipeline
{
private:
  bool ComputePipeline = false;
  ID3D11BlendState* BlendState = nullptr;
  ID3D11DepthStencilState* DepthStencilState = nullptr;
  ezUInt32 StencilReference = 0;
  ID3D11RasterizerState* RasterizerState = nullptr;
  D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
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

  virtual void SetName(const ezString& name) override;
  virtual bool IsDisposed() const override { return Disposed; }
  virtual void Dispose() override;

  virtual bool IsComputePipeline() const override;

public:
  D3D11Pipeline(D3D11ResourceCache* cache, const RHIGraphicsPipelineDescription& description);
  D3D11Pipeline(D3D11ResourceCache* cache, const RHIComputePipelineDescription& description);

  ID3D11BlendState* GetBlendState() const;
  ID3D11DepthStencilState* GetDepthStencilState() const;
  ezUInt32 GetStencilReference() const;
  ID3D11RasterizerState* GetRasterizerState() const;
  D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;
  ID3D11InputLayout* GetInputLayout() const;
  ID3D11VertexShader* GetVertexShader() const;
  ID3D11GeometryShader* GetGeometryShader() const;
  ID3D11HullShader* GetHullShader() const;
  ID3D11DomainShader* GetDomainShader() const;
  ID3D11PixelShader* GetPixelShader() const;
  ID3D11ComputeShader* GetComputeShader() const;
  ezDynamicArray<D3D11ResourceLayout*> GetResourceLayouts() const;
  ezDynamicArray<ezUInt32> GetVertexStrides() const;

  virtual ezUInt32 GetHash() const override
  {
    ezUInt32 hash = 0;

    // TODO: hash

    return hash;
  }
};
