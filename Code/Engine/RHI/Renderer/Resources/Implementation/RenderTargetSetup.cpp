#include <RHIPCH.h>

#include <RHI/Renderer/Device/RenderDevice.h>
#include <RHI/Renderer/Resources/RenderTargetSetup.h>

ezRHIRenderTargetSetup::ezRHIRenderTargetSetup()
  : m_uiMaxRTIndex(0xFFu)
{
}

ezRHIRenderTargetSetup& ezRHIRenderTargetSetup::SetRenderTarget(ezUInt8 uiIndex, ezRHIRenderTargetViewHandle hRenderTarget)
{
  EZ_ASSERT_DEV(uiIndex < EZ_RHI_MAX_RENDERTARGET_COUNT, "Render target index out of bounds - should be less than EZ_RHI_MAX_RENDERTARGET_COUNT");

  m_hRTs[uiIndex] = hRenderTarget;

  if (!HasRenderTargets())
  {
    m_uiMaxRTIndex = uiIndex;
  }
  else
  {
    m_uiMaxRTIndex = ezMath::Max(m_uiMaxRTIndex, uiIndex);
  }

  return *this;
}

ezRHIRenderTargetSetup& ezRHIRenderTargetSetup::SetDepthStencilTarget(ezRHIRenderTargetViewHandle hDSTarget)
{
  m_hDSTarget = hDSTarget;

  return *this;
}

bool ezRHIRenderTargetSetup::operator==(const ezRHIRenderTargetSetup& other) const
{
  if (m_hDSTarget != other.m_hDSTarget)
    return false;

  if (m_uiMaxRTIndex != other.m_uiMaxRTIndex)
    return false;

  for (ezUInt8 uiRTIndex = 0; uiRTIndex <= m_uiMaxRTIndex; ++uiRTIndex)
  {
    if (m_hRTs[uiRTIndex] != other.m_hRTs[uiRTIndex])
      return false;
  }

  return true;
}

bool ezRHIRenderTargetSetup::operator!=(const ezRHIRenderTargetSetup& other) const
{
  return !(*this == other);
}

void ezRHIRenderTargetSetup::DestroyAllAttachedViews()
{
  ezRHIRenderDevice* pDevice = ezRHIRenderDevice::GetDefaultDevice();

  ezArrayPtr<ezRHIRenderTargetViewHandle> colorViews(m_hRTs);
  for (ezRHIRenderTargetViewHandle& hView : colorViews)
  {
    if (!hView.IsInvalidated())
    {
      pDevice->DestroyRenderTargetView(hView);
      hView.Invalidate();
    }
  }

  if (!m_hDSTarget.IsInvalidated())
  {
    pDevice->DestroyRenderTargetView(m_hDSTarget);
    m_hDSTarget.Invalidate();
  }
  m_uiMaxRTIndex = 0xFFu;
}

EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Resources_Implementation_RenderTargetSetup);
