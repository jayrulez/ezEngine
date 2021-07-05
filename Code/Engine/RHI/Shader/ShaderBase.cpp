#include <RHIPCH.h>

#include <RHI/Shader/ShaderBase.h>
#include <RHI/ShaderReflection/ShaderReflection.h>

static ezUInt64 GenId()
{
  static ezUInt64 id = 0;
  return ++id;
}

ezRHIShaderBase::ezRHIShaderBase(const ezRHIShaderCreationDescription& desc, ezRHIShaderBlobType blobType)
  : m_ShaderType{desc.m_Type}
  , m_BlobType{blobType}
{
  m_Reflection = nullptr;
  //m_Blob = Compile(desc, blobType);
  //m_Reflection = CreateShaderReflection(blobType, m_Blob.GetData(), m_Blob.GetCount());

  m_Bindings = m_Reflection->GetBindings();
  for (ezUInt32 i = 0; i < m_Bindings.GetCount(); ++i)
  {
    ezRHIBindKeyDescription bindKey = {m_ShaderType, m_Bindings[i].m_Type, m_Bindings[i].m_Slot, m_Bindings[i].m_Space, m_Bindings[i].m_Count};
    m_BindKeys[m_Bindings[i].m_Name] = bindKey;
    m_Mapping[bindKey] = i;
    m_BindingKeys.PushBack(bindKey);
  }

  const ezDynamicArray<ezRHIInputParameterDescription>& inputParameters = m_Reflection->GetInputParameters();
  for (ezUInt32 i = 0; i < inputParameters.GetCount(); ++i)
  {
    ezRHIInputLayoutDescription& layout = m_InputLayoutDescs.ExpandAndGetRef();
    layout.m_Slot = i;
    layout.m_SemanticName = inputParameters[i].m_SemanticName;
    layout.m_Format = inputParameters[i].m_Format;
    //layout.m_Stride = gli::detail::bits_per_pixel(layout.m_Format) / 8;
    m_Locations[inputParameters[i].m_SemanticName] = inputParameters[i].m_Location;
  }

  for (const ezRHIEntryPoint& entryPoint : m_Reflection->GetEntryPoints())
  {
    m_Ids.Insert(entryPoint.m_Name, GenId());
  }
}

ezRHIShaderType ezRHIShaderBase::GetType() const
{
  return m_ShaderType;
}

const ezDynamicArray<ezUInt8>& ezRHIShaderBase::GetBlob() const
{
  return m_Blob;
}

ezUInt64 ezRHIShaderBase::GetId(const ezString& entryPoint) const
{
  return *m_Ids.GetValue(entryPoint);
}

const ezRHIBindKeyDescription& ezRHIShaderBase::GetBindKey(const ezString& name) const
{
  return *m_BindKeys.GetValue(name);
}

const ezDynamicArray<ezRHIResourceBindingDescription>& ezRHIShaderBase::GetResourceBindings() const
{
  return m_Bindings;
}

const ezRHIResourceBindingDescription& ezRHIShaderBase::GetResourceBinding(const ezRHIBindKeyDescription& bindKey) const
{
  return m_Bindings[*m_Mapping.GetValue(bindKey)];
}

const ezDynamicArray<ezRHIInputLayoutDescription>& ezRHIShaderBase::GetInputLayouts() const
{
  return m_InputLayoutDescs;
}

ezUInt32 ezRHIShaderBase::GetInputLayoutLocation(const ezString& semanticName) const
{
  return *m_Locations.GetValue(semanticName);
}

const ezDynamicArray<ezRHIBindKeyDescription>& ezRHIShaderBase::GetBindings() const
{
  return m_BindingKeys;
}

const ezRHIShaderReflection* ezRHIShaderBase::GetReflection() const
{
  return m_Reflection;
}
