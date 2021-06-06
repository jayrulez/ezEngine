#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/Descriptors/Descriptors.h>

class EZ_RHI_DLL ezRHICommandList
{
  EZ_DISALLOW_COPY_AND_ASSIGN(ezRHICommandList);

public:
  EZ_ALWAYS_INLINE ezRHICommandList() {}
  EZ_ALWAYS_INLINE virtual ~ezRHICommandList() {}

  ///////////////Thread-sensitive////////////////////////

  EZ_ALWAYS_INLINE virtual void WaitCommandList(ezRHICommandList* wait_for) {}
  virtual void RenderPassBegin(const ezRHISwapChain* swapchain) = 0;
  virtual void RenderPassBegin(const ezRHIRenderPass* renderpass) = 0;
  virtual void RenderPassEnd() = 0;
  virtual void BindScissorRects(ezUInt32 numRects, const ezRHIRect* rects) = 0;
  virtual void BindViewports(ezUInt32 NumViewports, const ezRHIViewport* pViewports) = 0;
  virtual void BindResource(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* resource, ezUInt32 slot, int subresource = -1) = 0;
  virtual void BindResources(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* const* resources, ezUInt32 slot, ezUInt32 count) = 0;
  virtual void BindUAV(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* resource, ezUInt32 slot, int subresource = -1) = 0;
  virtual void BindUAVs(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* const* resources, ezUInt32 slot, ezUInt32 count) = 0;
  virtual void UnbindResources(ezUInt32 slot, ezUInt32 num) = 0;
  virtual void UnbindUAVs(ezUInt32 slot, ezUInt32 num) = 0;
  virtual void BindSampler(ezRHIShaderStage::Enum stage, const ezRHISamplerState* sampler, ezUInt32 slot) = 0;
  virtual void BindConstantBuffer(ezRHIShaderStage::Enum stage, const ezRHIGPUBuffer* buffer, ezUInt32 slot) = 0;
  virtual void BindVertexBuffers(const ezRHIGPUBuffer* const* vertexBuffers, ezUInt32 slot, ezUInt32 count, const ezUInt32* strides, const ezUInt32* offsets) = 0;
  virtual void BindIndexBuffer(const ezRHIGPUBuffer* indexBuffer, const ezRHIIndexType::Enum format, ezUInt32 offset) = 0;
  virtual void BindStencilRef(ezUInt32 value) = 0;
  virtual void BindBlendFactor(float r, float g, float b, float a) = 0;
  EZ_ALWAYS_INLINE virtual void BindShadingRate(ezRHIShadingRate::Enum rate) {}
  virtual void BindPipelineState(const ezRHIPipelineState* pso) = 0;
  virtual void BindComputeShader(const ezRHIShader* cs) = 0;
  virtual void Draw(ezUInt32 vertexCount, ezUInt32 startVertexLocation) = 0;
  virtual void DrawIndexed(ezUInt32 indexCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation) = 0;
  virtual void DrawInstanced(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 startVertexLocation, ezUInt32 startInstanceLocation) = 0;
  virtual void DrawIndexedInstanced(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, ezUInt32 startInstanceLocation) = 0;
  virtual void DrawInstancedIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset) = 0;
  virtual void DrawIndexedInstancedIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset) = 0;
  virtual void Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ) = 0;
  virtual void DispatchIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset) = 0;
  EZ_ALWAYS_INLINE virtual void DispatchMesh(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ) {}
  EZ_ALWAYS_INLINE virtual void DispatchMeshIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset) {}
  virtual void CopyResource(const ezRHIResourceBase* pDst, const ezRHIResourceBase* pSrc) = 0;
  virtual void UpdateBuffer(const ezRHIGPUBuffer* buffer, const void* data, ezInt32 dataSize = -1) = 0;
  virtual void QueryBegin(const ezRHIGPUQueryHeap* heap, ezUInt32 index) = 0;
  virtual void QueryEnd(const ezRHIGPUQueryHeap* heap, ezUInt32 index) = 0;
  EZ_ALWAYS_INLINE virtual void QueryResolve(const ezRHIGPUQueryHeap* heap, ezUInt32 index, ezUInt32 count) {}
  virtual void Barrier(const ezRHIGPUBarrierDesc* barriers, ezUInt32 numBarriers) = 0;
  EZ_ALWAYS_INLINE virtual void BuildRaytracingAccelerationStructure(const ezRHIRaytracingAccelerationStructure* dst, const ezRHIRaytracingAccelerationStructure* src = nullptr) {}
  EZ_ALWAYS_INLINE virtual void BindRaytracingPipelineState(const ezRHIRaytracingPipelineState* rtpso) {}
  EZ_ALWAYS_INLINE virtual void DispatchRays(const ezRHIDispatchRaysDesc* desc) {}
  EZ_ALWAYS_INLINE virtual void PushConstants(const void* data, ezUInt32 size) {}

  struct GPUAllocation
  {
    void* Data = nullptr;              // application can write to this. Reads might be not supported or slow. The offset is already applied
    const ezRHIGPUBuffer* Buffer = nullptr; // application can bind it to the GPU
    ezUInt32 Offset = 0;               // allocation's offset from the GPUbuffer's beginning

    // Returns true if the allocation was successful
    EZ_ALWAYS_INLINE bool IsValid() const { return Data != nullptr && Buffer != nullptr; }
  };
  // Allocates temporary memory that the CPU can write and GPU can read.
  //	It is only alive for one frame and automatically invalidated after that.
  //	The CPU pointer gets invalidated as soon as there is a Draw() or Dispatch() event on the same thread
  //	This allocation can be used to provide temporary vertex buffer, index buffer or raw buffer data to shaders
  virtual GPUAllocation AllocateGPU(ezUInt64 dataSize) = 0;

  virtual void EventBegin(const char* name) = 0;
  virtual void EventEnd() = 0;
  virtual void SetMarker(const char* name) = 0;

protected:
};
