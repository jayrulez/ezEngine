#pragma once

#include <RHI/RHIDLL.h>

struct ezRHIMemoryBudget
{
  ezUInt64 m_Budget;
  ezUInt64 m_Usage;
};

struct EZ_RHI_DLL ezRHIDeviceCapabilities
{
  ezRHIDeviceCapabilities();

  ezUInt32 m_TextureDataPitchAlignment;
  bool m_IsDxrSupported;
  bool m_IsRayQuerySupported;
  bool m_IsVariableRateShadingSupported;
  bool m_IsMeshShadingSupported;
  ezUInt32 m_ShadingRateImageTileSize;
  ezRHIMemoryBudget m_MemoryBudget;
  ezUInt32 m_ShaderGroupHandleSize;
  ezUInt32 m_ShaderRecordAlignment;
  ezUInt32 m_ShaderTableAlignment;
};
