
#pragma once

#include <RHI/Renderer/Resources/RenderTargetSetup.h>

class EZ_RHI_DLL ezRHIPass
{
  EZ_DISALLOW_COPY_AND_ASSIGN(ezRHIPass);

public:
  ezRHIRenderCommandEncoder* BeginRendering(const ezRHIRenderingSetup& renderingSetup, const char* szName = "");
  void EndRendering(ezRHIRenderCommandEncoder* pCommandEncoder);

  ezRHIComputeCommandEncoder* BeginCompute(const char* szName = "");
  void EndCompute(ezRHIComputeCommandEncoder* pCommandEncoder);

  // BeginRaytracing() could be here as well (would match Vulkan)

protected:
  virtual ezRHIRenderCommandEncoder* BeginRenderingPlatform(const ezRHIRenderingSetup& renderingSetup, const char* szName) = 0;
  virtual void EndRenderingPlatform(ezRHIRenderCommandEncoder* pCommandEncoder) = 0;

  virtual ezRHIComputeCommandEncoder* BeginComputePlatform(const char* szName) = 0;
  virtual void EndComputePlatform(ezRHIComputeCommandEncoder* pCommandEncoder) = 0;

  ezRHIPass(ezRHIDevice& device);
  virtual ~ezRHIPass();

  ezRHIDevice& m_Device;

  enum class CommandEncoderType
  {
    Invalid,
    Render,
    Compute
  };

  CommandEncoderType m_CurrentCommandEncoderType = CommandEncoderType::Invalid;
  bool m_bMarker = false;
};
