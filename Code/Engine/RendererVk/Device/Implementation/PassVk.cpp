#include <RendererVkPCH.h>

#include <RendererVk/CommandEncoder/CommandEncoderImplVk.h>
#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Device/PassVk.h>
#include <RendererFoundation/CommandEncoder/CommandEncoderState.h>
#include <RendererFoundation/CommandEncoder/ComputeCommandEncoder.h>
#include <RendererFoundation/CommandEncoder/RenderCommandEncoder.h>

ezGALPassVk::ezGALPassVk(ezGALDevice& device)
  : ezGALPass(device)
{
  m_pCommandEncoderState = EZ_DEFAULT_NEW(ezGALCommandEncoderRenderState);
  m_pCommandEncoderImpl = EZ_DEFAULT_NEW(ezGALCommandEncoderImplVk, static_cast<ezGALDeviceVk&>(device));

  m_pRenderCommandEncoder = EZ_DEFAULT_NEW(ezGALRenderCommandEncoder, device, *m_pCommandEncoderState, *m_pCommandEncoderImpl, *m_pCommandEncoderImpl);
  m_pComputeCommandEncoder = EZ_DEFAULT_NEW(ezGALComputeCommandEncoder, device, *m_pCommandEncoderState, *m_pCommandEncoderImpl, *m_pCommandEncoderImpl);

  m_pCommandEncoderImpl->m_pOwner = m_pRenderCommandEncoder.Borrow();
}

ezGALPassVk::~ezGALPassVk() = default;

ezGALRenderCommandEncoder* ezGALPassVk::BeginRenderingPlatform(const ezGALRenderingSetup& renderingSetup, const char* szName)
{
  m_pCommandEncoderImpl->BeginRendering(renderingSetup);

  return m_pRenderCommandEncoder.Borrow();
}

void ezGALPassVk::EndRenderingPlatform(ezGALRenderCommandEncoder* pCommandEncoder)
{
  EZ_ASSERT_DEV(m_pRenderCommandEncoder.Borrow() == pCommandEncoder, "Invalid command encoder");
}

ezGALComputeCommandEncoder* ezGALPassVk::BeginComputePlatform(const char* szName)
{
  return m_pComputeCommandEncoder.Borrow();
}

void ezGALPassVk::EndComputePlatform(ezGALComputeCommandEncoder* pCommandEncoder)
{
  EZ_ASSERT_DEV(m_pComputeCommandEncoder.Borrow() == pCommandEncoder, "Invalid command encoder");
}

void ezGALPassVk::BeginPass(const char* szName)
{
  m_pCommandEncoderImpl->PushMarkerPlatform(szName);
}

void ezGALPassVk::EndPass()
{
  m_pCommandEncoderImpl->PopMarkerPlatform();
}
