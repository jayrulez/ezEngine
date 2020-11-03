#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Descriptors/BlendStateDescription.h>
#include <RHI/Descriptors/DepthStencilStateDescription.h>
#include <RHI/Descriptors/RasterizerStateDescription.h>
#include <RHI/Descriptors/VertexLayoutDescription.h>
#include <RHI/Util.h>

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

  bool operator==(const InputLayoutCacheKey& other) const
  {
    return Util::AreEquatable(VertexLayouts, other.VertexLayouts);
  }
};

template <>
struct ezHashHelper<InputLayoutCacheKey>
{
  EZ_ALWAYS_INLINE static ezUInt32 Hash(const InputLayoutCacheKey& value) { return ezHashHelper<ezUInt64>::Hash(0); }

  EZ_ALWAYS_INLINE static bool Equal(const InputLayoutCacheKey& a, const InputLayoutCacheKey& b) { return a == b; }
};

struct D3D11RasterizerStateCacheKey : public ezHashableStruct<D3D11RasterizerStateCacheKey>
{
  EZ_DECLARE_POD_TYPE();

  RHIRasterizerStateDescription RHIDescription;
  bool Multisampled = false;

  D3D11RasterizerStateCacheKey() = default;

  D3D11RasterizerStateCacheKey(const RHIRasterizerStateDescription& rhiDescription, bool multisampled)
  {
    RHIDescription = rhiDescription;
    Multisampled = multisampled;
  }

  bool operator==(const D3D11RasterizerStateCacheKey& other) const
  {
    return RHIDescription == other.RHIDescription && Multisampled == other.Multisampled;
  }
};

template <>
struct ezHashHelper<D3D11RasterizerStateCacheKey>
{
  EZ_ALWAYS_INLINE static ezUInt32 Hash(const D3D11RasterizerStateCacheKey& value) { return ezHashHelper<ezUInt64>::Hash(0); }

  EZ_ALWAYS_INLINE static bool Equal(const D3D11RasterizerStateCacheKey& a, const D3D11RasterizerStateCacheKey& b) { return a == b; }
};

class D3D11ResourceCache
{
private:
  struct SemanticIndices
  {
  private:
    ezUInt32 Position = 0;
    ezUInt32 TexCoord = 0;
    ezUInt32 Normal = 0;
    ezUInt32 Color = 0;

  public:
    static ezUInt32 GetAndIncrement(SemanticIndices& si, ezEnum<RHIVertexElementSemantic> type);
  };



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



  ID3D11RasterizerState* GetRasterizerState(const RHIRasterizerStateDescription& description, bool multisample);

  ID3D11RasterizerState* CreateNewRasterizerState(const D3D11RasterizerStateCacheKey& key);


  ID3D11InputLayout* GetInputLayout(ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts, ezDynamicArray<ezUInt8> vsBytecode);

  ID3D11InputLayout* CreateNewInputLayout(ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts, ezDynamicArray<ezUInt8> vsBytecode);

  ezString GetSemanticString(ezEnum<RHIVertexElementSemantic> semantic);


private:
  bool Disposed = false;
  ezMutex DeviceMutex;
  ID3D11Device* Device = nullptr;

  ezHashTable<RHIBlendStateDescription, ID3D11BlendState*> BlendStates;
  ezHashTable<RHIDepthStencilStateDescription, ID3D11DepthStencilState*> DepthStencilStates;
  ezHashTable<D3D11RasterizerStateCacheKey, ID3D11RasterizerState*> RasterizerStates;
  ezHashTable<InputLayoutCacheKey, ID3D11InputLayout*> InputLayouts;
};
