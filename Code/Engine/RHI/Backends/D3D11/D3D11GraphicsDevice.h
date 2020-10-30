#pragma once

#include <RHI/Device/D3D11DeviceOptions.h>
#include <RHI/Device/GraphicsDevice.h>
#include <optional>

struct ID3D11Device;
struct IDXGIAdapter;

class D3D11GraphicsDevice : public RHIGraphicsDevice
{
private:
  static RHID3D11DeviceOptions MergeOptions(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions);

public:
  D3D11GraphicsDevice(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions, const std::optional<RHISwapchainDescription>& swapchainDesc);

  D3D11GraphicsDevice(const RHID3D11DeviceOptions& options, const std::optional<RHISwapchainDescription>& swapchainDesc);

  ID3D11Device* GetDevice() const { return Device; }
  IDXGIAdapter* GetAdapter() const { return DXGIAdapter; }

  bool SupportsCommandLists() const
  {
    return _supportsCommandLists;
  }

private:
  bool _supportsCommandLists;
  IDXGIAdapter* DXGIAdapter = nullptr;
  ID3D11Device* Device = nullptr;
};
