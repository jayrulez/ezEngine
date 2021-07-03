
#pragma once

#include <RHI/Renderer/CommandEncoder/CommandEncoder.h>
#include <RHI/CommandList/CommandList.h>

class EZ_RHI_DLL ezRHIComputeCommandEncoder : public ezRHICommandEncoder
{
public:
  ezRHIComputeCommandEncoder(ezRHIRenderDevice& device, ezRHICommandEncoderState& state);
  virtual ~ezRHIComputeCommandEncoder();

  // Dispatch

  void Dispatch(ezUInt32 uiThreadGroupCountX, ezUInt32 uiThreadGroupCountY, ezUInt32 uiThreadGroupCountZ);
  void DispatchIndirect(ezRHIBufferHandle hIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes);

  virtual void ClearStatisticsCounters() override;

private:
  void CountDispatchCall() { m_uiDispatchCalls++; }

  // Statistic variables
  ezUInt32 m_uiDispatchCalls = 0;

  std::shared_ptr<CommandList> m_pCommandList;
};
