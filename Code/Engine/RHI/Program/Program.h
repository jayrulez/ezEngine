#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

#include <RHI/ShaderReflection/ShaderReflection.h>

class EZ_RHI_DLL ezRHIProgram : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIProgram() = default;
  virtual bool HasShader(ezRHIShaderType type) const = 0;
  virtual ezRHIShader* GetShader(ezRHIShaderType type) const = 0;
  virtual const ezDynamicArray<ezRHIShader*>& GetShaders() const = 0;
  virtual const ezDynamicArray<ezRHIBindKeyDescription>& GetBindings() const = 0;
  virtual const ezDynamicArray<ezRHIEntryPoint>& GetEntryPoints() const = 0;
};
