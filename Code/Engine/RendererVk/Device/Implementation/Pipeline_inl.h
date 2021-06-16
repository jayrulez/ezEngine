EZ_ALWAYS_INLINE PipelineType Pipeline::GetPipelineType() const
{
  return m_Description.Type;
}

EZ_ALWAYS_INLINE const PipelineStateDesc& Pipeline::GetDescription() const
{
  return m_Description;
}
