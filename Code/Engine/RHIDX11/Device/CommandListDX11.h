#pragma once

#include <RHIDX11/RHIDX11DLL.h>

#include <RHI/Device/CommandList.h>

struct GPUAllocator
{
  GPUBuffer buffer;
  size_t byteOffset = 0;
  uint64_t residentFrame = 0;
  bool dirty = false;
};

class ezRHIGraphicsDeviceDX11;

class EZ_RHIDX11_DLL ezRHICommandListDX11 : public ezRHICommandList
{
protected:
  friend class ezMemoryUtils;
  friend class ezRHIGraphicsDeviceDX11;
  ezRHICommandListDX11();

  ezRHIGraphicsDeviceDX11* m_pDeviceDX11 = nullptr;

  Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
  Microsoft::WRL::ComPtr<ID3D11CommandList> commandList;
  Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> userDefinedAnnotation;
  
  uint32_t stencilRef;
  ezColor blendFactor;

  ID3D11VertexShader* prev_vs = {};
  ID3D11PixelShader* prev_ps = {};
  ID3D11HullShader* prev_hs = {};
  ID3D11DomainShader* prev_ds = {};
  ID3D11GeometryShader* prev_gs = {};
  ID3D11ComputeShader* prev_cs = {};
  ezColor prev_blendfactor = {};
  uint32_t prev_samplemask = {};
  const BlendState* prev_bs = {};
  const RasterizerState* prev_rs = {};
  uint32_t prev_stencilRef = {};
  const DepthStencilState* prev_dss = {};
  const InputLayout* prev_il = {};
  PRIMITIVETOPOLOGY prev_pt = {};
  ezDynamicArray<const SwapChain*> swapchains;

  const PipelineState* active_pso = {};
  bool dirty_pso = {};
  void pso_validate();

  const RenderPass* active_renderpass = {};
  RenderPass dummyrenderpass;

  ID3D11UnorderedAccessView* raster_uavs[8] = {};
  uint8_t raster_uavs_slot = {};
  uint8_t raster_uavs_count = {};
  void validate_raster_uavs();

  GPUAllocator frame_allocator;
  void commit_allocations();

public:
  virtual void Initialize(ezRHIGraphicsDevice* pDevice) override;
  virtual void Destroy(ezRHIGraphicsDevice* pDevice) override;
  virtual void Submit(ezRHIGraphicsDevice* pDevice) override;

  ///////////////Thread-sensitive////////////////////////

  void RenderPassBegin(const SwapChain* swapchain) override;
  void RenderPassBegin(const RenderPass* renderpass) override;
  void RenderPassEnd() override;
  void BindScissorRects(uint32_t numRects, const Rect* rects) override;
  void BindViewports(uint32_t NumViewports, const Viewport* pViewports) override;
  void BindResource(SHADERSTAGE stage, const GPUResource* resource, uint32_t slot, int subresource = -1) override;
  void BindResources(SHADERSTAGE stage, const GPUResource* const* resources, uint32_t slot, uint32_t count) override;
  void BindUAV(SHADERSTAGE stage, const GPUResource* resource, uint32_t slot, int subresource = -1) override;
  void BindUAVs(SHADERSTAGE stage, const GPUResource* const* resources, uint32_t slot, uint32_t count) override;
  void UnbindResources(uint32_t slot, uint32_t num) override;
  void UnbindUAVs(uint32_t slot, uint32_t num) override;
  void BindSampler(SHADERSTAGE stage, const Sampler* sampler, uint32_t slot) override;
  void BindConstantBuffer(SHADERSTAGE stage, const GPUBuffer* buffer, uint32_t slot) override;
  void BindVertexBuffers(const GPUBuffer* const* vertexBuffers, uint32_t slot, uint32_t count, const uint32_t* strides, const uint32_t* offsets) override;
  void BindIndexBuffer(const GPUBuffer* indexBuffer, const INDEXBUFFER_FORMAT format, uint32_t offset) override;
  void BindStencilRef(uint32_t value) override;
  void BindBlendFactor(float r, float g, float b, float a) override;
  void BindPipelineState(const PipelineState* pso) override;
  void BindComputeShader(const Shader* cs) override;
  void Draw(uint32_t vertexCount, uint32_t startVertexLocation) override;
  void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation) override;
  void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation) override;
  void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation) override;
  void DrawInstancedIndirect(const GPUBuffer* args, uint32_t args_offset) override;
  void DrawIndexedInstancedIndirect(const GPUBuffer* args, uint32_t args_offset) override;
  void Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ) override;
  void DispatchIndirect(const GPUBuffer* args, uint32_t args_offset) override;
  void CopyResource(const GPUResource* pDst, const GPUResource* pSrc) override;
  void UpdateBuffer(const GPUBuffer* buffer, const void* data, int dataSize = -1) override;
  void QueryBegin(const GPUQueryHeap* heap, uint32_t index) override;
  void QueryEnd(const GPUQueryHeap* heap, uint32_t index) override;
  void Barrier(const GPUBarrier* barriers, uint32_t numBarriers) override {}

  GPUAllocation AllocateGPU(size_t dataSize) override;

  void EventBegin(const char* name) override;
  void EventEnd() override;
  void SetMarker(const char* name) override;

  inline const ezDynamicArray<const SwapChain*> GetSwapChains() const {
    return swapchains;
  }

  
  inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetD3D11DeviceContext() const { return deviceContext; }
  inline Microsoft::WRL::ComPtr<ID3D11CommandList> GetD3D11CommandList() const {
    return commandList;
  }
};

#include <RHIDX11/Device/Implementation/CommandListDX11_inl.h>
