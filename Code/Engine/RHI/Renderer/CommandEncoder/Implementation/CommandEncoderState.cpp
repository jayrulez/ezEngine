#include <RHIPCH.h>

#include <RHI/Renderer/CommandEncoder/CommandEncoderState.h>

void ezRHICommandEncoderState::InvalidateState()
{
  m_hShader = ezRHIShaderHandle();

  for (ezUInt32 i = 0; i < EZ_ARRAY_SIZE(m_hConstantBuffers); ++i)
  {
    m_hConstantBuffers[i].Invalidate();
  }

  for (ezUInt32 i = 0; i < ezRHIShaderStage::ENUM_COUNT; ++i)
  {
    m_hResourceViews[i].Clear();
    m_pResourcesForResourceViews[i].Clear();
  }

  m_hUnorderedAccessViews.Clear();
  m_pResourcesForUnorderedAccessViews.Clear();

  for (ezUInt32 i = 0; i < ezRHIShaderStage::ENUM_COUNT; ++i)
  {
    for (ezUInt32 j = 0; j < EZ_RHI_MAX_SAMPLER_COUNT; j++)
    {
      m_hSamplerStates[i][j].Invalidate();
    }
  }
}

void ezRHICommandEncoderRenderState::InvalidateState()
{
  ezRHICommandEncoderState::InvalidateState();

  for (ezUInt32 i = 0; i < EZ_ARRAY_SIZE(m_hVertexBuffers); ++i)
  {
    m_hVertexBuffers[i].Invalidate();
  }
  m_hIndexBuffer.Invalidate();

  m_hVertexDeclaration.Invalidate();
  m_Topology = ezRHIPrimitiveTopology::ENUM_COUNT;

  m_hBlendState.Invalidate();
  m_BlendFactor = ezColor(0, 0, 0, 0);
  m_uiSampleMask = 0;

  m_hDepthStencilState.Invalidate();
  m_uiStencilRefValue = 0;

  m_hRasterizerState.Invalidate();

  m_ScissorRect = ezRectU32(0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
  m_ViewPortRect = ezRectFloat(ezMath::MaxValue<float>(), ezMath::MaxValue<float>(), 0.0f, 0.0f);
  m_fViewPortMinDepth = ezMath::MaxValue<float>();
  m_fViewPortMaxDepth = -ezMath::MaxValue<float>();
}
