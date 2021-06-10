#include <RendererVkPCH.h>

#include <RendererVk/CommandEncoder/CommandEncoderImplVk.h>
#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Resources/BufferVk.h>
#include <RendererVk/Resources/FenceVk.h>
#include <RendererVk/Resources/QueryVk.h>
#include <RendererVk/Resources/RenderTargetViewVk.h>
#include <RendererVk/Resources/ResourceViewVk.h>
#include <RendererVk/Resources/TextureVk.h>
#include <RendererVk/Resources/UnorderedAccessViewVk.h>
#include <RendererVk/Shader/ShaderVk.h>
#include <RendererVk/Shader/VertexDeclarationVk.h>
#include <RendererVk/State/StateVk.h>
#include <RendererFoundation/CommandEncoder/CommandEncoder.h>


ezGALCommandEncoderImplVk::ezGALCommandEncoderImplVk(ezGALDeviceVk& deviceVk)
  : m_GALDeviceVk(deviceVk)
{

}

ezGALCommandEncoderImplVk::~ezGALCommandEncoderImplVk()
{
}

// State setting functions

void ezGALCommandEncoderImplVk::SetShaderPlatform(const ezGALShader* pShader)
{

}

void ezGALCommandEncoderImplVk::SetConstantBufferPlatform(ezUInt32 uiSlot, const ezGALBuffer* pBuffer)
{
}

void ezGALCommandEncoderImplVk::SetSamplerStatePlatform(ezGALShaderStage::Enum Stage, ezUInt32 uiSlot, const ezGALSamplerState* pSamplerState)
{
}

void ezGALCommandEncoderImplVk::SetResourceViewPlatform(ezGALShaderStage::Enum Stage, ezUInt32 uiSlot, const ezGALResourceView* pResourceView)
{
}

void ezGALCommandEncoderImplVk::SetUnorderedAccessViewPlatform(ezUInt32 uiSlot, const ezGALUnorderedAccessView* pUnorderedAccessView)
{
}

// Fence & Query functions

void ezGALCommandEncoderImplVk::InsertFencePlatform(const ezGALFence* pFence)
{
}

bool ezGALCommandEncoderImplVk::IsFenceReachedPlatform(const ezGALFence* pFence)
{
  return false;
}

void ezGALCommandEncoderImplVk::WaitForFencePlatform(const ezGALFence* pFence)
{
}

void ezGALCommandEncoderImplVk::BeginQueryPlatform(const ezGALQuery* pQuery)
{
}

void ezGALCommandEncoderImplVk::EndQueryPlatform(const ezGALQuery* pQuery)
{
}

ezResult ezGALCommandEncoderImplVk::GetQueryResultPlatform(const ezGALQuery* pQuery, ezUInt64& uiQueryResult)
{
  return EZ_SUCCESS;
}

// Timestamp functions

void ezGALCommandEncoderImplVk::InsertTimestampPlatform(ezGALTimestampHandle hTimestamp)
{
}

// Resource update functions

void ezGALCommandEncoderImplVk::ClearUnorderedAccessViewPlatform(const ezGALUnorderedAccessView* pUnorderedAccessView, ezVec4 clearValues)
{
}

void ezGALCommandEncoderImplVk::ClearUnorderedAccessViewPlatform(const ezGALUnorderedAccessView* pUnorderedAccessView, ezVec4U32 clearValues)
{
}

void ezGALCommandEncoderImplVk::CopyBufferPlatform(const ezGALBuffer* pDestination, const ezGALBuffer* pSource)
{
}

void ezGALCommandEncoderImplVk::CopyBufferRegionPlatform(const ezGALBuffer* pDestination, ezUInt32 uiDestOffset, const ezGALBuffer* pSource, ezUInt32 uiSourceOffset, ezUInt32 uiByteCount)
{
}

void ezGALCommandEncoderImplVk::UpdateBufferPlatform(const ezGALBuffer* pDestination, ezUInt32 uiDestOffset, ezArrayPtr<const ezUInt8> pSourceData, ezGALUpdateMode::Enum updateMode)
{
}

void ezGALCommandEncoderImplVk::CopyTexturePlatform(const ezGALTexture* pDestination, const ezGALTexture* pSource)
{
}

void ezGALCommandEncoderImplVk::CopyTextureRegionPlatform(const ezGALTexture* pDestination, const ezGALTextureSubresource& DestinationSubResource,
  const ezVec3U32& DestinationPoint, const ezGALTexture* pSource, const ezGALTextureSubresource& SourceSubResource, const ezBoundingBoxu32& Box)
{
}

void ezGALCommandEncoderImplVk::UpdateTexturePlatform(const ezGALTexture* pDestination, const ezGALTextureSubresource& DestinationSubResource,
  const ezBoundingBoxu32& DestinationBox, const ezGALSystemMemoryDescription& pSourceData)
{
}

void ezGALCommandEncoderImplVk::ResolveTexturePlatform(const ezGALTexture* pDestination, const ezGALTextureSubresource& DestinationSubResource,
  const ezGALTexture* pSource, const ezGALTextureSubresource& SourceSubResource)
{
}

void ezGALCommandEncoderImplVk::ReadbackTexturePlatform(const ezGALTexture* pTexture)
{
}

ezUInt32 GetMipSize(ezUInt32 uiSize, ezUInt32 uiMipLevel)
{
  return 0;
}

void ezGALCommandEncoderImplVk::CopyTextureReadbackResultPlatform(const ezGALTexture* pTexture, ezArrayPtr<ezGALTextureSubresource> SourceSubResource, ezArrayPtr<ezGALSystemMemoryDescription> TargetData)
{
}

void ezGALCommandEncoderImplVk::GenerateMipMapsPlatform(const ezGALResourceView* pResourceView)
{
}

void ezGALCommandEncoderImplVk::FlushPlatform()
{
}

// Debug helper functions

void ezGALCommandEncoderImplVk::PushMarkerPlatform(const char* szMarker)
{
}

void ezGALCommandEncoderImplVk::PopMarkerPlatform()
{
}

void ezGALCommandEncoderImplVk::InsertEventMarkerPlatform(const char* szMarker)
{
}

//////////////////////////////////////////////////////////////////////////

void ezGALCommandEncoderImplVk::BeginRendering(const ezGALRenderingSetup& renderingSetup)
{

}

// Draw functions

void ezGALCommandEncoderImplVk::ClearPlatform(const ezColor& ClearColor, ezUInt32 uiRenderTargetClearMask, bool bClearDepth, bool bClearStencil, float fDepthClear, ezUInt8 uiStencilClear)
{

}

void ezGALCommandEncoderImplVk::DrawPlatform(ezUInt32 uiVertexCount, ezUInt32 uiStartVertex)
{
}

void ezGALCommandEncoderImplVk::DrawIndexedPlatform(ezUInt32 uiIndexCount, ezUInt32 uiStartIndex)
{
}

void ezGALCommandEncoderImplVk::DrawIndexedInstancedPlatform(ezUInt32 uiIndexCountPerInstance, ezUInt32 uiInstanceCount, ezUInt32 uiStartIndex)
{
}

void ezGALCommandEncoderImplVk::DrawIndexedInstancedIndirectPlatform(const ezGALBuffer* pIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes)
{
}

void ezGALCommandEncoderImplVk::DrawInstancedPlatform(ezUInt32 uiVertexCountPerInstance, ezUInt32 uiInstanceCount, ezUInt32 uiStartVertex)
{
}

void ezGALCommandEncoderImplVk::DrawInstancedIndirectPlatform(const ezGALBuffer* pIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes)
{
}

void ezGALCommandEncoderImplVk::DrawAutoPlatform()
{
}

void ezGALCommandEncoderImplVk::BeginStreamOutPlatform()
{
}

void ezGALCommandEncoderImplVk::EndStreamOutPlatform()
{
}

void ezGALCommandEncoderImplVk::SetIndexBufferPlatform(const ezGALBuffer* pIndexBuffer)
{
}

void ezGALCommandEncoderImplVk::SetVertexBufferPlatform(ezUInt32 uiSlot, const ezGALBuffer* pVertexBuffer)
{
}

void ezGALCommandEncoderImplVk::SetVertexDeclarationPlatform(const ezGALVertexDeclaration* pVertexDeclaration)
{
}

/*
static const D3D11_PRIMITIVE_TOPOLOGY GALTopologyToVk[ezGALPrimitiveTopology::ENUM_COUNT] = {
  D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
  D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
  D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
};
*/

void ezGALCommandEncoderImplVk::SetPrimitiveTopologyPlatform(ezGALPrimitiveTopology::Enum Topology)
{
}

void ezGALCommandEncoderImplVk::SetBlendStatePlatform(const ezGALBlendState* pBlendState, const ezColor& BlendFactor, ezUInt32 uiSampleMask)
{
}

void ezGALCommandEncoderImplVk::SetDepthStencilStatePlatform(const ezGALDepthStencilState* pDepthStencilState, ezUInt8 uiStencilRefValue)
{
}

void ezGALCommandEncoderImplVk::SetRasterizerStatePlatform(const ezGALRasterizerState* pRasterizerState)
{
}

void ezGALCommandEncoderImplVk::SetViewportPlatform(const ezRectFloat& rect, float fMinDepth, float fMaxDepth)
{
}

void ezGALCommandEncoderImplVk::SetScissorRectPlatform(const ezRectU32& rect)
{
}

void ezGALCommandEncoderImplVk::SetStreamOutBufferPlatform(ezUInt32 uiSlot, const ezGALBuffer* pBuffer, ezUInt32 uiOffset)
{
}

//////////////////////////////////////////////////////////////////////////

void ezGALCommandEncoderImplVk::DispatchPlatform(ezUInt32 uiThreadGroupCountX, ezUInt32 uiThreadGroupCountY, ezUInt32 uiThreadGroupCountZ)
{
}

void ezGALCommandEncoderImplVk::DispatchIndirectPlatform(const ezGALBuffer* pIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes)
{
}

//////////////////////////////////////////////////////////////////////////

