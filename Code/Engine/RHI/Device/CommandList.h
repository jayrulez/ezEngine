#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Graphics.h>

struct GPUAllocation
{
  void* data = nullptr;              // application can write to this. Reads might be not supported or slow. The offset is already applied
  const GPUBuffer* buffer = nullptr; // application can bind it to the GPU
  uint32_t offset = 0;               // allocation's offset from the GPUbuffer's beginning

  // Returns true if the allocation was successful
  inline bool IsValid() const { return data != nullptr && buffer != nullptr; }
};

class ezRHIGraphicsDevice;

class EZ_RHI_DLL ezRHICommandList
{
protected:
  ezRHICommandList() = default;

public:
  virtual ~ezRHICommandList() = default;
  ///////////////Thread-sensitive////////////////////////

  virtual void Initialize(ezRHIGraphicsDevice* pDevice) = 0;
  virtual void Destroy(ezRHIGraphicsDevice* pDevice) = 0;
  virtual void Submit(ezRHIGraphicsDevice* pDevice) = 0;

  virtual void WaitCommandList(ezRHICommandList* wait_for) {}
  virtual void RenderPassBegin(const SwapChain* swapchain) = 0;
  virtual void RenderPassBegin(const RenderPass* renderpass) = 0;
  virtual void RenderPassEnd() = 0;
  virtual void BindScissorRects(uint32_t numRects, const Rect* rects) = 0;
  virtual void BindViewports(uint32_t NumViewports, const Viewport* pViewports) = 0;
  virtual void BindResource(SHADERSTAGE stage, const GPUResource* resource, uint32_t slot, int subresource = -1) = 0;
  virtual void BindResources(SHADERSTAGE stage, const GPUResource* const* resources, uint32_t slot, uint32_t count) = 0;
  virtual void BindUAV(SHADERSTAGE stage, const GPUResource* resource, uint32_t slot, int subresource = -1) = 0;
  virtual void BindUAVs(SHADERSTAGE stage, const GPUResource* const* resources, uint32_t slot, uint32_t count) = 0;
  virtual void UnbindResources(uint32_t slot, uint32_t num) = 0;
  virtual void UnbindUAVs(uint32_t slot, uint32_t num) = 0;
  virtual void BindSampler(SHADERSTAGE stage, const Sampler* sampler, uint32_t slot) = 0;
  virtual void BindConstantBuffer(SHADERSTAGE stage, const GPUBuffer* buffer, uint32_t slot) = 0;
  virtual void BindVertexBuffers(const GPUBuffer* const* vertexBuffers, uint32_t slot, uint32_t count, const uint32_t* strides, const uint32_t* offsets) = 0;
  virtual void BindIndexBuffer(const GPUBuffer* indexBuffer, const INDEXBUFFER_FORMAT format, uint32_t offset) = 0;
  virtual void BindStencilRef(uint32_t value) = 0;
  virtual void BindBlendFactor(float r, float g, float b, float a) = 0;
  virtual void BindShadingRate(SHADING_RATE rate) {}
  virtual void BindPipelineState(const PipelineState* pso) = 0;
  virtual void BindComputeShader(const Shader* cs) = 0;
  virtual void Draw(uint32_t vertexCount, uint32_t startVertexLocation) = 0;
  virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation) = 0;
  virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation) = 0;
  virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation) = 0;
  virtual void DrawInstancedIndirect(const GPUBuffer* args, uint32_t args_offset) = 0;
  virtual void DrawIndexedInstancedIndirect(const GPUBuffer* args, uint32_t args_offset) = 0;
  virtual void Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ) = 0;
  virtual void DispatchIndirect(const GPUBuffer* args, uint32_t args_offset) = 0;
  virtual void DispatchMesh(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ) {}
  virtual void DispatchMeshIndirect(const GPUBuffer* args, uint32_t args_offset) {}
  virtual void CopyResource(const GPUResource* pDst, const GPUResource* pSrc) = 0;
  virtual void UpdateBuffer(const GPUBuffer* buffer, const void* data, int dataSize = -1) = 0;
  virtual void QueryBegin(const GPUQueryHeap* heap, uint32_t index) = 0;
  virtual void QueryEnd(const GPUQueryHeap* heap, uint32_t index) = 0;
  virtual void QueryResolve(const GPUQueryHeap* heap, uint32_t index, uint32_t count) {}
  virtual void Barrier(const GPUBarrier* barriers, uint32_t numBarriers) = 0;
  virtual void PushConstants(const void* data, uint32_t size) {}


  // Allocates temporary memory that the CPU can write and GPU can read.
  //	It is only alive for one frame and automatically invalidated after that.
  //	The CPU pointer gets invalidated as soon as there is a Draw() or Dispatch() event on the same thread
  //	This allocation can be used to provide temporary vertex buffer, index buffer or raw buffer data to shaders
  virtual GPUAllocation AllocateGPU(size_t dataSize) = 0;

  virtual void EventBegin(const char* name) = 0;
  virtual void EventEnd() = 0;
  virtual void SetMarker(const char* name) = 0;
};

#include <RHI/Device/Implementation/CommandList_inl.h>
