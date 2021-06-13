#include <RHIDX11/CommandList/DX11CommandList.h>

ezRHIDX11CommantList::ezRHIDX11CommantList()
  : ezRHICommandList()
{
}

ezRHIDX11CommantList::~ezRHIDX11CommantList()
{
}

void ezRHIDX11CommantList::RenderPassBegin(const ezRHISwapChain* swapchain)
{
}

void ezRHIDX11CommantList::RenderPassBegin(const ezRHIRenderPass* renderpass)
{
}

void ezRHIDX11CommantList::RenderPassEnd()
{
}

void ezRHIDX11CommantList::BindScissorRects(ezUInt32 numRects, const ezRHIRect* rects)
{
}

void ezRHIDX11CommantList::BindViewports(ezUInt32 NumViewports, const ezRHIViewport* pViewports)
{
}

void ezRHIDX11CommantList::BindResource(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* resource, ezUInt32 slot, ezInt32 subresource)
{
}

void ezRHIDX11CommantList::BindResources(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* const* resources, ezUInt32 slot, ezUInt32 count)
{
}

void ezRHIDX11CommantList::BindUAV(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* resource, ezUInt32 slot, ezInt32 subresource)
{
}

void ezRHIDX11CommantList::BindUAVs(ezRHIShaderStage::Enum stage, const ezRHIResourceBase* const* resources, ezUInt32 slot, ezUInt32 count)
{
}

void ezRHIDX11CommantList::UnbindResources(ezUInt32 slot, ezUInt32 num)
{
}

void ezRHIDX11CommantList::UnbindUAVs(ezUInt32 slot, ezUInt32 num)
{
}

void ezRHIDX11CommantList::BindSampler(ezRHIShaderStage::Enum stage, const ezRHISamplerState* sampler, ezUInt32 slot)
{
}

void ezRHIDX11CommantList::BindConstantBuffer(ezRHIShaderStage::Enum stage, const ezRHIGPUBuffer* buffer, ezUInt32 slot)
{
}

void ezRHIDX11CommantList::BindVertexBuffers(const ezRHIGPUBuffer* const* vertexBuffers, ezUInt32 slot, ezUInt32 count, const ezUInt32* strides, const ezUInt32* offsets)
{
}

void ezRHIDX11CommantList::BindIndexBuffer(const ezRHIGPUBuffer* indexBuffer, const ezRHIIndexType::Enum format, ezUInt32 offset)
{
}

void ezRHIDX11CommantList::BindStencilRef(ezUInt32 value)
{
}

void ezRHIDX11CommantList::BindBlendFactor(float r, float g, float b, float a)
{
}

void ezRHIDX11CommantList::BindPipelineState(const ezRHIPipelineState* pso)
{
}

void ezRHIDX11CommantList::BindComputeShader(const ezRHIShader* cs)
{
}

void ezRHIDX11CommantList::Draw(ezUInt32 vertexCount, ezUInt32 startVertexLocation)
{
}

void ezRHIDX11CommantList::DrawIndexed(ezUInt32 indexCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation)
{
}

void ezRHIDX11CommantList::DrawInstanced(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 startVertexLocation, ezUInt32 startInstanceLocation)
{
}

void ezRHIDX11CommantList::DrawIndexedInstanced(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, ezUInt32 startInstanceLocation)
{
}

void ezRHIDX11CommantList::DrawInstancedIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset)
{
}

void ezRHIDX11CommantList::DrawIndexedInstancedIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset)
{
}

void ezRHIDX11CommantList::Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ)
{
}

void ezRHIDX11CommantList::DispatchIndirect(const ezRHIGPUBuffer* args, ezUInt32 argsOffset)
{
}

void ezRHIDX11CommantList::CopyResource(const ezRHIResourceBase* pDst, const ezRHIResourceBase* pSrc)
{
}

void ezRHIDX11CommantList::UpdateBuffer(const ezRHIGPUBuffer* buffer, const void* data, ezInt32 dataSize)
{
}

void ezRHIDX11CommantList::QueryBegin(const ezRHIQueryHeap* heap, ezUInt32 index)
{
}

void ezRHIDX11CommantList::QueryEnd(const ezRHIQueryHeap* heap, ezUInt32 index)
{
}

ezRHICommandList::GPUAllocation ezRHIDX11CommantList::AllocateGPU(ezUInt64 dataSize)
{
  return GPUAllocation();
}

void ezRHIDX11CommantList::EventBegin(const char* name)
{
}

void ezRHIDX11CommantList::EventEnd()
{
}

void ezRHIDX11CommantList::SetMarker(const char* name)
{
}
