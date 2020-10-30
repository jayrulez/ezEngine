#include <RHI/Backends/D3D11/D3D11GraphicsDevice.h>

RHID3D11DeviceOptions D3D11GraphicsDevice::MergeOptions(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions)
{
  if (options.Debug)
  {
    d3D11DeviceOptions.DeviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
  }

  return d3D11DeviceOptions;
}

D3D11GraphicsDevice::D3D11GraphicsDevice(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions, const std::optional<RHISwapchainDescription>& swapchainDesc)
  : D3D11GraphicsDevice(MergeOptions(options, d3D11DeviceOptions), swapchainDesc)
{
}

D3D11GraphicsDevice::D3D11GraphicsDevice(const RHID3D11DeviceOptions& options, const std::optional<RHISwapchainDescription>& swapchainDesc)
  : RHIGraphicsDevice()
{
  ezUInt32 flags = options.DeviceCreationFlags;

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  // If debug flag set but SDK layers aren't available we can't enable debug.
  if ((flags & D3D11_CREATE_DEVICE_DEBUG) != 0 && !D3D11DeviceUtils::IsSdkLayersAvailable())
  {
    flags &= ~D3D11_CREATE_DEVICE_DEBUG;
  }

  if (options.AdapterPtr != nullptr)
  {
  }
}
