// static
EZ_ALWAYS_INLINE void ezRHIDevice::SetDefaultDevice(ezRHIDevice* pDefaultDevice)
{
  s_pDefaultDevice = pDefaultDevice;
}

// static
EZ_ALWAYS_INLINE ezRHIDevice* ezRHIDevice::GetDefaultDevice()
{
  EZ_ASSERT_DEBUG(s_pDefaultDevice != nullptr, "Default device not set.");
  return s_pDefaultDevice;
}

// static
EZ_ALWAYS_INLINE bool ezRHIDevice::HasDefaultDevice()
{
  return s_pDefaultDevice != nullptr;
}