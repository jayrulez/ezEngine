#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class EZ_RHI_DLL ezRHICommandList : public ezRHIQueryInterface
{
public:
  virtual ~ezRHICommandList() = default;
  void Reset();
  void Close();
  void BindPipeline(const ezRHIPipeline* pState);
  void BindBindingSet(const ezRHIBindingSet* pBindingSet);
  void BeginRenderPass(const ezRHIRenderPass* pRenderPass, const ezRHIFramebuffer* pFramebuffer, const ezRHIClearDescription& clearDesc);
  void EndRenderPass();
  void BeginEvent(const ezString& name);
  void EndEvent();
  void Draw(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 firstVertex, ezUInt32 firstInstance);
  void DrawIndexed(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 firstIndex, ezInt32 vertexOffset, ezUInt32 firstInstance);
  void DrawIndirect(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset);
  void DrawIndexedIndirect(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset);
  void DrawIndirectCount(
    const ezRHIResource* pArgumentBuffer,
    ezUInt64 argumentBufferOffset,
    const ezRHIResource* pCountBuffer,
    ezUInt64 countBufferOffset,
    ezUInt32 maxDrawCount,
    ezUInt32 stride);
  void DrawIndexedIndirectCount(
    const ezRHIResource* pArgumentBuffer,
    ezUInt64 argumentBufferOffset,
    const ezRHIResource* pCountBuffer,
    ezUInt64 countBufferOffset,
    ezUInt32 maxDrawCount,
    ezUInt32 stride);
  void Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ);
  void DispatchIndirect(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset);
  void DispatchMesh(ezUInt32 threadGroupCountX);
  void DispatchRays(const ezRHIRayTracingShaderTables& shaderTables, ezUInt32 width, ezUInt32 height, ezUInt32 depth);
  void ResourceBarrier(const ezDynamicArray<ezRHIResourceBarrierDescription>& barriers);
  void UAVResourceBarrier(const ezRHIResource* pResource);
  void SetViewport(float x, float y, float width, float height);
  void SetScissorRect(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom);
  void IASetIndexBuffer(const ezRHIResource* pResource, ezRHIResourceFormat format);
  void IASetVertexBuffer(ezUInt32 slot, const ezRHIResource* pResource);
  void RSSetShadingRate(ezRHIShadingRate shadingRate, const ezStaticArray<ezRHIShadingRateCombiner, 2>& combiners);
  void BuildBottomLevelAS(
    const ezRHIResource* pSrc,
    const ezRHIResource* pDst,
    const ezRHIResource* pScratch,
    ezUInt64 scratchOffset,
    const ezDynamicArray<ezRHIRaytracingGeometryDescription>& descs,
    ezBitflags<ezRHIBuildAccelerationStructureFlags> flags);
  void BuildTopLevelAS(
    const ezRHIResource* pSrc,
    const ezRHIResource* pDst,
    const ezRHIResource* pScratch,
    ezUInt64 scratchOffset,
    const ezRHIResource* pInstanceData,
    ezUInt64 instanceOffset,
    ezUInt32 instanceCount,
    ezBitflags<ezRHIBuildAccelerationStructureFlags> flags);
  void CopyAccelerationStructure(const ezRHIResource* pSrc, const ezRHIResource* pDst, ezRHICopyAccelerationStructureMode mode);
  void CopyBuffer(const ezRHIResource* pSrcBuffer, const ezRHIResource* pDstBuffer,
    const ezDynamicArray<ezRHIBufferCopyRegion>& regions);
  void CopyBufferToTexture(const ezRHIResource* pSrcBuffer, const ezRHIResource* DstTexture,
    const ezDynamicArray<ezRHIBufferToTextureCopyRegion>& regions);
  void CopyTexture(const ezRHIResource* pSrcTexture, const ezRHIResource* pDstTexture,
    const ezDynamicArray<ezRHITextureCopyRegion>& regions);
  void WriteAccelerationStructuresProperties(
    const ezDynamicArray<ezRHIResource*>& accelerationStructures,
    const ezRHIQueryHeap* pQueryHeap,
    ezUInt32 firstQuery);
  void ResolveQueryData(
    const ezRHIQueryHeap* pQueryHeap,
    ezUInt32 firstQuery,
    ezUInt32 queryQount,
    const ezRHIResource* pDstBuffer,
    ezUInt64 dstOffset);

  protected:
  virtual void ResetPlatform() = 0;
  virtual void ClosePlatform() = 0;
  virtual void BindPipelinePlatform(const ezRHIPipeline* pState) = 0;
  virtual void BindBindingSetPlatform(const ezRHIBindingSet* pBindingSet) = 0;
  virtual void BeginRenderPassPlatform(const ezRHIRenderPass* pRenderPass, const ezRHIFramebuffer* pFramebuffer, const ezRHIClearDescription& clearDesc) = 0;
  virtual void EndRenderPassPlatform() = 0;
  virtual void BeginEventPlatform(const ezString& name) = 0;
  virtual void EndEventPlatform() = 0;
  virtual void DrawPlatform(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 firstVertex, ezUInt32 firstInstance) = 0;
  virtual void DrawIndexedPlatform(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 firstIndex, ezInt32 vertexOffset, ezUInt32 firstInstance) = 0;
  virtual void DrawIndirectPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset) = 0;
  virtual void DrawIndexedIndirectPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset) = 0;
  virtual void DrawIndirectCountPlatform(
    const ezRHIResource* pArgumentBuffer,
    ezUInt64 argumentBufferOffset,
    const ezRHIResource* pCountBuffer,
    ezUInt64 countBufferOffset,
    ezUInt32 maxDrawCount,
    ezUInt32 stride) = 0;
  virtual void DrawIndexedIndirectCountPlatform(
    const ezRHIResource* pArgumentBuffer,
    ezUInt64 argumentBufferOffset,
    const ezRHIResource* pCountBuffer,
    ezUInt64 countBufferOffset,
    ezUInt32 maxDrawCount,
    ezUInt32 stride) = 0;
  virtual void DispatchPlatform(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ) = 0;
  virtual void DispatchIndirectPlatform(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset) = 0;
  virtual void DispatchMeshPlatform(ezUInt32 threadGroupCountX) = 0;
  virtual void DispatchRaysPlatform(const ezRHIRayTracingShaderTables& shaderTables, ezUInt32 width, ezUInt32 height, ezUInt32 depth) = 0;
  virtual void ResourceBarrierPlatform(const ezDynamicArray<ezRHIResourceBarrierDescription>& barriers) = 0;
  virtual void UAVResourceBarrierPlatform(const ezRHIResource* pResource) = 0;
  virtual void SetViewportPlatform(float x, float y, float width, float height) = 0;
  virtual void SetScissorRectPlatform(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom) = 0;
  virtual void IASetIndexBufferPlatform(const ezRHIResource* pResource, ezRHIResourceFormat format) = 0;
  virtual void IASetVertexBufferPlatform(ezUInt32 slot, const ezRHIResource* pResource) = 0;
  virtual void RSSetShadingRatePlatform(ezRHIShadingRate shadingRate, const ezStaticArray<ezRHIShadingRateCombiner, 2>& combiners) = 0;
  virtual void BuildBottomLevelASPlatform(
    const ezRHIResource* pSrc,
    const ezRHIResource* pDst,
    const ezRHIResource* pScratch,
    ezUInt64 scratchOffset,
    const ezDynamicArray<ezRHIRaytracingGeometryDescription>& descs,
    ezBitflags<ezRHIBuildAccelerationStructureFlags> flags) = 0;
  virtual void BuildTopLevelASPlatform(
    const ezRHIResource* pSrc,
    const ezRHIResource* pDst,
    const ezRHIResource* pScratch,
    ezUInt64 scratchOffset,
    const ezRHIResource* pInstanceData,
    ezUInt64 instanceOffset,
    ezUInt32 instanceCount,
    ezBitflags<ezRHIBuildAccelerationStructureFlags> flags) = 0;
  virtual void CopyAccelerationStructurePlatform(const ezRHIResource* pSrc, const ezRHIResource* pDst, ezRHICopyAccelerationStructureMode mode) = 0;
  virtual void CopyBufferPlatform(const ezRHIResource* pSrcBuffer, const ezRHIResource* pDstBuffer,
    const ezDynamicArray<ezRHIBufferCopyRegion>& regions) = 0;
  virtual void CopyBufferToTexturePlatform(const ezRHIResource* pSrcBuffer, const ezRHIResource* DstTexture,
    const ezDynamicArray<ezRHIBufferToTextureCopyRegion>& regions) = 0;
  virtual void CopyTexturePlatform(const ezRHIResource* pSrcTexture, const ezRHIResource* pDstTexture,
    const ezDynamicArray<ezRHITextureCopyRegion>& regions) = 0;
  virtual void WriteAccelerationStructuresPropertiesPlatform(
    const ezDynamicArray<ezRHIResource*>& accelerationStructures,
    const ezRHIQueryHeap* pQueryHeap,
    ezUInt32 firstQuery) = 0;
  virtual void ResolveQueryDataPlatform(
    const ezRHIQueryHeap* pQueryHeap,
    ezUInt32 firstQuery,
    ezUInt32 queryQount,
    const ezRHIResource* pDstBuffer,
    ezUInt64 dstOffset) = 0;
};
