EZ_ALWAYS_INLINE VkDevice ezGALDeviceVk::GetVkDevice() const
{
  return m_VkDevice;
}

EZ_ALWAYS_INLINE VkInstance ezGALDeviceVk::GetVkInstance() const
{
  return m_VkInstance;
}

EZ_ALWAYS_INLINE VkPhysicalDevice ezGALDeviceVk::GetVkPhysicalDevice() const
{
  return m_VkPhysicalDevice;
}

EZ_ALWAYS_INLINE const ezStaticArray<ezInt32, ezInternal::Vk::QueueType::Count>& ezGALDeviceVk::GetVkQueueFamilyIndices() const
{
  return m_QueueFamilyIndices;
}

EZ_ALWAYS_INLINE ezInt32 ezGALDeviceVk::GetVkPresentQueueFamilyIndex() const
{
  return m_VkPresentQueueFamilyIndex;
}
