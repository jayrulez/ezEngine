
#pragma once

#include <Foundation/Math/Color.h>
#include <Foundation/Math/Rect.h>
#include <RHI/Renderer/Basics.h>

struct EZ_RHI_DLL ezRHICommandEncoderState
{
  virtual void InvalidateState();

  ezRHIShaderHandle m_hShader;

  ezRHIBufferHandle m_hConstantBuffers[EZ_GAL_MAX_CONSTANT_BUFFER_COUNT];

  ezHybridArray<ezRHIResourceViewHandle, 16> m_hResourceViews[ezRHIShaderStage::ENUM_COUNT];
  ezHybridArray<const ezRHIResourceBase*, 16> m_pResourcesForResourceViews[ezRHIShaderStage::ENUM_COUNT];

  ezHybridArray<ezRHIUnorderedAccessViewHandle, 16> m_hUnorderedAccessViews;
  ezHybridArray<const ezRHIResourceBase*, 16> m_pResourcesForUnorderedAccessViews;

  ezRHISamplerStateHandle m_hSamplerStates[ezRHIShaderStage::ENUM_COUNT][EZ_GAL_MAX_SAMPLER_COUNT];
};

struct EZ_RHI_DLL ezRHICommandEncoderRenderState : public ezRHICommandEncoderState
{
  virtual void InvalidateState() override;

  ezRHIBufferHandle m_hVertexBuffers[EZ_GAL_MAX_VERTEX_BUFFER_COUNT];
  ezRHIBufferHandle m_hIndexBuffer;

  ezRHIVertexDeclarationHandle m_hVertexDeclaration;
  ezRHIPrimitiveTopology::Enum m_Topology = ezRHIPrimitiveTopology::ENUM_COUNT;

  ezRHIBlendStateHandle m_hBlendState;
  ezColor m_BlendFactor = ezColor(0, 0, 0, 0);
  ezUInt32 m_uiSampleMask = 0;

  ezRHIDepthStencilStateHandle m_hDepthStencilState;
  ezUInt8 m_uiStencilRefValue = 0;

  ezRHIRasterizerStateHandle m_hRasterizerState;

  ezRectU32 m_ScissorRect = ezRectU32(0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
  ezRectFloat m_ViewPortRect = ezRectFloat(ezMath::MaxValue<float>(), ezMath::MaxValue<float>(), 0.0f, 0.0f);
  float m_fViewPortMinDepth = ezMath::MaxValue<float>();
  float m_fViewPortMaxDepth = -ezMath::MaxValue<float>();
};
