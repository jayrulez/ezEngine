#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/CommandList/CommandList.h>
#include <RHIVulkan/Pipeline/VKPipeline.h>
#include <RHIVulkan/VKBasics.h>

class ezRHIVKCommandList : public ezRHICommandList
{
public:
  ezRHIVKCommandList(ezRHIVKDevice& device, ezRHICommandListType type);
  vk::CommandBuffer GetCommandList();

protected:
  virtual void ResetPlatform() override;
  virtual void ClosePlatform() override;
  virtual void BindPipelinePlatform(ezRHIPipeline* pState) override;
  virtual void BindBindingSetPlatform(ezRHIBindingSet* pBindingSet) override;
  virtual void BeginRenderPassPlatform(const ezRHIRenderPass* pRenderPass, const ezRHIFramebuffer* pFramebuffer, const ezRHIClearDescription& clearDesc) override;
  virtual void EndRenderPassPlatform() override;
  virtual void BeginEventPlatform(const ezString& name) override;
  virtual void EndEventPlatform() override;
  virtual void DrawPlatform(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 firstVertex, ezUInt32 firstInstance) override;
  virtual void DrawIndexedPlatform(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 firstIndex, ezInt32 vertexOffset, ezUInt32 firstInstance) override;
  virtual void DrawIndirectPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset) override;
  virtual void DrawIndexedIndirectPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset) override;
  virtual void DrawIndirectCountPlatform(
    const ezRHIResource* pArgumentBuffer,
    ezUInt64 argumentBufferOffset,
    const ezRHIResource* pCountBuffer,
    ezUInt64 countBufferOffset,
    ezUInt32 maxDrawCount,
    ezUInt32 stride) override;
  virtual void DrawIndexedIndirectCountPlatform(
    const ezRHIResource* pArgumentBuffer,
    ezUInt64 argumentBufferOffset,
    const ezRHIResource* pCountBuffer,
    ezUInt64 countBufferOffset,
    ezUInt32 maxDrawCount,
    ezUInt32 stride) override;
  virtual void DispatchPlatform(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ) override;
  virtual void DispatchIndirectPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset) override;
  virtual void DispatchMeshPlatform(ezUInt32 threadGroupCountX) override;
  virtual void DispatchRaysPlatform(const ezRHIRayTracingShaderTables& shaderTables, ezUInt32 width, ezUInt32 height, ezUInt32 depth) override;
  virtual void ResourceBarrierPlatform(const ezDynamicArray<ezRHIResourceBarrierDescription>& barriers) override;
  virtual void UAVResourceBarrierPlatform(const ezRHIResource* pResource) override;
  virtual void SetViewportPlatform(float x, float y, float width, float height) override;
  virtual void SetScissorRectPlatform(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom) override;
  virtual void IASetIndexBufferPlatform(const ezRHIResource* pResource, ezRHIResourceFormat::Enum format) override;
  virtual void IASetVertexBufferPlatform(ezUInt32 slot, const ezRHIResource* pResource) override;
  virtual void RSSetShadingRatePlatform(ezRHIShadingRate shadingRate, const ezStaticArray<ezRHIShadingRateCombiner, 2>& combiners) override;
  virtual void BuildBottomLevelASPlatform(
    const ezRHIResource* pSrc,
    const ezRHIResource* pDst,
    const ezRHIResource* pScratch,
    ezUInt64 scratchOffset,
    const ezDynamicArray<ezRHIRaytracingGeometryDescription>& descs,
    ezBitflags<ezRHIBuildAccelerationStructureFlags> flags) override;
  virtual void BuildTopLevelASPlatform(
    const ezRHIResource* pSrc,
    const ezRHIResource* pDst,
    const ezRHIResource* pScratch,
    ezUInt64 scratchOffset,
    const ezRHIResource* pInstanceData,
    ezUInt64 instanceOffset,
    ezUInt32 instanceCount,
    ezBitflags<ezRHIBuildAccelerationStructureFlags> flags) override;
  virtual void CopyAccelerationStructurePlatform(const ezRHIResource* pSrc, const ezRHIResource* pDst, ezRHICopyAccelerationStructureMode mode) override;
  virtual void CopyBufferPlatform(const ezRHIResource* pSrcBuffer, const ezRHIResource* pDstBuffer,
    const ezDynamicArray<ezRHIBufferCopyRegion>& regions) override;
  virtual void CopyBufferToTexturePlatform(const ezRHIResource* pSrcBuffer, const ezRHIResource* DstTexture,
    const ezDynamicArray<ezRHIBufferToTextureCopyRegion>& regions) override;
  virtual void CopyTexturePlatform(const ezRHIResource* pSrcTexture, const ezRHIResource* pDstTexture,
    const ezDynamicArray<ezRHITextureCopyRegion>& regions) override;
  virtual void WriteAccelerationStructuresPropertiesPlatform(
    const ezDynamicArray<ezRHIResource*>& accelerationStructures,
    const ezRHIQueryHeap* pQueryHeap,
    ezUInt32 firstQuery) override;
  virtual void ResolveQueryDataPlatform(
    const ezRHIQueryHeap* pQueryHeap,
    ezUInt32 firstQuery,
    ezUInt32 queryCount,
    const ezRHIResource* pDstBuffer,
    ezUInt64 dstOffset) override;

private:
  void BuildAccelerationStructure(vk::AccelerationStructureCreateInfoKHR& buildInfo, const vk::Buffer& instanceData, ezUInt64 instanceOffset, const ezRHIResource* src, const ezRHIResource* dst, const ezRHIResource* scratch, ezUInt64 scratchOffset);

private:
  ezRHIVKDevice& m_Device;
  vk::UniqueCommandBuffer m_CommandList;
  bool m_Closed = false;
  ezRHIVKPipeline* m_State;
  ezRHIBindingSet* m_BindingSet;
};
