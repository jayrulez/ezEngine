#include <RHIPCH.h>

#include <RHI/Renderer/Shader/ShaderByteCode.h>

ezRHIShaderByteCode::ezRHIShaderByteCode() {}

ezRHIShaderByteCode::ezRHIShaderByteCode(const ezArrayPtr<const ezUInt8>& pByteCode)
{
  CopyFrom(pByteCode);
}

void ezRHIShaderByteCode::CopyFrom(const ezArrayPtr<const ezUInt8>& pByteCode)
{
  EZ_ASSERT_DEV(pByteCode.GetPtr() != nullptr && pByteCode.GetCount() != 0, "Byte code is invalid!");

  m_Source = pByteCode;
}



EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Shader_Implementation_ShaderByteCode);
