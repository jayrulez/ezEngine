#include <RHIPCH.h>

#include <RHI/Renderer/Shader/Shader.h>
#include <RHI/Renderer/Shader/ShaderUtils.h>

ezRHIShader::ezRHIShader(const ezRHIShaderCreationDescription& Description)
  : ezRHIObject(Description)
{
}

ezRHIShader::~ezRHIShader() {}



EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Shader_Implementation_Shader);
