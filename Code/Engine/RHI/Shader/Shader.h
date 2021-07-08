#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class EZ_RHI_DLL ezRHIShader : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIShader() = default;
  virtual ezRHIShaderType GetType() const = 0;
  virtual const ezDynamicArray<ezUInt8>& GetBlob() const = 0;
  virtual ezUInt64 GetId(const ezString& entryPoint) const = 0;
  virtual const ezRHIBindKeyDescription& GetBindKey(const ezString& name) const = 0;
  virtual const ezDynamicArray<ezRHIResourceBindingDescription>& GetResourceBindings() const = 0;
  virtual const ezRHIResourceBindingDescription& GetResourceBinding(const ezRHIBindKeyDescription& bindKey) const = 0;
  virtual const ezDynamicArray<ezRHIInputLayoutDescription>& GetInputLayouts() const = 0;
  virtual ezUInt32 GetInputLayoutLocation(const ezString& semanticName) const = 0;
  virtual const ezDynamicArray<ezRHIBindKeyDescription>& GetBindings() const = 0;
  virtual const ezRHIShaderReflection* GetReflection() const = 0;
};
