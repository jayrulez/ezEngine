#include <RHI/Shader/ShaderBase.h>

static uint64_t GenId()
{
  static uint64_t id = 0;
  return ++id;
}

ShaderBase::ShaderBase(const ShaderDesc& desc, std::vector<uint8_t> byteCode, std::shared_ptr<ShaderReflection> reflection, ShaderBlobType blob_type)
    : m_shader_type(desc.type)
    , m_blob_type(blob_type)
{
  m_blob = byteCode;
  m_reflection = reflection;
  m_bindings = m_reflection->GetBindings();
  for (uint32_t i = 0; i < m_bindings.size(); ++i)
  {
    BindKey bind_key = {m_shader_type, m_bindings[i].type, m_bindings[i].slot, m_bindings[i].space, m_bindings[i].count};
    m_bind_keys[m_bindings[i].name] = bind_key;
    m_mapping[bind_key] = i;
    m_binding_keys.emplace_back(bind_key);
  }

  decltype(auto) input_parameters = m_reflection->GetInputParameters();
  for (uint32_t i = 0; i < input_parameters.size(); ++i)
  {
    decltype(auto) layout = m_input_layout_descs.emplace_back();
    layout.slot = i;
    layout.semantic_name = input_parameters[i].semantic_name;
    layout.format = input_parameters[i].format;
    layout.stride = ezRHIResourceFormat::GetFormatStride(layout.format);
    m_locations[input_parameters[i].semantic_name] = input_parameters[i].location;
  }

  for (const auto& entry_point : m_reflection->GetEntryPoints())
  {
    m_ids.emplace(entry_point.name, GenId());
  }
}

ShaderType ShaderBase::GetType() const
{
    return m_shader_type;
}

const std::vector<uint8_t>& ShaderBase::GetBlob() const
{
    return m_blob;
}

uint64_t ShaderBase::GetId(const ezString& entry_point) const
{
    return m_ids.at(entry_point);
}

const BindKey& ShaderBase::GetBindKey(const ezString& name) const
{
    return m_bind_keys.at(name);
}

const std::vector<ResourceBindingDesc>& ShaderBase::GetResourceBindings() const
{
    return m_bindings;
}

const ResourceBindingDesc& ShaderBase::GetResourceBinding(const BindKey& bind_key) const
{
    return m_bindings.at(m_mapping.at(bind_key));
}

const std::vector<InputLayoutDesc>& ShaderBase::GetInputLayouts() const
{
    return m_input_layout_descs;
}

uint32_t ShaderBase::GetInputLayoutLocation(const ezString& semantic_name) const
{
    return m_locations.at(semantic_name);
}

const std::vector<BindKey>& ShaderBase::GetBindings() const
{
    return m_binding_keys;
}

const std::shared_ptr<ShaderReflection>& ShaderBase::GetReflection() const
{
    return m_reflection;
}
