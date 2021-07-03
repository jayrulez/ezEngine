#include <RHIPCH.h>

#include <RHI/Renderer/CommandEncoder/ComputeCommandEncoder.h>
#include <RHI/Renderer/Device/Device.h>
#include <RHI/Renderer/Resources/Buffer.h>

ezRHIComputeCommandEncoder::ezRHIComputeCommandEncoder(ezRHIDevice& device, ezRHICommandEncoderState& state, ezRHICommandEncoderCommonPlatformInterface& commonImpl, ezRHICommandEncoderComputePlatformInterface& computeImpl)
  : ezRHICommandEncoder(device, state, commonImpl)
  , m_ComputeImpl(computeImpl)
{
}

ezRHIComputeCommandEncoder::~ezRHIComputeCommandEncoder() = default;

void ezRHIComputeCommandEncoder::Dispatch(ezUInt32 uiThreadGroupCountX, ezUInt32 uiThreadGroupCountY, ezUInt32 uiThreadGroupCountZ)
{
  AssertRenderingThread();

  EZ_ASSERT_DEBUG(uiThreadGroupCountX > 0 && uiThreadGroupCountY > 0 && uiThreadGroupCountZ > 0, "Thread group counts of zero are not meaningful. Did you mean 1?");

  /// \todo Assert for compute

  m_ComputeImpl.DispatchPlatform(uiThreadGroupCountX, uiThreadGroupCountY, uiThreadGroupCountZ);

  CountDispatchCall();
}

void ezRHIComputeCommandEncoder::DispatchIndirect(ezRHIBufferHandle hIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes)
{
  AssertRenderingThread();
  /// \todo Assert for compute
  /// \todo Assert for indirect dispatch
  /// \todo Assert offset < buffer size

  const ezRHIBuffer* pBuffer = GetDevice().GetBuffer(hIndirectArgumentBuffer);
  EZ_ASSERT_DEV(pBuffer != nullptr, "Invalid buffer handle for indirect arguments!");

  /// \todo Assert that the buffer can be used for indirect arguments (flag in desc)
  m_ComputeImpl.DispatchIndirectPlatform(pBuffer, uiArgumentOffsetInBytes);

  CountDispatchCall();
}

void ezRHIComputeCommandEncoder::ClearStatisticsCounters()
{
  ezRHICommandEncoder::ClearStatisticsCounters();

  m_uiDispatchCalls = 0;
}
