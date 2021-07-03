

#pragma once
#include <RHI/Renderer/Basics.h>

/// \brief This struct holds information about the rendering device capabilities (e.g. what shader stages are supported and more)
/// To get the device capabilities you need to call the GetCapabilities() function on an ezRHIRenderDevice object.
struct EZ_RHI_DLL ezRHIRenderDeviceCapabilities
{
  ezRHIRenderDeviceCapabilities();

  // Device description
  ezString m_sAdapterName = "Unknown";
  ezUInt64 m_uiDedicatedVRAM = 0;
  ezUInt64 m_uiDedicatedSystemRAM = 0;
  ezUInt64 m_uiSharedSystemRAM = 0;
  bool m_bHardwareAccelerated = false;

  // General capabilities
  bool m_bMultithreadedResourceCreation; ///< whether creating resources is allowed on other threads than the main thread
  bool m_bNoOverwriteBufferUpdate;

  // Draw related capabilities
  bool m_bShaderStageSupported[ezRHIShaderStage::ENUM_COUNT];
  bool m_bInstancing;
  bool m_b32BitIndices;
  bool m_bIndirectDraw;
  bool m_bStreamOut;
  bool m_bConservativeRasterization;
  ezUInt16 m_uiMaxConstantBuffers;


  // Texture related capabilities
  bool m_bTextureArrays;
  bool m_bCubemapArrays;
  bool m_bB5G6R5Textures;
  ezUInt16 m_uiMaxTextureDimension;
  ezUInt16 m_uiMaxCubemapDimension;
  ezUInt16 m_uiMax3DTextureDimension;
  ezUInt16 m_uiMaxAnisotropy;


  // Output related capabilities
  ezUInt16 m_uiMaxRendertargets;
  ezUInt16 m_uiUAVCount;
  bool m_bAlphaToCoverage;
};
