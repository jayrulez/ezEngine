#include <RHI/Device/D3D11DeviceOptions.h>
#include <RHI/Device/GraphicsDevice.h>
#include <optional>

#include <d3d11.h>

class RHID3D11GraphicsDevice : public RHIGraphicsDevice
{
private:
  static RHID3D11DeviceOptions MergeOptions(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions)
  {
    if (options.Debug)
    {
      d3D11DeviceOptions.DeviceCreationFlags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
    }

    return d3D11DeviceOptions;
  }

public:
  RHID3D11GraphicsDevice(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions, std::optional<const RHISwapchainDescription&> swapchainDesc)
    : RHID3D11GraphicsDevice(MergeOptions(options, d3D11DeviceOptions), swapchainDesc)
  {
  }

  RHID3D11GraphicsDevice(const RHID3D11DeviceOptions& options, std::optional<const RHISwapchainDescription&> swapchainDesc)
    : RHIGraphicsDevice()
  {
  }
};
