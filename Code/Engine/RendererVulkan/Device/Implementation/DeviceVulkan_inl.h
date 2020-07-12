
EZ_ALWAYS_INLINE vk::Device ezGALDeviceVulkan::GetVulkanDevice() const
{
  return m_device;
}

EZ_ALWAYS_INLINE vk::Instance ezGALDeviceVulkan::GetVulkanInstance() const
{
  return m_instance;
}

EZ_ALWAYS_INLINE vk::Instance ezGALDeviceVulkan::GetFamilyQueueIndices() const
{
  return m_queueFamilyIndices;
}

EZ_ALWAYS_INLINE const ezGALFormatLookupTableVulkan& ezGALDeviceVulkan::GetFormatLookupTable() const
{
  return m_FormatLookupTable;
}
/*
inline ID3D11Query* ezGALDeviceVulkan::GetTimestamp(ezGALTimestampHandle hTimestamp)
{
  if (hTimestamp.m_uiIndex < m_Timestamps.GetCount())
  {
    return m_Timestamps[static_cast<ezUInt32>(hTimestamp.m_uiIndex)];
  }

  return nullptr;
}

*/
