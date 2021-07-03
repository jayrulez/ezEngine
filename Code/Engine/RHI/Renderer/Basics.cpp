#include <RHIPCH.h>

#include <RHI/Renderer/Basics.h>

const ezUInt8 ezRHIIndexType::Size[ezRHIIndexType::ENUM_COUNT] = {
  sizeof(ezInt16), // UShort
  sizeof(ezInt32)  // UInt
};

const char* ezRHIShaderStage::Names[ENUM_COUNT] = {
  "VertexShader",
  "HullShader",
  "DomainShader",
  "GeometryShader",
  "PixelShader",
  "ComputeShader",
};

EZ_BEGIN_STATIC_REFLECTED_ENUM(ezRHIMSAASampleCount, 1)
  EZ_ENUM_CONSTANTS(
    ezRHIMSAASampleCount::None, ezRHIMSAASampleCount::TwoSamples, ezRHIMSAASampleCount::FourSamples, ezRHIMSAASampleCount::EightSamples)
EZ_END_STATIC_REFLECTED_ENUM;

EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Basics);
