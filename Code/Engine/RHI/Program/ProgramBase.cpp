#include <RHIPCH.h>

#include <RHI/Program/ProgramBase.h>
#include <RHI/Shader/Shader.h>

ezRHIProgramBase::ezRHIProgramBase(const ezDynamicArray<ezRHIShader*>& shaders)
  : m_Shaders{shaders}
{
  for (const auto& shader : m_Shaders)
  {
    m_ShadersByType[shader->GetType()] = shader;
    const ezDynamicArray<ezRHIBindKeyDescription>& bindings = shader->GetBindings();
    // m_bindings.insert(m_bindings.begin(), bindings.begin(), bindings.end());
    m_Bindings.PushBackRange(bindings); // todo: verify behaviour

    const ezRHIShaderReflection* reflection = shader->GetReflection();
    auto shaderEntryPoints = reflection->GetEntryPoints();

    //m_entry_points.insert(m_entry_points.end(), shader_entry_points.begin(), shader_entry_points.end());
    m_EntryPoints.PushBackRange(shaderEntryPoints); // todo: verify behaviour
  }
}

bool ezRHIProgramBase::HasShader(ezRHIShaderType type) const
{
  return m_ShadersByType.Contains(type);
}

ezRHIShader* ezRHIProgramBase::GetShader(ezRHIShaderType type) const
{
  auto it = m_ShadersByType.Find(type);
  if (it != end(m_ShadersByType))
  {
    return it.Value();
  }

  return {};
}

const ezDynamicArray<ezRHIShader*>& ezRHIProgramBase::GetShaders() const
{
  return m_Shaders;
}

const ezDynamicArray<ezRHIBindKeyDescription>& ezRHIProgramBase::GetBindings() const
{
  return m_Bindings;
}

const ezDynamicArray<ezRHIEntryPoint>& ezRHIProgramBase::GetEntryPoints() const
{
  return m_EntryPoints;
}
