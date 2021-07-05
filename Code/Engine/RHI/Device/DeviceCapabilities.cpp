#include <RHIPCH.h>

#include <RHI/Device/DeviceCapabilities.h>

ezRHIDeviceCapabilities::ezRHIDeviceCapabilities()
{
  m_TextureDataPitchAlignment = 0;
  m_IsDxrSupported = false;
  m_IsRayQuerySupported = false;
  m_IsVariableRateShadingSupported = false;
  m_IsMeshShadingSupported = false;
  m_ShadingRateImageTileSize = 0;
  m_MemoryBudget = {};
  m_ShaderGroupHandleSize = 0;
  m_ShaderRecordAlignment = 0;
  m_ShaderTableAlignment = 0;
}
