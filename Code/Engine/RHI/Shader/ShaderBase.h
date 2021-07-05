#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>
#include <RHI/Shader/Shader.h>

class ezRHIShaderBase : public ezRHIShader
{
public:
  ezRHIShaderBase(const ezRHIShaderCreationDescription& desc, ezRHIShaderBlobType blobType);
  ezRHIShaderType GetType() const override;
  const ezDynamicArray<ezUInt8>& GetBlob() const override;
  ezUInt64 GetId(const ezString& entryPoint) const override;
  const ezRHIBindKeyDescription& GetBindKey(const ezString& name) const override;
  const ezDynamicArray<ezRHIResourceBindingDescription>& GetResourceBindings() const override;
  const ezRHIResourceBindingDescription& GetResourceBinding(const ezRHIBindKeyDescription& bindKey) const override;
  const ezDynamicArray<ezRHIInputLayoutDescription>& GetInputLayouts() const override;
  ezUInt32 GetInputLayoutLocation(const ezString& semanticName) const override;
  const ezDynamicArray<ezRHIBindKeyDescription>& GetBindings() const override;
  const ezRHIShaderReflection* GetReflection() const override;

protected:
  ezRHIShaderType m_ShaderType;
  ezRHIShaderBlobType m_BlobType;
  ezDynamicArray<ezUInt8> m_Blob;
  ezMap<ezString, ezUInt64> m_Ids;
  ezDynamicArray<ezRHIResourceBindingDescription> m_Bindings;
  ezDynamicArray<ezRHIBindKeyDescription> m_BindingKeys;
  ezMap<ezRHIBindKeyDescription, ezUInt32> m_Mapping;
  ezMap<ezString, ezRHIBindKeyDescription> m_BindKeys;
  ezDynamicArray<ezRHIInputLayoutDescription> m_InputLayoutDescs;
  ezMap<ezString, ezUInt32> m_Locations;
  ezRHIShaderReflection* m_Reflection;
};
