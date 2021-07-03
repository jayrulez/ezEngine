#include <RHIPCH.h>

#include <RHI/Renderer/CommandEncoder/ComputeCommandEncoder.h>
#include <RHI/Renderer/CommandEncoder/RenderCommandEncoder.h>
#include <RHI/Renderer/Device/Pass.h>

ezRHIRenderCommandEncoder* ezRHIPass::BeginRendering(const ezRHIRenderingSetup& renderingSetup, const char* szName /*= ""*/)
{
  EZ_ASSERT_DEV(m_CurrentCommandEncoderType == CommandEncoderType::Invalid, "Nested Command Encoder are not allowed");
  m_CurrentCommandEncoderType = CommandEncoderType::Render;

  ezRHIRenderCommandEncoder* pCommandEncoder = BeginRenderingPlatform(renderingSetup, szName);

  m_bMarker = !ezStringUtils::IsNullOrEmpty(szName);
  if (m_bMarker)
  {
    pCommandEncoder->PushMarker(szName);
  }

  return pCommandEncoder;
}

void ezRHIPass::EndRendering(ezRHIRenderCommandEncoder* pCommandEncoder)
{
  EZ_ASSERT_DEV(m_CurrentCommandEncoderType == CommandEncoderType::Render, "BeginRendering has not been called");
  m_CurrentCommandEncoderType = CommandEncoderType::Invalid;

  if (m_bMarker)
  {
    pCommandEncoder->PopMarker();
    m_bMarker = false;
  }

  EndRenderingPlatform(pCommandEncoder);
}

ezRHIComputeCommandEncoder* ezRHIPass::BeginCompute(const char* szName /*= ""*/)
{
  EZ_ASSERT_DEV(m_CurrentCommandEncoderType == CommandEncoderType::Invalid, "Nested Command Encoder are not allowed");
  m_CurrentCommandEncoderType = CommandEncoderType::Compute;

  ezRHIComputeCommandEncoder* pCommandEncoder = BeginComputePlatform(szName);

  m_bMarker = !ezStringUtils::IsNullOrEmpty(szName);
  if (m_bMarker)
  {
    pCommandEncoder->PushMarker(szName);
  }

  return pCommandEncoder;
}

void ezRHIPass::EndCompute(ezRHIComputeCommandEncoder* pCommandEncoder)
{
  EZ_ASSERT_DEV(m_CurrentCommandEncoderType == CommandEncoderType::Compute, "BeginCompute has not been called");
  m_CurrentCommandEncoderType = CommandEncoderType::Invalid;

  if (m_bMarker)
  {
    pCommandEncoder->PopMarker();
    m_bMarker = false;
  }

  EndComputePlatform(pCommandEncoder);
}

ezRHIPass::ezRHIPass(ezRHIDevice& device)
  : m_Device(device)
{
}

ezRHIPass::~ezRHIPass() = default;
