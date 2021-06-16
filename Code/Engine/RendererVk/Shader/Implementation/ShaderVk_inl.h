EZ_ALWAYS_INLINE VkShaderModule ezGALShaderVk::GetVkShaderModule() const {
  return m_pVkShaderModule;
}

EZ_ALWAYS_INLINE VkShaderStageFlagBits ezGALShaderVk::GetVkShaderStage() const
{
  return m_VkShaderStage;
}
