
#pragma once

#include <Foundation/Types/UniquePtr.h>
#include <RendererFoundation/Device/Pass.h>

struct ezGALCommandEncoderRenderState;
class ezGALRenderCommandEncoder;
class ezGALComputeCommandEncoder;

class ezGALCommandEncoderImplVk;

class ezGALPassVk : public ezGALPass
{
protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALPassVk(ezGALDevice& device);
  virtual ~ezGALPassVk();

  virtual ezGALRenderCommandEncoder* BeginRenderingPlatform(const ezGALRenderingSetup& renderingSetup, const char* szName) override;
  virtual void EndRenderingPlatform(ezGALRenderCommandEncoder* pCommandEncoder) override;

  virtual ezGALComputeCommandEncoder* BeginComputePlatform(const char* szName) override;
  virtual void EndComputePlatform(ezGALComputeCommandEncoder* pCommandEncoder) override;

  void BeginPass(const char* szName);
  void EndPass();

private:
  ezUniquePtr<ezGALCommandEncoderRenderState> m_pCommandEncoderState;
  ezUniquePtr<ezGALCommandEncoderImplVk> m_pCommandEncoderImpl;

  ezUniquePtr<ezGALRenderCommandEncoder> m_pRenderCommandEncoder;
  ezUniquePtr<ezGALComputeCommandEncoder> m_pComputeCommandEncoder;
};
