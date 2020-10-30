#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Descriptors/BlendStateDescription.h>
#include <RHI/Descriptors/DepthStencilStateDescription.h>
#include <RHI/Descriptors/RasterizerStateDescription.h>
#include <RHI/Descriptors/VertexLayoutDescription.h>

struct ID3D11Device;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11InputLayout;

struct InputLayoutCacheKey : public ezHashableStruct<InputLayoutCacheKey>
{
  ezDynamicArray<RHIVertexLayoutDescription> VertexLayouts;

  InputLayoutCacheKey() = default;
  InputLayoutCacheKey(ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts)
  {
    VertexLayouts = vertexLayouts;
  }

  static InputLayoutCacheKey CreateTempKey(ezDynamicArray<RHIVertexLayoutDescription> original);

  static InputLayoutCacheKey CreatePermanentKey(ezDynamicArray<RHIVertexLayoutDescription> original);

  bool operator==(const InputLayoutCacheKey& other)
  {
    return VertexLayouts == other.VertexLayouts;
  }
};

struct D3D11RasterizerStateCacheKey : public ezHashableStruct<D3D11RasterizerStateCacheKey>
{
  EZ_DECLARE_POD_TYPE();

  RHIRasterizerStateDescription RHIDescription;
  bool Multisampled = false;

  D3D11RasterizerStateCacheKey() = default;

  D3D11RasterizerStateCacheKey(const RHIRasterizerStateDescription& rhiDescription, bool multisampled);

  bool operator==(const D3D11RasterizerStateCacheKey& other)
  {
    return RHIDescription == other.RHIDescription && Multisampled == other.Multisampled;
  }
};

class D3D11ResourceCache
{
private:
  struct SemanticIndices
  {
  private:
    int _position = 0;
    int _texCoord = 0;
    int _normal = 0;
    int _color = 0;

  public:
    static int GetAndIncrement(SemanticIndices& si, ezEnum<RHIVertexElementSemantic> type);
  };



private:
  ezMutex DeviceMutex;
  ID3D11Device* Device = nullptr;

  ezHashTable<RHIBlendStateDescription, ID3D11BlendState*> BlendStates;
  ezHashTable<RHIDepthStencilStateDescription, ID3D11DepthStencilState*> DepthStencilStates;
  ezHashTable<RHIRasterizerStateDescription, ID3D11RasterizerState*> RasterizerStates;
  ezHashTable<InputLayoutCacheKey, ID3D11InputLayout*> InputLayouts;

public:
  D3D11ResourceCache(ID3D11Device* device);

  void GetPipelineResources(
    const RHIBlendStateDescription& blendDesc,
    const RHIDepthStencilStateDescription& dssDesc,
    const RHIRasterizerStateDescription& rasterDesc,
    bool multisample,
    ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts,
    ezDynamicArray<ezUInt8> vsBytecode,
    ID3D11BlendState* blendState,
    ID3D11DepthStencilState* depthState,
    ID3D11RasterizerState* rasterState,
    ID3D11InputLayout* inputLayout);

  void Dispose();
  bool IsDisposed() { return Disposed; }

private:
  ID3D11BlendState* GetBlendState(const RHIBlendStateDescription& description);


  ID3D11BlendState* CreateNewBlendState(const RHIBlendStateDescription& description);


  ID3D11DepthStencilState* GetDepthStencilState(const RHIDepthStencilStateDescription& description);


  ID3D11DepthStencilState* CreateNewDepthStencilState(const RHIDepthStencilStateDescription& description);

  D3D11_DEPTH_STENCILOP_DESC ToD3D11StencilOpDesc(RHIStencilBehaviorDescription sbd);

  ID3D11RasterizerState* GetRasterizerState(const RHIRasterizerStateDescription& description, bool multisample);

  ID3D11RasterizerState* CreateNewRasterizerState(const D3D11RasterizerStateCacheKey& key);


  ID3D11InputLayout* GetInputLayout(ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts, ezDynamicArray<ezUInt8> vsBytecode);

  ID3D11InputLayout* CreateNewInputLayout(ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts, ezDynamicArray<ezUInt8> vsBytecode);

  ezString GetSemanticString(ezEnum<RHIVertexElementSemantic> semantic);

private:
  bool Disposed = false;
};
