#include <RHIPCH.h>

#include <RHI/Renderer/CommandEncoder/CommandEncoder.h>
#include <RHI/Renderer/Device/Device.h>
#include <RHI/Renderer/Resources/Buffer.h>
#include <RHI/Renderer/Resources/Query.h>
#include <RHI/Renderer/Resources/RenderTargetView.h>
#include <RHI/Renderer/Resources/ResourceView.h>
#include <RHI/Renderer/Resources/Texture.h>
#include <RHI/Renderer/Resources/UnorderedAccesView.h>

void ezRHICommandEncoder::SetShader(ezRHIShaderHandle hShader)
{
  AssertRenderingThread();
  /// \todo Assert for shader capabilities (supported shader stages etc.)

  if (m_State.m_hShader == hShader)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHIShader* pShader = m_Device.GetShader(hShader);
  EZ_ASSERT_DEV(pShader != nullptr, "The given shader handle isn't valid, this may be a use after destroy!");

  m_CommonImpl.SetShaderPlatform(pShader);

  m_State.m_hShader = hShader;
  CountStateChange();
}

void ezRHICommandEncoder::SetConstantBuffer(ezUInt32 uiSlot, ezRHIBufferHandle hBuffer)
{
  AssertRenderingThread();
  EZ_ASSERT_RELEASE(uiSlot < EZ_GAL_MAX_CONSTANT_BUFFER_COUNT, "Constant buffer slot index too big!");

  if (m_State.m_hConstantBuffers[uiSlot] == hBuffer)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHIBuffer* pBuffer = m_Device.GetBuffer(hBuffer);
  EZ_ASSERT_DEV(pBuffer == nullptr || pBuffer->GetDescription().m_BufferType == ezRHIBufferType::ConstantBuffer, "Wrong buffer type");

  m_CommonImpl.SetConstantBufferPlatform(uiSlot, pBuffer);

  m_State.m_hConstantBuffers[uiSlot] = hBuffer;

  CountStateChange();
}

void ezRHICommandEncoder::SetSamplerState(ezRHIShaderStage::Enum Stage, ezUInt32 uiSlot, ezRHISamplerStateHandle hSamplerState)
{
  AssertRenderingThread();
  EZ_ASSERT_RELEASE(uiSlot < EZ_GAL_MAX_SAMPLER_COUNT, "Sampler state slot index too big!");

  if (m_State.m_hSamplerStates[Stage][uiSlot] == hSamplerState)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHISamplerState* pSamplerState = m_Device.GetSamplerState(hSamplerState);

  m_CommonImpl.SetSamplerStatePlatform(Stage, uiSlot, pSamplerState);

  m_State.m_hSamplerStates[Stage][uiSlot] = hSamplerState;

  CountStateChange();
}

void ezRHICommandEncoder::SetResourceView(ezRHIShaderStage::Enum Stage, ezUInt32 uiSlot, ezRHIResourceViewHandle hResourceView)
{
  AssertRenderingThread();

  /// \todo Check if the device supports the stage / the slot index

  auto& boundResourceViews = m_State.m_hResourceViews[Stage];
  if (uiSlot < boundResourceViews.GetCount() && boundResourceViews[uiSlot] == hResourceView)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHIResourceView* pResourceView = m_Device.GetResourceView(hResourceView);
  if (pResourceView != nullptr)
  {
    if (UnsetUnorderedAccessViews(pResourceView->GetResource()))
    {
      m_CommonImpl.FlushPlatform();
    }
  }

  m_CommonImpl.SetResourceViewPlatform(Stage, uiSlot, pResourceView);

  boundResourceViews.EnsureCount(uiSlot + 1);
  boundResourceViews[uiSlot] = hResourceView;

  auto& boundResources = m_State.m_pResourcesForResourceViews[Stage];
  boundResources.EnsureCount(uiSlot + 1);
  boundResources[uiSlot] = pResourceView != nullptr ? pResourceView->GetResource()->GetParentResource() : nullptr;

  CountStateChange();
}

void ezRHICommandEncoder::SetUnorderedAccessView(ezUInt32 uiSlot, ezRHIUnorderedAccessViewHandle hUnorderedAccessView)
{
  AssertRenderingThread();

  /// \todo Check if the device supports the stage / the slot index

  if (uiSlot < m_State.m_hUnorderedAccessViews.GetCount() && m_State.m_hUnorderedAccessViews[uiSlot] == hUnorderedAccessView)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHIUnorderedAccessView* pUnorderedAccessView = m_Device.GetUnorderedAccessView(hUnorderedAccessView);
  if (pUnorderedAccessView != nullptr)
  {
    if (UnsetResourceViews(pUnorderedAccessView->GetResource()))
    {
      m_CommonImpl.FlushPlatform();
    }
  }

  m_CommonImpl.SetUnorderedAccessViewPlatform(uiSlot, pUnorderedAccessView);

  m_State.m_hUnorderedAccessViews.EnsureCount(uiSlot + 1);
  m_State.m_hUnorderedAccessViews[uiSlot] = hUnorderedAccessView;

  m_State.m_pResourcesForUnorderedAccessViews.EnsureCount(uiSlot + 1);
  m_State.m_pResourcesForUnorderedAccessViews[uiSlot] = pUnorderedAccessView != nullptr ? pUnorderedAccessView->GetResource()->GetParentResource() : nullptr;

  CountStateChange();
}

bool ezRHICommandEncoder::UnsetResourceViews(const ezRHIResourceBase* pResource)
{
  EZ_ASSERT_DEV(pResource->GetParentResource() == pResource, "No proxies allowed");

  bool bResult = false;

  for (ezUInt32 stage = 0; stage < ezRHIShaderStage::ENUM_COUNT; ++stage)
  {
    for (ezUInt32 uiSlot = 0; uiSlot < m_State.m_pResourcesForResourceViews[stage].GetCount(); ++uiSlot)
    {
      if (m_State.m_pResourcesForResourceViews[stage][uiSlot] == pResource)
      {
        m_CommonImpl.SetResourceViewPlatform((ezRHIShaderStage::Enum)stage, uiSlot, nullptr);

        m_State.m_hResourceViews[stage][uiSlot].Invalidate();
        m_State.m_pResourcesForResourceViews[stage][uiSlot] = nullptr;

        bResult = true;
      }
    }
  }

  return bResult;
}

bool ezRHICommandEncoder::UnsetUnorderedAccessViews(const ezRHIResourceBase* pResource)
{
  EZ_ASSERT_DEV(pResource->GetParentResource() == pResource, "No proxies allowed");

  bool bResult = false;

  for (ezUInt32 uiSlot = 0; uiSlot < m_State.m_pResourcesForUnorderedAccessViews.GetCount(); ++uiSlot)
  {
    if (m_State.m_pResourcesForUnorderedAccessViews[uiSlot] == pResource)
    {
      m_CommonImpl.SetUnorderedAccessViewPlatform(uiSlot, nullptr);

      m_State.m_hUnorderedAccessViews[uiSlot].Invalidate();
      m_State.m_pResourcesForUnorderedAccessViews[uiSlot] = nullptr;

      bResult = true;
    }
  }

  return bResult;
}

void ezRHICommandEncoder::InsertFence(ezRHIFenceHandle hFence)
{
  AssertRenderingThread();

  m_CommonImpl.InsertFencePlatform(m_Device.GetFence(hFence));
}

bool ezRHICommandEncoder::IsFenceReached(ezRHIFenceHandle hFence)
{
  AssertRenderingThread();

  return m_CommonImpl.IsFenceReachedPlatform(m_Device.GetFence(hFence));
}

void ezRHICommandEncoder::WaitForFence(ezRHIFenceHandle hFence)
{
  AssertRenderingThread();

  m_CommonImpl.WaitForFencePlatform(m_Device.GetFence(hFence));
}

void ezRHICommandEncoder::BeginQuery(ezRHIQueryHandle hQuery)
{
  AssertRenderingThread();

  auto query = m_Device.GetQuery(hQuery);
  EZ_ASSERT_DEV(!query->m_bStarted, "Can't stat ezRHIQuery because it is already running.");

  m_CommonImpl.BeginQueryPlatform(query);
}

void ezRHICommandEncoder::EndQuery(ezRHIQueryHandle hQuery)
{
  AssertRenderingThread();

  auto query = m_Device.GetQuery(hQuery);
  EZ_ASSERT_DEV(query->m_bStarted, "Can't end ezRHIQuery, query hasn't started yet.");

  m_CommonImpl.EndQueryPlatform(query);
}

ezResult ezRHICommandEncoder::GetQueryResult(ezRHIQueryHandle hQuery, ezUInt64& uiQueryResult)
{
  AssertRenderingThread();

  auto query = m_Device.GetQuery(hQuery);
  EZ_ASSERT_DEV(!query->m_bStarted, "Can't retrieve data from ezRHIQuery while query is still running.");

  return m_CommonImpl.GetQueryResultPlatform(query, uiQueryResult);
}

ezRHITimestampHandle ezRHICommandEncoder::InsertTimestamp()
{
  ezRHITimestampHandle hTimestamp = m_Device.GetTimestamp();

  m_CommonImpl.InsertTimestampPlatform(hTimestamp);

  return hTimestamp;
}

void ezRHICommandEncoder::ClearUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView, ezVec4 clearValues)
{
  AssertRenderingThread();

  const ezRHIUnorderedAccessView* pUnorderedAccessView = m_Device.GetUnorderedAccessView(hUnorderedAccessView);
  if (pUnorderedAccessView == nullptr)
  {
    EZ_REPORT_FAILURE("ClearUnorderedAccessView failed, unordered access view handle invalid.");
    return;
  }

  m_CommonImpl.ClearUnorderedAccessViewPlatform(pUnorderedAccessView, clearValues);
}

void ezRHICommandEncoder::ClearUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView, ezVec4U32 clearValues)
{
  AssertRenderingThread();

  const ezRHIUnorderedAccessView* pUnorderedAccessView = m_Device.GetUnorderedAccessView(hUnorderedAccessView);
  if (pUnorderedAccessView == nullptr)
  {
    EZ_REPORT_FAILURE("ClearUnorderedAccessView failed, unordered access view handle invalid.");
    return;
  }

  m_CommonImpl.ClearUnorderedAccessViewPlatform(pUnorderedAccessView, clearValues);
}

void ezRHICommandEncoder::CopyBuffer(ezRHIBufferHandle hDest, ezRHIBufferHandle hSource)
{
  AssertRenderingThread();

  const ezRHIBuffer* pDest = m_Device.GetBuffer(hDest);
  const ezRHIBuffer* pSource = m_Device.GetBuffer(hSource);

  if (pDest != nullptr && pSource != nullptr)
  {
    m_CommonImpl.CopyBufferPlatform(pDest, pSource);
  }
  else
  {
    EZ_REPORT_FAILURE("CopyBuffer failed, buffer handle invalid - destination = {0}, source = {1}", ezArgP(pDest), ezArgP(pSource));
  }
}

void ezRHICommandEncoder::CopyBufferRegion(
  ezRHIBufferHandle hDest, ezUInt32 uiDestOffset, ezRHIBufferHandle hSource, ezUInt32 uiSourceOffset, ezUInt32 uiByteCount)
{
  AssertRenderingThread();

  const ezRHIBuffer* pDest = m_Device.GetBuffer(hDest);
  const ezRHIBuffer* pSource = m_Device.GetBuffer(hSource);

  if (pDest != nullptr && pSource != nullptr)
  {
    const ezUInt32 uiDestSize = pDest->GetSize();
    const ezUInt32 uiSourceSize = pSource->GetSize();

    EZ_ASSERT_DEV(uiDestSize >= uiDestOffset + uiByteCount, "Destination buffer too small (or offset too big)");
    EZ_ASSERT_DEV(uiSourceSize >= uiSourceOffset + uiByteCount, "Source buffer too small (or offset too big)");

    m_CommonImpl.CopyBufferRegionPlatform(pDest, uiDestOffset, pSource, uiSourceOffset, uiByteCount);
  }
  else
  {
    EZ_REPORT_FAILURE("CopyBuffer failed, buffer handle invalid - destination = {0}, source = {1}", ezArgP(pDest), ezArgP(pSource));
  }
}

void ezRHICommandEncoder::UpdateBuffer(
  ezRHIBufferHandle hDest, ezUInt32 uiDestOffset, ezArrayPtr<const ezUInt8> pSourceData, ezRHIUpdateMode::Enum updateMode)
{
  AssertRenderingThread();

  EZ_VERIFY(!pSourceData.IsEmpty(), "Source data for buffer update is invalid!");

  const ezRHIBuffer* pDest = m_Device.GetBuffer(hDest);

  if (pDest != nullptr)
  {
    if (updateMode == ezRHIUpdateMode::NoOverwrite && !(GetDevice().GetCapabilities().m_bNoOverwriteBufferUpdate))
    {
      updateMode = ezRHIUpdateMode::CopyToTempStorage;
    }

    EZ_VERIFY(pDest->GetSize() >= (uiDestOffset + pSourceData.GetCount()), "Buffer is too small (or offset too big)");
    m_CommonImpl.UpdateBufferPlatform(pDest, uiDestOffset, pSourceData, updateMode);
  }
  else
  {
    EZ_REPORT_FAILURE("UpdateBuffer failed, buffer handle invalid");
  }
}

void ezRHICommandEncoder::CopyTexture(ezRHITextureHandle hDest, ezRHITextureHandle hSource)
{
  AssertRenderingThread();

  const ezRHITexture* pDest = m_Device.GetTexture(hDest);
  const ezRHITexture* pSource = m_Device.GetTexture(hSource);

  if (pDest != nullptr && pSource != nullptr)
  {
    m_CommonImpl.CopyTexturePlatform(pDest, pSource);
  }
  else
  {
    EZ_REPORT_FAILURE("CopyTexture failed, texture handle invalid - destination = {0}, source = {1}", ezArgP(pDest), ezArgP(pSource));
  }
}

void ezRHICommandEncoder::CopyTextureRegion(ezRHITextureHandle hDest, const ezRHITextureSubresource& DestinationSubResource,
  const ezVec3U32& DestinationPoint, ezRHITextureHandle hSource, const ezRHITextureSubresource& SourceSubResource, const ezBoundingBoxu32& Box)
{
  AssertRenderingThread();

  const ezRHITexture* pDest = m_Device.GetTexture(hDest);
  const ezRHITexture* pSource = m_Device.GetTexture(hSource);

  if (pDest != nullptr && pSource != nullptr)
  {
    m_CommonImpl.CopyTextureRegionPlatform(pDest, DestinationSubResource, DestinationPoint, pSource, SourceSubResource, Box);
  }
  else
  {
    EZ_REPORT_FAILURE("CopyTextureRegion failed, texture handle invalid - destination = {0}, source = {1}", ezArgP(pDest), ezArgP(pSource));
  }
}

void ezRHICommandEncoder::UpdateTexture(ezRHITextureHandle hDest, const ezRHITextureSubresource& DestinationSubResource,
  const ezBoundingBoxu32& DestinationBox, const ezRHISystemMemoryDescription& pSourceData)
{
  AssertRenderingThread();

  const ezRHITexture* pDest = m_Device.GetTexture(hDest);

  if (pDest != nullptr)
  {
    m_CommonImpl.UpdateTexturePlatform(pDest, DestinationSubResource, DestinationBox, pSourceData);
  }
  else
  {
    EZ_REPORT_FAILURE("UpdateTexture failed, texture handle invalid - destination = {0}", ezArgP(pDest));
  }
}

void ezRHICommandEncoder::ResolveTexture(ezRHITextureHandle hDest, const ezRHITextureSubresource& DestinationSubResource, ezRHITextureHandle hSource,
  const ezRHITextureSubresource& SourceSubResource)
{
  AssertRenderingThread();

  const ezRHITexture* pDest = m_Device.GetTexture(hDest);
  const ezRHITexture* pSource = m_Device.GetTexture(hSource);

  if (pDest != nullptr && pSource != nullptr)
  {
    m_CommonImpl.ResolveTexturePlatform(pDest, DestinationSubResource, pSource, SourceSubResource);
  }
  else
  {
    EZ_REPORT_FAILURE("ResolveTexture failed, texture handle invalid - destination = {0}, source = {1}", ezArgP(pDest), ezArgP(pSource));
  }
}

void ezRHICommandEncoder::ReadbackTexture(ezRHITextureHandle hTexture)
{
  AssertRenderingThread();

  const ezRHITexture* pTexture = m_Device.GetTexture(hTexture);

  if (pTexture != nullptr)
  {
    EZ_ASSERT_RELEASE(pTexture->GetDescription().m_ResourceAccess.m_bReadBack,
      "A texture supplied to read-back needs to be created with the correct resource usage (m_bReadBack = true)!");

    m_CommonImpl.ReadbackTexturePlatform(pTexture);
  }
}

void ezRHICommandEncoder::CopyTextureReadbackResult(ezRHITextureHandle hTexture, ezArrayPtr<ezRHITextureSubresource> SourceSubResource, ezArrayPtr<ezRHISystemMemoryDescription> TargetData)
{
  AssertRenderingThread();

  const ezRHITexture* pTexture = m_Device.GetTexture(hTexture);

  if (pTexture != nullptr)
  {
    EZ_ASSERT_RELEASE(pTexture->GetDescription().m_ResourceAccess.m_bReadBack,
      "A texture supplied to read-back needs to be created with the correct resource usage (m_bReadBack = true)!");

    m_CommonImpl.CopyTextureReadbackResultPlatform(pTexture, SourceSubResource, TargetData);
  }
}

void ezRHICommandEncoder::GenerateMipMaps(ezRHIResourceViewHandle hResourceView)
{
  AssertRenderingThread();

  const ezRHIResourceView* pResourceView = m_Device.GetResourceView(hResourceView);
  if (pResourceView != nullptr)
  {
    EZ_ASSERT_DEV(!pResourceView->GetDescription().m_hTexture.IsInvalidated(), "Resource view needs a valid texture to generate mip maps.");
    const ezRHITexture* pTexture = m_Device.GetTexture(pResourceView->GetDescription().m_hTexture);
    EZ_ASSERT_DEV(pTexture->GetDescription().m_bAllowDynamicMipGeneration,
      "Dynamic mip map generation needs to be enabled (m_bAllowDynamicMipGeneration = true)!");

    m_CommonImpl.GenerateMipMapsPlatform(pResourceView);
  }
}

void ezRHICommandEncoder::Flush()
{
  AssertRenderingThread();

  m_CommonImpl.FlushPlatform();
}

// Debug helper functions

void ezRHICommandEncoder::PushMarker(const char* Marker)
{
  AssertRenderingThread();

  EZ_ASSERT_DEV(Marker != nullptr, "Invalid marker!");

  m_CommonImpl.PushMarkerPlatform(Marker);
}

void ezRHICommandEncoder::PopMarker()
{
  AssertRenderingThread();

  m_CommonImpl.PopMarkerPlatform();
}

void ezRHICommandEncoder::InsertEventMarker(const char* Marker)
{
  AssertRenderingThread();

  EZ_ASSERT_DEV(Marker != nullptr, "Invalid marker!");

  m_CommonImpl.InsertEventMarkerPlatform(Marker);
}

void ezRHICommandEncoder::ClearStatisticsCounters()
{
  // Reset counters for various statistics
  m_uiStateChanges = 0;
  m_uiRedundantStateChanges = 0;
}

ezRHICommandEncoder::ezRHICommandEncoder(ezRHIDevice& device, ezRHICommandEncoderState& state, ezRHICommandEncoderCommonPlatformInterface& commonImpl)
  : m_Device(device)
  , m_State(state)
  , m_CommonImpl(commonImpl)
{
}

ezRHICommandEncoder::~ezRHICommandEncoder() = default;

void ezRHICommandEncoder::InvalidateState()
{
  m_State.InvalidateState();
}
