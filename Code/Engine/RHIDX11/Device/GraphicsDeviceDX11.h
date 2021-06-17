#pragma once

#include <RHI/Device/GraphicsDevice.h>
#include <RHIDX11/RHIDX11DLL.h>

EZ_DEFINE_AS_POD_TYPE(D3D11_SUBRESOURCE_DATA);
EZ_DEFINE_AS_POD_TYPE(D3D11_INPUT_ELEMENT_DESC);

class ezRHICommandListDX11;

class EZ_RHIDX11_DLL ezRHIGraphicsDeviceDX11 : public ezRHIGraphicsDevice
{
protected:
  D3D_DRIVER_TYPE driverType;
  D3D_FEATURE_LEVEL featureLevel;
  Microsoft::WRL::ComPtr<IDXGIFactory2> DXGIFactory;
  Microsoft::WRL::ComPtr<ID3D11Device> device;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;

  Microsoft::WRL::ComPtr<ID3D11Query> disjointQueries[BUFFERCOUNT + 3];
  ezDynamicArray<ezRHICommandListDX11*> m_CommandLists;


  void CreateBackBufferResources();

  ezDynamicArray<StaticSampler> common_samplers;

  struct EmptyResourceHandle : public ezRefCounted
  {
  }; // only care about control-block
  ezSharedPtr<EmptyResourceHandle> emptyresource;

public:
  ezRHIGraphicsDeviceDX11(bool debuglayer = false);
  virtual ~ezRHIGraphicsDeviceDX11() override;

  bool CreateSwapChain(const SwapChainDesc* pDesc, SwapChain* swapChain) const override;
  bool CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer) const override;
  bool CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, Texture* pTexture) const override;
  bool CreateShader(SHADERSTAGE stage, const void* pShaderBytecode, ezUInt64 BytecodeLength, Shader* pShader) const override;
  bool CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState) const override;
  bool CreateQueryHeap(const GPUQueryHeapDesc* pDesc, GPUQueryHeap* pQueryHeap) const override;
  bool CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso) const override;
  bool CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass) const override;

  int CreateSubresource(Texture* texture, SUBRESOURCE_TYPE type, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount) const override;
  int CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, uint64_t offset, uint64_t size = ~0) const override;

  void Map(const GPUResource* resource, Mapping* mapping) const override;
  void Unmap(const GPUResource* resource) const override;
  void QueryRead(const GPUQueryHeap* resource, uint32_t index, uint32_t count, uint64_t* results) const override;

  void SetCommonSampler(const StaticSampler* sam) override;

  void SetName(GPUResource* pResource, const char* name) override;

  void WaitForGPU() const override;

  ezRHICommandList* BeginCommandList(QUEUE_TYPE queue = QUEUE_GRAPHICS) override;
  void SubmitCommandLists() override;

  SHADERFORMAT GetShaderFormat() const override { return SHADERFORMAT_HLSL5; }

  Texture GetBackBuffer(const SwapChain* swapchain) const override;
  inline Microsoft::WRL::ComPtr<ID3D11Device> GetD3D11Device() const { return device; }
  const ezDynamicArray<StaticSampler>& GetCommonSamplers() const { return common_samplers; }
  inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetD3D11ImmediateContext() const { return immediateContext; }

};
