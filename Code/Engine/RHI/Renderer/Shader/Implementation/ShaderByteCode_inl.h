

const void* ezRHIShaderByteCode::GetByteCode() const
{
  if (m_Source.IsEmpty())
    return nullptr;

  return &m_Source[0];
}

ezUInt32 ezRHIShaderByteCode::GetSize() const
{
  return m_Source.GetCount();
}

bool ezRHIShaderByteCode::IsValid() const
{
  return !m_Source.IsEmpty();
}
