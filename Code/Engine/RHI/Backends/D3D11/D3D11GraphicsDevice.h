#pragma once

#include <RHI/Backends/D3D11/D3D11DeviceOptions.h>
#include <RHI/Device/GraphicsDevice.h>
#include <optional>

#include <RHI/Backends/D3D11/D3D11ResourceFactory.h>
#include <RHI/Backends/D3D11/D3D11Swapchain.h>
#include <RHI/Resources/MappedResourceCacheKey.h>
#include <RHI/Resources/MappedResourceInfo.h>

struct ID3D11Device;
struct IDXGIAdapter;
struct ID3D11DeviceContext;



class D3D11GraphicsDevice : public RHIGraphicsDevice
{
private:
  static RHID3D11DeviceOptions MergeOptions(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions);

public:
  D3D11GraphicsDevice(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions, const std::optional<RHISwapchainDescription>& swapchainDesc);

  D3D11GraphicsDevice(const RHID3D11DeviceOptions& options, const std::optional<RHISwapchainDescription>& swapchainDesc);

  ID3D11Device* GetDevice() const { return Device; }

  IDXGIAdapter* GetAdapter() const { return DXGIAdapter; }

  ezEnum<RHIGraphicsBackend> GetBackendType() const override
  {
    return BackendType;
  }

  bool GetIsUvOriginTopLeft() const override { return true; }

  bool GetIsDepthRangeZeroToOne() const override { return true; }

  bool IsClipSpaceYInverted() const override { return false; }

  virtual RHIResourceFactory* GetResourceFactory() const override { return ResourceFactory; }

  virtual RHISwapchain* GetMainSwapchain() const override { return MainSwapchain; }

  virtual const RHIGraphicsDeviceFeatures& GetFeatures() const override
  {
    return Features;
  }

  bool SupportsCommandLists() const { return _supportsCommandLists; }
  bool SupportsConcurrentResources() const { return _supportsConcurrentResources; }

  private:
  ezResult InitializeDevice(IDXGIAdapter* dxgiAdapter, ID3D11Device* device, ID3D11DeviceContext* immediateContext, ezUInt32 flags);

private:
  bool _supportsCommandLists;
  RHIGraphicsDeviceFeatures Features;
  IDXGIAdapter* DXGIAdapter = nullptr;
  ID3D11Device* Device = nullptr;
  ID3D11DeviceContext* ImmediateContext;
  D3D11ResourceFactory* ResourceFactory;
  D3D11Swapchain* MainSwapchain;
  bool _supportsConcurrentResources;
  ezMutex ImmediateContextMutex;
  ezUInt32 FeatureLevel;

  ezMutex MappedResourceMutex;
  ezHashTable<RHIMappedResourceCacheKey, RHIMappedResourceInfo> MappedResources;

  ezMutex StagingResourceMutex;
  ezDynamicArray<D3D11DeviceBuffer> AvailableStagingBuffers;
  ezEnum<RHIGraphicsBackend> BackendType = RHIGraphicsBackend::Direct3D11;
};
