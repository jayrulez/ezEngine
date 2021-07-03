
#pragma once

#include <Foundation/Threading/ThreadUtils.h>
#include <RHI/Renderer/CommandEncoder/CommandEncoderState.h>

class EZ_RHI_DLL ezRHICommandEncoder
{
  EZ_DISALLOW_COPY_AND_ASSIGN(ezRHICommandEncoder);

public:
  // State setting functions

  void SetShader(ezRHIShaderHandle hShader);

  void SetConstantBuffer(ezUInt32 uiSlot, ezRHIBufferHandle hBuffer);
  void SetSamplerState(ezRHIShaderStage::Enum Stage, ezUInt32 uiSlot, ezRHISamplerStateHandle hSamplerState);
  void SetResourceView(ezRHIShaderStage::Enum Stage, ezUInt32 uiSlot, ezRHIResourceViewHandle hResourceView);
  void SetUnorderedAccessView(ezUInt32 uiSlot, ezRHIUnorderedAccessViewHandle hUnorderedAccessView);

  // Returns whether a resource view has been unset for the given resource
  bool UnsetResourceViews(const ezRHIResourceBase* pResource);
  // Returns whether a unordered access view has been unset for the given resource
  bool UnsetUnorderedAccessViews(const ezRHIResourceBase* pResource);

  // Fence & Query functions

  void InsertFence(ezRHIFenceHandle hFence);
  bool IsFenceReached(ezRHIFenceHandle hFence);
  void WaitForFence(ezRHIFenceHandle hFence);

  void BeginQuery(ezRHIQueryHandle hQuery);
  void EndQuery(ezRHIQueryHandle hQuery);

  /// \return Success if retrieving the query succeeded.
  ezResult GetQueryResult(ezRHIQueryHandle hQuery, ezUInt64& uiQueryResult);

  // Timestamp functions

  ezRHITimestampHandle InsertTimestamp();

  // Resource functions

  /// Clears an unordered access view with a float value.
  void ClearUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView, ezVec4 clearValues);

  /// Clears an unordered access view with an int value.
  void ClearUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView, ezVec4U32 clearValues);

  void CopyBuffer(ezRHIBufferHandle hDest, ezRHIBufferHandle hSource);
  void CopyBufferRegion(ezRHIBufferHandle hDest, ezUInt32 uiDestOffset, ezRHIBufferHandle hSource, ezUInt32 uiSourceOffset, ezUInt32 uiByteCount);
  void UpdateBuffer(ezRHIBufferHandle hDest, ezUInt32 uiDestOffset, ezArrayPtr<const ezUInt8> pSourceData, ezRHIUpdateMode::Enum updateMode = ezRHIUpdateMode::Discard);

  void CopyTexture(ezRHITextureHandle hDest, ezRHITextureHandle hSource);
  void CopyTextureRegion(ezRHITextureHandle hDest, const ezRHITextureSubresource& DestinationSubResource, const ezVec3U32& DestinationPoint, ezRHITextureHandle hSource, const ezRHITextureSubresource& SourceSubResource, const ezBoundingBoxu32& Box);

  void UpdateTexture(ezRHITextureHandle hDest, const ezRHITextureSubresource& DestinationSubResource, const ezBoundingBoxu32& DestinationBox, const ezRHISystemMemoryDescription& pSourceData);

  void ResolveTexture(ezRHITextureHandle hDest, const ezRHITextureSubresource& DestinationSubResource, ezRHITextureHandle hSource, const ezRHITextureSubresource& SourceSubResource);

  void ReadbackTexture(ezRHITextureHandle hTexture);
  void CopyTextureReadbackResult(ezRHITextureHandle hTexture, ezArrayPtr<ezRHITextureSubresource> SourceSubResource, ezArrayPtr<ezRHISystemMemoryDescription> TargetData);

  void GenerateMipMaps(ezRHIResourceViewHandle hResourceView);

  // Misc

  void Flush();

  // Debug helper functions

  void PushMarker(const char* Marker);
  void PopMarker();
  void InsertEventMarker(const char* Marker);

  virtual void ClearStatisticsCounters();

  EZ_ALWAYS_INLINE ezRHIRenderDevice& GetDevice() { return m_Device; }

protected:
  friend class ezRHIRenderDeviceImpl;

  ezRHICommandEncoder(ezRHIRenderDevice& device, ezRHICommandEncoderState& state);
  virtual ~ezRHICommandEncoder();

  // Don't use light hearted ;)
  void InvalidateState();

  void AssertRenderingThread()
  {
    EZ_ASSERT_DEV(ezThreadUtils::IsMainThread(), "This function can only be executed on the main thread.");
  }

  void CountStateChange() { m_uiStateChanges++; }
  void CountRedundantStateChange() { m_uiRedundantStateChanges++; }

private:
  friend class ezMemoryUtils;

  // Parent Device
  ezRHIRenderDevice& m_Device;

  // Statistic variables
  ezUInt32 m_uiStateChanges = 0;
  ezUInt32 m_uiRedundantStateChanges = 0;

  ezRHICommandEncoderState& m_State;
};
