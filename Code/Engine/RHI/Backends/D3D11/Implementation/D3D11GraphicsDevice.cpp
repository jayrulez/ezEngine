#include <RHI/Backends/D3D11/D3D11GraphicsDevice.h>

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

  if (!InitializeDevice(options.AdapterPtr, Device, ImmediateContext, flags).Succeeded())
  {
    if ((flags & D3D11_CREATE_DEVICE_DEBUG) != 0)
    {
      flags &= ~D3D11_CREATE_DEVICE_DEBUG;

      // try again without debug device
      if (!InitializeDevice(options.AdapterPtr, Device, ImmediateContext, flags).Succeeded())
      {
        EZ_REPORT_FAILURE("Failed to initialize ID3D11Device.");
      }
    }
    else
    {
      EZ_REPORT_FAILURE("Failed to initialize ID3D11Device.");
    }
  }

  IDXGIDevice* dxgiDevice = nullptr;
  if (!SUCCEEDED(Device->QueryInterface<IDXGIDevice>(&dxgiDevice)))
  {
    EZ_REPORT_FAILURE("Unable to get IDXGIDevice from Device.");
  }
  if (!SUCCEEDED(dxgiDevice->GetAdapter(&DXGIAdapter)))
  {
    EZ_REPORT_FAILURE("Unable to get IDXGIAdapter from IDXGIDevice.");
  }

  if (swapchainDesc.has_value())
  {
    RHISwapchainDescription desc = swapchainDesc.value();
    MainSwapchain = new D3D11Swapchain(this, desc);
  }

  D3D11_FEATURE_DATA_THREADING threadingSupport;
  if (SUCCEEDED(Device->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threadingSupport, sizeof(D3D11_FEATURE_DATA_THREADING))))
  {
    _supportsCommandLists = threadingSupport.DriverCommandLists;
    _supportsConcurrentResources = threadingSupport.DriverConcurrentCreates;
  }
  else
  {
    _supportsCommandLists = false;
    _supportsConcurrentResources = false;
  }

  D3D11_FEATURE_DATA_DOUBLES dataDoublesSupport;
  if (SUCCEEDED(Device->CheckFeatureSupport(D3D11_FEATURE_THREADING, &dataDoublesSupport, sizeof(D3D11_FEATURE_DATA_DOUBLES))))
  {
  }

  Features = RHIGraphicsDeviceFeatures(
    true,                                                //computeShader
    true,                                                //geometryShader
    true,                                                //tessellationShaders
    true,                                                //multipleViewports
    true,                                                // samplerLodBias
    true,                                                //drawBaseVertex
    true,                                                //drawBaseInstance
    true,                                                //drawIndirect
    true,                                                //drawIndirectBaseInstance
    true,                                                //fillModeWireframe
    true,                                                //samplerAnisotropy
    true,                                                //depthClipDisable
    true,                                                //texture1D
    true,                                                //independentBlend
    true,                                                //structuredBuffer
    true,                                                // subsetTextureView
    Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_1, //commandListDebugMarkers
    Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_1, //bufferRangeBinding
    dataDoublesSupport.DoublePrecisionFloatShaderOps     //shaderFloat64
  );

  ResourceFactory = new D3D11ResourceFactory(this);

  PostDeviceCreated();
}

ezResult D3D11GraphicsDevice::InitializeDevice(IDXGIAdapter* dxgiAdapter, ID3D11Device* device, ID3D11DeviceContext* immediateContext, ezUInt32 flags)
{
  D3D_FEATURE_LEVEL FeatureLevels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3};

  for (ezUInt32 FeatureLevelIdx = 0; FeatureLevelIdx < EZ_ARRAY_SIZE(FeatureLevels); FeatureLevelIdx++)
  {
    if (SUCCEEDED(D3D11CreateDevice(dxgiAdapter, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, FeatureLevels, 1, D3D11_SDK_VERSION, &device, (D3D_FEATURE_LEVEL*)&FeatureLevel, &immediateContext)))
      break;
  }

  if (ImmediateContext == nullptr)
    return EZ_FAILURE;
  return EZ_SUCCESS;
}

RHID3D11DeviceOptions D3D11GraphicsDevice::MergeOptions(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions)
{
  if (options.Debug)
  {
    d3D11DeviceOptions.DeviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
  }

  return d3D11DeviceOptions;
}
