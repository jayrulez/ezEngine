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
      d3D11DeviceOptions.DeviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }

    return d3D11DeviceOptions;
  }

  static bool IsSdkLayersAvailable()
  {
    return true;
  }

  static ezResult CreateDevice(IDXGIAdapter adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device)
  {
    return EZ_SUCCESS;
  }
  static ezResult CreateDevice(IDXGIAdapter adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, ID3D11DeviceContext* immediateContext)
  {
    return EZ_SUCCESS;
  }
  static ezResult CreateDevice(IDXGIAdapter adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, D3D_FEATURE_LEVEL& featureLevel, ID3D11DeviceContext* immediateContext)
  {
    return EZ_SUCCESS;
  }

  static ezResult CreateDevice(ezUInt32* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device)
  {
    return EZ_SUCCESS;
  }
  static ezResult CreateDevice(ezUInt32* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, ID3D11DeviceContext* immediateContext)
  {
    return EZ_SUCCESS;
  }
  static ezResult CreateDevice(ezUInt32* adapter, D3D_DRIVER_TYPE type, D3D11_CREATE_DEVICE_FLAG flags, D3D_FEATURE_LEVEL featureLevels[], ID3D11Device* device, D3D_FEATURE_LEVEL& featureLevel, ID3D11DeviceContext* immediateContext)
  {
    return EZ_SUCCESS;
  }

public:
  RHID3D11GraphicsDevice(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions, std::optional<const RHISwapchainDescription&> swapchainDesc)
    : RHID3D11GraphicsDevice(MergeOptions(options, d3D11DeviceOptions), swapchainDesc)
  {
  }

  RHID3D11GraphicsDevice(const RHID3D11DeviceOptions& options, std::optional<const RHISwapchainDescription&> swapchainDesc)
    : RHIGraphicsDevice()
  {
    ezUInt32 flags = options.DeviceCreationFlags;

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // If debug flag set but SDK layers aren't available we can't enable debug.
    if ((flags & D3D11_CREATE_DEVICE_DEBUG) != 0 &&  !IsSdkLayersAvailable())
    {
      flags &= ~D3D11_CREATE_DEVICE_DEBUG;
    }

    if (options.AdapterPtr != nullptr)
    {

    }
  }
};
