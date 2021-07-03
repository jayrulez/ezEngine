
#pragma once

#include <Foundation/Math/Color.h>
#include <Foundation/Math/Rect.h>
#include <RHI/Renderer/Basics.h>

class EZ_RHI_DLL ezRHICommandEncoderCommonPlatformInterface
{
public:
  // State setting functions

  virtual void SetShaderPlatform(const ezRHIShader* pShader) = 0;

  virtual void SetConstantBufferPlatform(ezUInt32 uiSlot, const ezRHIBuffer* pBuffer) = 0;
  virtual void SetSamplerStatePlatform(ezRHIShaderStage::Enum Stage, ezUInt32 uiSlot, const ezRHISamplerState* pSamplerState) = 0;
  virtual void SetResourceViewPlatform(ezRHIShaderStage::Enum Stage, ezUInt32 uiSlot, const ezRHIResourceView* pResourceView) = 0;
  virtual void SetUnorderedAccessViewPlatform(ezUInt32 uiSlot, const ezRHIUnorderedAccessView* pUnorderedAccessView) = 0;

  // Fence & Query functions

  virtual void InsertFencePlatform(const ezRHIFence* pFence) = 0;
  virtual bool IsFenceReachedPlatform(const ezRHIFence* pFence) = 0;
  virtual void WaitForFencePlatform(const ezRHIFence* pFence) = 0;

  virtual void BeginQueryPlatform(const ezRHIQuery* pQuery) = 0;
  virtual void EndQueryPlatform(const ezRHIQuery* pQuery) = 0;
  virtual ezResult GetQueryResultPlatform(const ezRHIQuery* pQuery, ezUInt64& uiQueryResult) = 0;

  // Timestamp functions

  virtual void InsertTimestampPlatform(ezRHITimestampHandle hTimestamp) = 0;

  // Resource update functions

  virtual void ClearUnorderedAccessViewPlatform(const ezRHIUnorderedAccessView* pUnorderedAccessView, ezVec4 clearValues) = 0;
  virtual void ClearUnorderedAccessViewPlatform(const ezRHIUnorderedAccessView* pUnorderedAccessView, ezVec4U32 clearValues) = 0;

  virtual void CopyBufferPlatform(const ezRHIBuffer* pDestination, const ezRHIBuffer* pSource) = 0;
  virtual void CopyBufferRegionPlatform(const ezRHIBuffer* pDestination, ezUInt32 uiDestOffset, const ezRHIBuffer* pSource, ezUInt32 uiSourceOffset, ezUInt32 uiByteCount) = 0;

  virtual void UpdateBufferPlatform(const ezRHIBuffer* pDestination, ezUInt32 uiDestOffset, ezArrayPtr<const ezUInt8> pSourceData, ezRHIUpdateMode::Enum updateMode) = 0;

  virtual void CopyTexturePlatform(const ezRHITexture* pDestination, const ezRHITexture* pSource) = 0;
  virtual void CopyTextureRegionPlatform(const ezRHITexture* pDestination, const ezRHITextureSubresource& DestinationSubResource, const ezVec3U32& DestinationPoint, const ezRHITexture* pSource, const ezRHITextureSubresource& SourceSubResource, const ezBoundingBoxu32& Box) = 0;

  virtual void UpdateTexturePlatform(const ezRHITexture* pDestination, const ezRHITextureSubresource& DestinationSubResource, const ezBoundingBoxu32& DestinationBox, const ezRHISystemMemoryDescription& pSourceData) = 0;

  virtual void ResolveTexturePlatform(const ezRHITexture* pDestination, const ezRHITextureSubresource& DestinationSubResource, const ezRHITexture* pSource, const ezRHITextureSubresource& SourceSubResource) = 0;

  virtual void ReadbackTexturePlatform(const ezRHITexture* pTexture) = 0;

  virtual void CopyTextureReadbackResultPlatform(const ezRHITexture* pTexture, ezArrayPtr<ezRHITextureSubresource> SourceSubResource, ezArrayPtr<ezRHISystemMemoryDescription> TargetData) = 0;

  virtual void GenerateMipMapsPlatform(const ezRHIResourceView* pResourceView) = 0;

  // Misc

  virtual void FlushPlatform() = 0;

  // Debug helper functions

  virtual void PushMarkerPlatform(const char* Marker) = 0;
  virtual void PopMarkerPlatform() = 0;
  virtual void InsertEventMarkerPlatform(const char* Marker) = 0;
};

class EZ_RHI_DLL ezRHICommandEncoderRenderPlatformInterface
{
public:
  // Draw functions

  virtual void ClearPlatform(const ezColor& ClearColor, ezUInt32 uiRenderTargetClearMask, bool bClearDepth, bool bClearStencil, float fDepthClear, ezUInt8 uiStencilClear) = 0;

  virtual void DrawPlatform(ezUInt32 uiVertexCount, ezUInt32 uiStartVertex) = 0;
  virtual void DrawIndexedPlatform(ezUInt32 uiIndexCount, ezUInt32 uiStartIndex) = 0;
  virtual void DrawIndexedInstancedPlatform(ezUInt32 uiIndexCountPerInstance, ezUInt32 uiInstanceCount, ezUInt32 uiStartIndex) = 0;
  virtual void DrawIndexedInstancedIndirectPlatform(const ezRHIBuffer* pIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes) = 0;
  virtual void DrawInstancedPlatform(ezUInt32 uiVertexCountPerInstance, ezUInt32 uiInstanceCount, ezUInt32 uiStartVertex) = 0;
  virtual void DrawInstancedIndirectPlatform(const ezRHIBuffer* pIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes) = 0;
  virtual void DrawAutoPlatform() = 0;

  virtual void BeginStreamOutPlatform() = 0;
  virtual void EndStreamOutPlatform() = 0;

  // State functions

  virtual void SetIndexBufferPlatform(const ezRHIBuffer* pIndexBuffer) = 0;
  virtual void SetVertexBufferPlatform(ezUInt32 uiSlot, const ezRHIBuffer* pVertexBuffer) = 0;
  virtual void SetVertexDeclarationPlatform(const ezRHIVertexDeclaration* pVertexDeclaration) = 0;
  virtual void SetPrimitiveTopologyPlatform(ezRHIPrimitiveTopology::Enum Topology) = 0;

  virtual void SetBlendStatePlatform(const ezRHIBlendState* pBlendState, const ezColor& BlendFactor, ezUInt32 uiSampleMask) = 0;
  virtual void SetDepthStencilStatePlatform(const ezRHIDepthStencilState* pDepthStencilState, ezUInt8 uiStencilRefValue) = 0;
  virtual void SetRasterizerStatePlatform(const ezRHIRasterizerState* pRasterizerState) = 0;

  virtual void SetViewportPlatform(const ezRectFloat& rect, float fMinDepth, float fMaxDepth) = 0;
  virtual void SetScissorRectPlatform(const ezRectU32& rect) = 0;

  virtual void SetStreamOutBufferPlatform(ezUInt32 uiSlot, const ezRHIBuffer* pBuffer, ezUInt32 uiOffset) = 0;
};

class EZ_RHI_DLL ezRHICommandEncoderComputePlatformInterface
{
public:
  // Dispatch

  virtual void DispatchPlatform(ezUInt32 uiThreadGroupCountX, ezUInt32 uiThreadGroupCountY, ezUInt32 uiThreadGroupCountZ) = 0;
  virtual void DispatchIndirectPlatform(const ezRHIBuffer* pIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes) = 0;
};
