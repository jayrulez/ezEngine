#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>
#include <RHI/Program/Program.h>

class EZ_RHI_DLL ezRHIProgramBase : public ezRHIProgram
{
public:
  ezRHIProgramBase(const ezDynamicArray<ezRHIShader*>& shaders);

  bool HasShader(ezRHIShaderType type) const override final;
  ezRHIShader* GetShader(ezRHIShaderType type) const override final;
  const ezDynamicArray<ezRHIShader*>& GetShaders() const override final;
  const ezDynamicArray<ezRHIBindKeyDescription>& GetBindings() const override final;
  const ezDynamicArray<ezRHIEntryPoint>& GetEntryPoints() const override final;

protected:
  ezMap<ezRHIShaderType, ezRHIShader*> m_ShadersByType;
  ezDynamicArray<ezRHIShader*> m_Shaders;
  ezDynamicArray<ezRHIBindKeyDescription> m_Bindings;
  ezDynamicArray<ezRHIEntryPoint> m_EntryPoints;
};
