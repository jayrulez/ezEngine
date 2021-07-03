
#pragma once

#include <RHI/Renderer/CommandEncoder/CommandEncoder.h>

class EZ_RHI_DLL ezRHIComputeCommandEncoder : public ezRHICommandEncoder
{
public:
  ezRHIComputeCommandEncoder(ezRHIDevice& device, ezRHICommandEncoderState& state, ezRHICommandEncoderCommonPlatformInterface& commonImpl, ezRHICommandEncoderComputePlatformInterface& computeImpl);
  virtual ~ezRHIComputeCommandEncoder();

  // Dispatch

  void Dispatch(ezUInt32 uiThreadGroupCountX, ezUInt32 uiThreadGroupCountY, ezUInt32 uiThreadGroupCountZ);
  void DispatchIndirect(ezRHIBufferHandle hIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes);

  virtual void ClearStatisticsCounters() override;

private:
  void CountDispatchCall() { m_uiDispatchCalls++; }

  // Statistic variables
  ezUInt32 m_uiDispatchCalls = 0;

  ezRHICommandEncoderComputePlatformInterface& m_ComputeImpl;
};
