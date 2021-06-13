#pragma once

#include <RHI/CommandList/CommandList.h>
#include <RHIDX11/RHIDX11DLL.h>

#include <DXGI1_3.h>
#include <d3d11_3.h>
#include <wrl/client.h>

class EZ_RHIDX11_DLL ezRHIDX11CommantList : public ezRHICommandList
{
private:
  ///////////////Thread-sensitive////////////////////////

  friend class ezMemoryUtils;
  friend class ezRHIDX11Device;
  ezRHIDX11CommantList();

public:
  virtual ~ezRHIDX11CommantList();

  void RenderPassBegin(const ezRHISwapChain* swapchain) override;
  void RenderPassBegin(const ezRHIRenderPass* renderpass) override;
  void RenderPassEnd() override;
  void BindScissorRects(ezUInt32 numRects, const ezRHIRect* rects) override;
  void BindViewports(ezUInt32 NumViewports, const ezRHIViewport* pViewports) override;
  void BindResource(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* resource, ezUInt32 slot, ezInt32 subresource = -1) override;
  void BindResources(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* const* resources, ezUInt32 slot, ezUInt32 count) override;
  void BindUAV(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* resource, ezUInt32 slot, ezInt32 subresource = -1) override;
  void BindUAVs(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* const* resources, ezUInt32 slot, ezUInt32 count) override;
  void UnbindResources(ezUInt32 slot, ezUInt32 num) override;
  void UnbindUAVs(ezUInt32 slot, ezUInt32 num) override;
  void BindSampler(ezRHIShaderStage::Enum stage, const ezRHISamplerState* sampler, ezUInt32 slot) override;
  void BindConstantBuffer(ezRHIShaderStage::Enum stage, const ezRHIGPUBuffer* buffer, ezUInt32 slot) override;
  void BindVertexBuffers(const ezRHIGPUBuffer* const* vertexBuffers, ezUInt32 slot, ezUInt32 count, const ezUInt32* strides, const ezUInt32* offsets) override;
  void BindIndexBuffer(const ezRHIGPUBuffer* indexBuffer, const ezRHIIndexType::Enum format, ezUInt32 offset) override;
  void BindStencilRef(ezUInt32 value) override;
  void BindBlendFactor(float r, float g, float b, float a) override;
  void BindPipelineState(const ezRHIPipelineState* pso) override;
  void BindComputeShader(const ezRHIShader* cs) override;
  void Draw(ezUInt32 vertexCount, ezUInt32 startVertexLocation) override;
  void DrawIndexed(ezUInt32 indexCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation) override;
  void DrawInstanced(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 startVertexLocation, ezUInt32 startInstanceLocation) override;
  void DrawIndexedInstanced(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, ezUInt32 startInstanceLocation) override;
  void DrawInstancedIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset) override;
  void DrawIndexedInstancedIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset) override;
  void Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ) override;
  void DispatchIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset) override;
  void CopyResource(const ezRHIResourceBase* pDst, const ezRHIResourceBase* pSrc) override;
  void UpdateBuffer(const ezRHIGPUBuffer* buffer, const void* data, ezInt32 dataSize = -1) override;
  void QueryBegin(const ezRHIQueryHeap* heap, ezUInt32 index) override;
  void QueryEnd(const ezRHIQueryHeap* heap, ezUInt32 index) override;
  void Barrier(const ezRHIGPUBarrierDesc* barriers, ezUInt32 numBarriers) override {}

  GPUAllocation AllocateGPU(ezUInt64 dataSize) override;

  void EventBegin(const char* name) override;
  void EventEnd() override;
  void SetMarker(const char* name) override;

  ///////////////////////////////////////

  inline virtual void DispatchMesh(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ) {}
  inline virtual void DispatchMeshIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset) {}
  inline virtual void WaitCommandList(ezRHICommandList* wait_for) {}
  inline virtual void BindShadingRate(ezRHIShadingRate::Enum rate) {}
  inline virtual void QueryResolve(const ezRHIQueryHeap* heap, ezUInt32 index, ezUInt32 count) {}
  inline virtual void PushConstants(const void* data, ezUInt32 size) {}
};
