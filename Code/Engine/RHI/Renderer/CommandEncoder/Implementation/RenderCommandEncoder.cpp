#include <RHIPCH.h>

#include <RHI/Renderer/CommandEncoder/RenderCommandEncoder.h>
#include <RHI/Renderer/Device/RenderDevice.h>
#include <RHI/Renderer/Resources/Buffer.h>
#include <RHI/Renderer/Resources/Query.h>
#include <RHI/Renderer/Resources/RenderTargetView.h>
#include <RHI/Renderer/Resources/ResourceView.h>
#include <RHI/Renderer/Resources/Texture.h>
#include <RHI/Renderer/Resources/UnorderedAccesView.h>

ezRHIRenderCommandEncoder::ezRHIRenderCommandEncoder(ezRHIRenderDevice& device, ezRHICommandEncoderRenderState& renderState)
  : ezRHICommandEncoder(device, renderState)
  , m_RenderState(renderState)
{
}

ezRHIRenderCommandEncoder::~ezRHIRenderCommandEncoder() = default;

void ezRHIRenderCommandEncoder::Clear(const ezColor& ClearColor, ezUInt32 uiRenderTargetClearMask /*= 0xFFFFFFFFu*/, bool bClearDepth /*= true*/, bool bClearStencil /*= true*/, float fDepthClear /*= 1.0f*/, ezUInt8 uiStencilClear /*= 0x0u*/)
{
  AssertRenderingThread();

  //m_RenderImpl.ClearPlatform(ClearColor, uiRenderTargetClearMask, bClearDepth, bClearStencil, fDepthClear, uiStencilClear);
}

void ezRHIRenderCommandEncoder::Draw(ezUInt32 uiVertexCount, ezUInt32 uiStartVertex)
{
  AssertRenderingThread();

  /// \todo If platform indicates that non-indexed rendering is not possible bind a helper index buffer which contains continuous indices
  /// (0, 1, 2, ..)

 // m_RenderImpl.DrawPlatform(uiVertexCount, uiStartVertex);

  CountDrawCall();
}

void ezRHIRenderCommandEncoder::DrawIndexed(ezUInt32 uiIndexCount, ezUInt32 uiStartIndex)
{
  AssertRenderingThread();

  //m_RenderImpl.DrawIndexedPlatform(uiIndexCount, uiStartIndex);

  CountDrawCall();
}

void ezRHIRenderCommandEncoder::DrawIndexedInstanced(ezUInt32 uiIndexCountPerInstance, ezUInt32 uiInstanceCount, ezUInt32 uiStartIndex)
{
  AssertRenderingThread();
  /// \todo Assert for instancing

  //m_RenderImpl.DrawIndexedInstancedPlatform(uiIndexCountPerInstance, uiInstanceCount, uiStartIndex);

  CountDrawCall();
}

void ezRHIRenderCommandEncoder::DrawIndexedInstancedIndirect(ezRHIBufferHandle hIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes)
{
  AssertRenderingThread();
  /// \todo Assert for instancing
  /// \todo Assert for indirect draw
  /// \todo Assert offset < buffer size

  const ezRHIBuffer* pBuffer = GetDevice().GetBuffer(hIndirectArgumentBuffer);
  EZ_ASSERT_DEV(pBuffer != nullptr, "Invalid buffer handle for indirect arguments!");

  /// \todo Assert that the buffer can be used for indirect arguments (flag in desc)
  //m_RenderImpl.DrawIndexedInstancedIndirectPlatform(pBuffer, uiArgumentOffsetInBytes);

  CountDrawCall();
}

void ezRHIRenderCommandEncoder::DrawInstanced(ezUInt32 uiVertexCountPerInstance, ezUInt32 uiInstanceCount, ezUInt32 uiStartVertex)
{
  AssertRenderingThread();
  /// \todo Assert for instancing

  /// \todo If platform indicates that non-indexed rendering is not possible bind a helper index buffer which contains continuous indices
  /// (0, 1, 2, ..)

  //m_RenderImpl.DrawInstancedPlatform(uiVertexCountPerInstance, uiInstanceCount, uiStartVertex);

  CountDrawCall();
}

void ezRHIRenderCommandEncoder::DrawInstancedIndirect(ezRHIBufferHandle hIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes)
{
  AssertRenderingThread();
  /// \todo Assert for instancing
  /// \todo Assert for indirect draw
  /// \todo Assert offset < buffer size

  const ezRHIBuffer* pBuffer = GetDevice().GetBuffer(hIndirectArgumentBuffer);
  EZ_ASSERT_DEV(pBuffer != nullptr, "Invalid buffer handle for indirect arguments!");

  /// \todo Assert that the buffer can be used for indirect arguments (flag in desc)
  //m_RenderImpl.DrawInstancedIndirectPlatform(pBuffer, uiArgumentOffsetInBytes);

  CountDrawCall();
}

void ezRHIRenderCommandEncoder::DrawAuto()
{
  AssertRenderingThread();
  /// \todo Assert for draw auto support

  //m_RenderImpl.DrawAutoPlatform();

  CountDrawCall();
}

void ezRHIRenderCommandEncoder::BeginStreamOut()
{
  AssertRenderingThread();
  /// \todo Assert for streamout support

  //m_RenderImpl.BeginStreamOutPlatform();
}

void ezRHIRenderCommandEncoder::EndStreamOut()
{
  AssertRenderingThread();

  //m_RenderImpl.EndStreamOutPlatform();
}

void ezRHIRenderCommandEncoder::SetIndexBuffer(ezRHIBufferHandle hIndexBuffer)
{
  if (m_RenderState.m_hIndexBuffer == hIndexBuffer)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHIBuffer* pBuffer = GetDevice().GetBuffer(hIndexBuffer);
  /// \todo Assert on index buffer type (if non nullptr)
  // Note that GL4 can bind arbitrary buffer to arbitrary binding points (index/vertex/transform-feedback/indirect-draw/...)

  //m_RenderImpl.SetIndexBufferPlatform(pBuffer);

  m_RenderState.m_hIndexBuffer = hIndexBuffer;
  CountStateChange();
}

void ezRHIRenderCommandEncoder::SetVertexBuffer(ezUInt32 uiSlot, ezRHIBufferHandle hVertexBuffer)
{
  if (m_RenderState.m_hVertexBuffers[uiSlot] == hVertexBuffer)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHIBuffer* pBuffer = GetDevice().GetBuffer(hVertexBuffer);
  // Assert on vertex buffer type (if non-zero)
  // Note that GL4 can bind arbitrary buffer to arbitrary binding points (index/vertex/transform-feedback/indirect-draw/...)

  //m_RenderImpl.SetVertexBufferPlatform(uiSlot, pBuffer);

  m_RenderState.m_hVertexBuffers[uiSlot] = hVertexBuffer;
  CountStateChange();
}

void ezRHIRenderCommandEncoder::SetPrimitiveTopology(ezRHIPrimitiveTopology::Enum Topology)
{
  AssertRenderingThread();

  if (m_RenderState.m_Topology == Topology)
  {
    CountRedundantStateChange();
    return;
  }

  //m_RenderImpl.SetPrimitiveTopologyPlatform(Topology);

  m_RenderState.m_Topology = Topology;

  CountStateChange();
}

void ezRHIRenderCommandEncoder::SetVertexDeclaration(ezRHIVertexDeclarationHandle hVertexDeclaration)
{
  AssertRenderingThread();

  if (m_RenderState.m_hVertexDeclaration == hVertexDeclaration)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHIVertexDeclaration* pVertexDeclaration = GetDevice().GetVertexDeclaration(hVertexDeclaration);
  // Assert on vertex buffer type (if non-zero)

  //m_RenderImpl.SetVertexDeclarationPlatform(pVertexDeclaration);

  m_RenderState.m_hVertexDeclaration = hVertexDeclaration;

  CountStateChange();
}

void ezRHIRenderCommandEncoder::SetBlendState(ezRHIBlendStateHandle hBlendState, const ezColor& BlendFactor, ezUInt32 uiSampleMask)
{
  AssertRenderingThread();

  if (m_RenderState.m_hBlendState == hBlendState && m_RenderState.m_BlendFactor.IsEqualRGBA(BlendFactor, 0.001f) && m_RenderState.m_uiSampleMask == uiSampleMask)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHIBlendState* pBlendState = GetDevice().GetBlendState(hBlendState);

  //m_RenderImpl.SetBlendStatePlatform(pBlendState, BlendFactor, uiSampleMask);

  m_RenderState.m_hBlendState = hBlendState;
  m_RenderState.m_BlendFactor = BlendFactor;
  m_RenderState.m_uiSampleMask = uiSampleMask;

  CountStateChange();
}

void ezRHIRenderCommandEncoder::SetDepthStencilState(ezRHIDepthStencilStateHandle hDepthStencilState, ezUInt8 uiStencilRefValue /*= 0xFFu*/)
{
  AssertRenderingThread();

  if (m_RenderState.m_hDepthStencilState == hDepthStencilState && m_RenderState.m_uiStencilRefValue == uiStencilRefValue)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHIDepthStencilState* pDepthStencilState = GetDevice().GetDepthStencilState(hDepthStencilState);

  //m_RenderImpl.SetDepthStencilStatePlatform(pDepthStencilState, uiStencilRefValue);

  m_RenderState.m_hDepthStencilState = hDepthStencilState;
  m_RenderState.m_uiStencilRefValue = uiStencilRefValue;

  CountStateChange();
}

void ezRHIRenderCommandEncoder::SetRasterizerState(ezRHIRasterizerStateHandle hRasterizerState)
{
  AssertRenderingThread();

  if (m_RenderState.m_hRasterizerState == hRasterizerState)
  {
    CountRedundantStateChange();
    return;
  }

  const ezRHIRasterizerState* pRasterizerState = GetDevice().GetRasterizerState(hRasterizerState);

  //m_RenderImpl.SetRasterizerStatePlatform(pRasterizerState);

  m_RenderState.m_hRasterizerState = hRasterizerState;

  CountStateChange();
}

void ezRHIRenderCommandEncoder::SetViewport(const ezRectFloat& rect, float fMinDepth, float fMaxDepth)
{
  AssertRenderingThread();

  if (m_RenderState.m_ViewPortRect == rect && m_RenderState.m_fViewPortMinDepth == fMinDepth && m_RenderState.m_fViewPortMaxDepth == fMaxDepth)
  {
    CountRedundantStateChange();
    return;
  }

  //m_RenderImpl.SetViewportPlatform(rect, fMinDepth, fMaxDepth);

  m_RenderState.m_ViewPortRect = rect;
  m_RenderState.m_fViewPortMinDepth = fMinDepth;
  m_RenderState.m_fViewPortMaxDepth = fMaxDepth;

  CountStateChange();
}

void ezRHIRenderCommandEncoder::SetScissorRect(const ezRectU32& rect)
{
  AssertRenderingThread();

  if (m_RenderState.m_ScissorRect == rect)
  {
    CountRedundantStateChange();
    return;
  }

  //m_RenderImpl.SetScissorRectPlatform(rect);

  m_RenderState.m_ScissorRect = rect;

  CountStateChange();
}

void ezRHIRenderCommandEncoder::SetStreamOutBuffer(ezUInt32 uiSlot, ezRHIBufferHandle hBuffer, ezUInt32 uiOffset)
{
  EZ_ASSERT_NOT_IMPLEMENTED;

  CountStateChange();
}

void ezRHIRenderCommandEncoder::ClearStatisticsCounters()
{
  ezRHICommandEncoder::ClearStatisticsCounters();

  m_uiDrawCalls = 0;
}
