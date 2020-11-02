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
    return RHIGraphicsBackend::Direct3D11;
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

protected:
  virtual ezUInt32 GetUniformBufferMinOffsetAlignmentCore() const override { return 256u; }
  virtual ezUInt32 GetStructuredBufferMinOffsetAlignmentCore() const override { return 16u; }
  virtual void SubmitCommandsCore(RHICommandList* commandList, RHIFence* fence) override;
  virtual bool WaitForFenceCore(RHIFence* fence, ezUInt64 nanosecondTimeout) override;
  virtual bool WaitForFencesCore(ezDynamicArray<RHIFence*> fences, bool waitAll, ezUInt64 nanosecondTimeout) override;
  virtual void ResetFenceCore(RHIFence* fence) override { Util::AssertSubtype<RHIFence, D3D11Fence>(fence)->Reset(); }
  virtual void SwapBuffersCore(RHISwapchain* swapchain) override;
  virtual void WaitForIdleCore() override {}

  virtual RHIMappedResource* MapCore(RHIResource* resource, ezEnum<RHIMapMode> mode, ezUInt32 subresource) override;
  virtual void UnmapCore(RHIResource* resource, ezUInt32 subresource) override;
  virtual void UpdateTextureCore(
    RHITexture* texture,
    ezUInt8* source,
    ezUInt32 size,
    ezUInt32 x, ezUInt32 y, ezUInt32 z,
    ezUInt32 width, ezUInt32 height, ezUInt32 depth,
    ezUInt32 mipLevel, ezUInt32 arrayLayer) override;
  virtual void UpdateBufferCore(RHIBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size) override;
  virtual void DisposeCore() override;
  virtual ezEnum<RHITextureSampleCount> GetSampleCountLimit(ezEnum<RHIPixelFormat> format, bool depthFormat) override;
  virtual bool GetPixelFormatSupportCore(
    ezEnum<RHIPixelFormat> format,
    ezEnum<RHITextureType> type,
    ezBitflags<RHITextureUsage> usage,
    RHIPixelFormatProperties& properties) override;

private:
  ezResult InitializeDevice(IDXGIAdapter* dxgiAdapter, ID3D11Device* device, ID3D11DeviceContext* immediateContext, ezUInt32 flags);
  bool CheckFormatMultisample(DXGI_FORMAT format, ezUInt32 sampleCount);
  D3D11DeviceBuffer* GetFreeStagingBuffer(ezUInt32 size);

  ezUInt32 CalculateMipSize(ezUInt32 mipLevel, ezUInt32 baseSize)
  {
    baseSize = baseSize >> mipLevel;
    return baseSize > 0u ? baseSize : 1u;
  }

private:
  bool _supportsCommandLists;
  RHIGraphicsDeviceFeatures Features;
  IDXGIAdapter* DXGIAdapter = nullptr;
  ID3D11Device* Device = nullptr;
  ID3D11DeviceContext* ImmediateContext = nullptr;
  D3D11ResourceFactory* ResourceFactory = nullptr;
  D3D11Swapchain* MainSwapchain = nullptr;
  bool _supportsConcurrentResources;
  ezMutex ImmediateContextMutex;
  ezUInt32 FeatureLevel;

  ezMutex MappedResourceMutex;
  ezHashTable<RHIMappedResourceCacheKey, RHIMappedResourceInfo> MappedResources;

  ezMutex StagingResourceMutex;
  ezDynamicArray<D3D11DeviceBuffer*> AvailableStagingBuffers;
  ezDynamicArray<ezThreadSignal> ResetEvents;
  ezMutex ResetEventsMutex;
};
