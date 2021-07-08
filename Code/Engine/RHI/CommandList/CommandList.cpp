#include <RHIPCH.h>

#include <RHI/CommandList/CommandList.h>

void ezRHICommandList::Reset()
{
  ResetPlatform();
}

void ezRHICommandList::Close()
{
  ClosePlatform();
}

void ezRHICommandList::BindPipeline(ezRHIPipeline* pState)
{
  BindPipelinePlatform(pState);
}

void ezRHICommandList::BindBindingSet(ezRHIBindingSet* pBindingSet)
{
  BindBindingSetPlatform(pBindingSet);
}

void ezRHICommandList::BeginRenderPass(const ezRHIRenderPass* pRenderPass, const ezRHIFramebuffer* pFramebuffer, const ezRHIClearDescription& clearDesc)
{
  BeginRenderPassPlatform(pRenderPass, pFramebuffer, clearDesc);
}

void ezRHICommandList::EndRenderPass()
{
  EndRenderPassPlatform();
}

void ezRHICommandList::BeginEvent(const ezString& name)
{
  BeginEventPlatform(name);
}

void ezRHICommandList::EndEvent()
{
  EndEventPlatform();
}

void ezRHICommandList::Draw(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 firstVertex, ezUInt32 firstInstance)
{
  DrawPlatform(vertexCount, instanceCount, firstVertex, firstInstance);
}

void ezRHICommandList::DrawIndexed(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 firstIndex, ezInt32 vertexOffset, ezUInt32 firstInstance)
{
  DrawIndexedPlatform(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void ezRHICommandList::DrawIndirect(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset)
{
  DrawIndirectPlatform(pArgumentBuffer, argumentBufferOffset);
}

void ezRHICommandList::DrawIndexedIndirect(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset)
{
  DrawIndexedIndirectPlatform(pArgumentBuffer, argumentBufferOffset);
}

void ezRHICommandList::DrawIndirectCount(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset, const ezRHIResource* pCountBuffer, ezUInt64 countBufferOffset, ezUInt32 maxDrawCount, ezUInt32 stride)
{
  DrawIndirectCountPlatform(pArgumentBuffer, argumentBufferOffset, pCountBuffer, countBufferOffset, maxDrawCount, stride);
}

void ezRHICommandList::DrawIndexedIndirectCount(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset, const ezRHIResource* pCountBuffer, ezUInt64 countBufferOffset, ezUInt32 maxDrawCount, ezUInt32 stride)
{
}

void ezRHICommandList::Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ)
{
  DispatchPlatform(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void ezRHICommandList::DispatchIndirect(const ezRHIResource* pArgumentBuffer, ezUInt64 argumentBufferOffset)
{
  DispatchIndirectPlatform(pArgumentBuffer, argumentBufferOffset);
}

void ezRHICommandList::DispatchMesh(ezUInt32 threadGroupCountX)
{
  DispatchMeshPlatform(threadGroupCountX);
}

void ezRHICommandList::DispatchRays(const ezRHIRayTracingShaderTables& shaderTables, ezUInt32 width, ezUInt32 height, ezUInt32 depth)
{
  DispatchRaysPlatform(shaderTables, width, height, depth);
}

void ezRHICommandList::ResourceBarrier(const ezDynamicArray<ezRHIResourceBarrierDescription>& barriers)
{
  ResourceBarrierPlatform(barriers);
}

void ezRHICommandList::UAVResourceBarrier(const ezRHIResource* pResource)
{
  UAVResourceBarrierPlatform(pResource);
}

void ezRHICommandList::SetViewport(float x, float y, float width, float height)
{
  SetViewportPlatform(x, y, width, height);
}

void ezRHICommandList::SetScissorRect(ezInt32 left, ezInt32 top, ezUInt32 right, ezUInt32 bottom)
{
  SetScissorRectPlatform(left, top, right, bottom);
}

void ezRHICommandList::IASetIndexBuffer(const ezRHIResource* pResource, ezRHIResourceFormat::Enum format)
{
  IASetIndexBufferPlatform(pResource, format);
}

void ezRHICommandList::IASetVertexBuffer(ezUInt32 slot, const ezRHIResource* pResource)
{
  IASetVertexBufferPlatform(slot, pResource);
}

void ezRHICommandList::RSSetShadingRate(ezRHIShadingRate shadingRate, const ezStaticArray<ezRHIShadingRateCombiner, 2>& combiners)
{
  RSSetShadingRatePlatform(shadingRate, combiners);
}

void ezRHICommandList::BuildBottomLevelAS(const ezRHIResource* pSrc, const ezRHIResource* pDst, const ezRHIResource* pScratch, ezUInt64 scratchOffset, const ezDynamicArray<ezRHIRaytracingGeometryDescription>& descs, ezBitflags<ezRHIBuildAccelerationStructureFlags> flags)
{
  BuildBottomLevelASPlatform(pSrc, pDst, pScratch, scratchOffset, descs, flags);
}

void ezRHICommandList::BuildTopLevelAS(const ezRHIResource* pSrc, const ezRHIResource* pDst, const ezRHIResource* pScratch, ezUInt64 scratchOffset, const ezRHIResource* pInstanceData, ezUInt64 instanceOffset, ezUInt32 instanceCount, ezBitflags<ezRHIBuildAccelerationStructureFlags> flags)
{
  BuildTopLevelASPlatform(pSrc, pDst, pScratch, scratchOffset, pInstanceData, instanceOffset, instanceCount, flags);
}

void ezRHICommandList::CopyAccelerationStructure(const ezRHIResource* pSrc, const ezRHIResource* pDst, ezRHICopyAccelerationStructureMode mode)
{
  CopyAccelerationStructurePlatform(pSrc, pDst, mode);
}

void ezRHICommandList::CopyBuffer(const ezRHIResource* pSrcBuffer, const ezRHIResource* pDstBuffer, const ezDynamicArray<ezRHIBufferCopyRegion>& regions)
{
  CopyBufferPlatform(pSrcBuffer, pDstBuffer, regions);
}

void ezRHICommandList::CopyBufferToTexture(const ezRHIResource* pSrcBuffer, const ezRHIResource* DstTexture, const ezDynamicArray<ezRHIBufferToTextureCopyRegion>& regions)
{
  CopyBufferToTexturePlatform(pSrcBuffer, DstTexture, regions);
}

void ezRHICommandList::CopyTexture(const ezRHIResource* pSrcTexture, const ezRHIResource* pDstTexture, const ezDynamicArray<ezRHITextureCopyRegion>& regions)
{
  CopyTexturePlatform(pSrcTexture, pDstTexture, regions);
}

void ezRHICommandList::WriteAccelerationStructuresProperties(const ezDynamicArray<ezRHIResource*>& accelerationStructures, const ezRHIQueryHeap* pQueryHeap, ezUInt32 firstQuery)
{
  WriteAccelerationStructuresPropertiesPlatform(accelerationStructures, pQueryHeap, firstQuery);
}

void ezRHICommandList::ResolveQueryData(const ezRHIQueryHeap* pQueryHeap, ezUInt32 firstQuery, ezUInt32 queryCount, const ezRHIResource* pDstBuffer, ezUInt64 dstOffset)
{
  ResolveQueryDataPlatform(pQueryHeap, firstQuery, queryCount, pDstBuffer, dstOffset);
}
