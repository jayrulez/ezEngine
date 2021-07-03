
#pragma once

bool ezRHIRenderTargetSetup::HasRenderTargets() const
{
  return m_uiMaxRTIndex != 0xFFu;
}

ezUInt8 ezRHIRenderTargetSetup::GetMaxRenderTargetIndex() const
{
  return m_uiMaxRTIndex;
}

ezRHIRenderTargetViewHandle ezRHIRenderTargetSetup::GetRenderTarget(ezUInt8 uiIndex) const
{
  EZ_ASSERT_DEBUG(uiIndex <= m_uiMaxRTIndex, "Render target index out of range");

  return m_hRTs[uiIndex];
}

ezRHIRenderTargetViewHandle ezRHIRenderTargetSetup::GetDepthStencilTarget() const
{
  return m_hDSTarget;
}
