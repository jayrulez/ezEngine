#include <Foundation/Threading/TaskSystem.h>
#include <RHI/Backends/D3D11/D3D11GraphicsDevice.h>

#include <dxgi1_3.h>
#include <dxgidebug.h>

struct FenceWaitTask final : public ezTask
{
  virtual void Execute() override
  {
    if (Event != nullptr)
    {
      Result = Event->WaitForSignal(ezTime::Nanoseconds((double)Timeout));
      Signaled = true;
    }
  }

  ezThreadSignal::WaitResult GetResult() const
  {
    return Result;
  }

  bool GetSignaled() const
  {
    return Signaled;
  }

  void SetEvent(ezThreadSignal* event, ezUInt64 timeout)
  {
    if (Event == nullptr)
    {
      Event = event;
      Timeout = timeout;
    }
    else
    {
      EZ_REPORT_FAILURE("Cannot modify the event for a Fence wait task.");
    }
  }

private:
  ezThreadSignal::WaitResult Result = ezThreadSignal::WaitResult::Timeout;
  ezThreadSignal* Event = nullptr;
  ezUInt64 Timeout = 0;
  bool Signaled = false;
};

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
    //MainSwapchain = new D3D11Swapchain(this, desc);
    MainSwapchain = EZ_DEFAULT_NEW(D3D11Swapchain, this, desc);
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
  if (SUCCEEDED(Device->CheckFeatureSupport(D3D11_FEATURE_DOUBLES, &dataDoublesSupport, sizeof(D3D11_FEATURE_DATA_DOUBLES))))
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

  //ResourceFactory = new D3D11ResourceFactory(this);
  ResourceFactory = EZ_DEFAULT_NEW(D3D11ResourceFactory, this);

  PostDeviceCreated();
}

void D3D11GraphicsDevice::SubmitCommandsCore(RHICommandList* commandList, RHIFence* fence)
{
  D3D11CommandList* d3d11CL = Util::AssertSubtype<RHICommandList, D3D11CommandList>(commandList);

  {
    ezLock lock(ImmediateContextMutex);

    if (d3d11CL->GetDeviceCommandList() != nullptr) // CommandList may have been reset in the meantime (resized swapchain).
    {
      ImmediateContext->ExecuteCommandList(d3d11CL->GetDeviceCommandList(), false);
      d3d11CL->OnCompleted();
    }
  }

  D3D11Fence* d3d11Fence = dynamic_cast<D3D11Fence*>(fence);
  if (d3d11Fence)
  {
    d3d11Fence->Set();
  }
}

bool D3D11GraphicsDevice::WaitForFenceCore(RHIFence* fence, ezUInt64 nanosecondTimeout)
{
  return Util::AssertSubtype<RHIFence, D3D11Fence>(fence)->Wait(nanosecondTimeout);
}

bool D3D11GraphicsDevice::WaitForFencesCore(ezDynamicArray<RHIFence*> fences, bool waitAll, ezUInt64 nanosecondTimeout)
{
  ezDynamicArray<ezThreadSignal*> events = GetResetEventArray(fences.GetCount());


  for (ezUInt32 i = 0; i < fences.GetCount(); i++)
  {
    events[i] = Util::AssertSubtype<RHIFence, D3D11Fence>(fences[i])->GetResetSignal();
  }
  bool result = true;

  ezTaskGroupID waitGroupId = ezTaskSystem::CreateTaskGroup(ezTaskPriority::ThisFrame);
  ezDeque<ezSharedPtr<FenceWaitTask>> waitTasks;

  for (ezUInt32 i = 0; i < events.GetCount(); i++)
  {
    ezSharedPtr<FenceWaitTask> pTask;
    pTask->SetEvent(events[i], nanosecondTimeout);

    ezTaskSystem::AddTaskToGroup(waitGroupId, pTask);
    waitTasks.PushBack(pTask);
  }
  if (waitAll)
  {
    ezTaskSystem::WaitForGroup(waitGroupId);

    for (auto& waitedTask : waitTasks)
    {
      if (waitedTask->GetResult() == ezThreadSignal::WaitResult::Timeout)
        result = false;
    }
  }
  else
  {
    ezTaskSystem::StartTaskGroup(waitGroupId);
    ezThreadSignal::WaitResult waitedResult = ezThreadSignal::WaitResult::Timeout;
    ezTaskSystem::WaitForCondition([&waitTasks, &waitedResult] {
      for (;;)
      {
        for (auto& waitedTask : waitTasks)
        {
          if (waitedTask->GetSignaled())
          {
            waitedResult = waitedTask->GetResult();
            true;
          }
        }
      }

      return false;
    });

    result = waitedResult == ezThreadSignal::WaitResult::Signaled;
  }

  ReturnResetEventArray(events);

  return result;
}

ezDynamicArray<ezThreadSignal*> D3D11GraphicsDevice::GetResetEventArray(ezUInt32 length)
{
  {
    ezLock lock(ResetEventsMutex);
    for (ezUInt32 i = ResetEvents.GetCount() - 1; i > 0; i--)
    {
      ezDynamicArray<ezThreadSignal*> array = ResetEvents[i];
      if (array.GetCount() == length)
      {
        ResetEvents.RemoveAndCopy(array);
        return array;
      }
    }
  }

  ezDynamicArray<ezThreadSignal*> newArray;
  newArray.SetCountUninitialized(length);
  return newArray;
}

void D3D11GraphicsDevice::ReturnResetEventArray(ezDynamicArray<ezThreadSignal*> array)
{
  ezLock lock(ResetEventsMutex);
  ResetEvents.PushBack(array);
}

void D3D11GraphicsDevice::ResetFenceCore(RHIFence* fence)
{
  Util::AssertSubtype<RHIFence, D3D11Fence>(fence)->Reset();
}

void D3D11GraphicsDevice::SwapBuffersCore(RHISwapchain* swapchain)
{
  ezLock lock(ImmediateContextMutex);
  D3D11Swapchain* d3d11SC = Util::AssertSubtype<RHISwapchain, D3D11Swapchain>(swapchain);
  HRESULT hr = d3d11SC->GetIDXGISwapChain()->Present(d3d11SC->GetSyncInterval(), 0);
}

RHIMappedResource* D3D11GraphicsDevice::MapCore(RHIResource* resource, ezEnum<RHIMapMode> mode, ezUInt32 subresource)
{
  RHIMappedResourceCacheKey key = RHIMappedResourceCacheKey(resource, subresource);
  ezLock resourceLock(MappedResourceMutex);
  RHIMappedResourceInfo info;

  if (MappedResources.TryGetValue(key, info))
  {
    if (info.Mode != mode)
    {
      EZ_REPORT_FAILURE("The given resource was already mapped with a different MapMode.");
    }

    info.RefCount += 1;
    MappedResources[key] = info;
  }
  else
  {
    // No current mapping exists -- create one.

    D3D11DeviceBuffer* buffer = dynamic_cast<D3D11DeviceBuffer*>(resource);

    if (buffer != nullptr)
    {
      {
        ezLock lock(ImmediateContextMutex);
        D3D11_MAPPED_SUBRESOURCE msr;
        HRESULT hr = ImmediateContext->Map(buffer->GetBuffer(), 0,
          D3D11Formats::RHIToD3D11MapMode((buffer->GetUsage() & RHIBufferUsage::Dynamic) == RHIBufferUsage::Dynamic, mode), 0, &msr);

        ezUInt64* data = reinterpret_cast<ezUInt64*>(msr.pData);

        //info.Resource = new RHIMappedResource(resource, mode, data, buffer->GetSize());
        info.Resource = EZ_DEFAULT_NEW(RHIMappedResource, resource, mode, data, buffer->GetSize());
        info.RefCount = 1;
        info.Mode = mode;
        MappedResources.Insert(key, info);
      }
    }
    else
    {
      D3D11Texture* texture = Util::AssertSubtype<RHIResource, D3D11Texture>(resource);
      EZ_ASSERT_ALWAYS(texture->GetFlags().IsSet(RHIResourceFlags::Mappable), "Resource is not mappable.");

      {
        ezLock lock(ImmediateContextMutex);
        D3D11_MAPPED_SUBRESOURCE msr;

        HRESULT hr = ImmediateContext->Map(
          texture->GetDeviceTexture(),
          subresource,
          D3D11Formats::RHIToD3D11MapMode(false, mode),
          0,
          &msr);

        ezUInt64* data = reinterpret_cast<ezUInt64*>(msr.pData);

        //info.Resource = new RHIMappedResource(
        //  resource,
        //  mode,
        //  data,
        //  texture->GetHeight() * msr.RowPitch,
        //  subresource,
        //  msr.RowPitch,
        //  msr.DepthPitch);

        info.Resource = EZ_DEFAULT_NEW(RHIMappedResource,
          resource,
          mode,
          data,
          texture->GetHeight() * msr.RowPitch,
          subresource,
          msr.RowPitch,
          msr.DepthPitch);

        info.RefCount = 1;
        info.Mode = mode;
        MappedResources.Insert(key, info);
      }
    }
  }

  return info.Resource;
}

void D3D11GraphicsDevice::UnmapCore(RHIResource* resource, ezUInt32 subresource)
{
  RHIMappedResourceCacheKey key = RHIMappedResourceCacheKey(resource, subresource);
  bool commitUnmap = false;

  {
    ezLock resourceLock(MappedResourceMutex);
    RHIMappedResourceInfo info;
    if (!MappedResources.TryGetValue(key, info))
    {
      EZ_REPORT_FAILURE("The given resource ({resource}) is not mapped.");
      return;
    }

    info.RefCount -= 1;
    commitUnmap = info.RefCount == 0;
    if (commitUnmap)
    {
      {
        ezLock lock(ImmediateContextMutex);
        D3D11DeviceBuffer* buffer = dynamic_cast<D3D11DeviceBuffer*>(resource);
        if (buffer != nullptr)
        {
          ImmediateContext->Unmap(buffer->GetBuffer(), 0);
        }
        else
        {
          D3D11Texture* texture = Util::AssertSubtype<RHIResource, D3D11Texture>(resource);
          ImmediateContext->Unmap(texture->GetDeviceTexture(), subresource);
        }

        bool result = MappedResources.Remove(key);
        EZ_ASSERT_DEBUG(result, "Failed to removed maped resource entry.");
      }
    }
  }
}

void D3D11GraphicsDevice::UpdateTextureCore(RHITexture* texture, ezUInt8* source, ezUInt32 size, ezUInt32 x, ezUInt32 y, ezUInt32 z, ezUInt32 width, ezUInt32 height, ezUInt32 depth, ezUInt32 mipLevel, ezUInt32 arrayLayer)
{
  D3D11Texture* d3dTex = Util::AssertSubtype<RHITexture, D3D11Texture>(texture);
  bool useMap = (texture->GetUsage() & RHITextureUsage::Staging) == RHITextureUsage::Staging;
  if (useMap)
  {
    ezUInt32 subresource = texture->CalculateSubresourceIndex(mipLevel, arrayLayer);
    RHIMappedResourceCacheKey key = RHIMappedResourceCacheKey(texture, subresource);
    RHIMappedResource* map = MapCore(texture, RHIMapMode::Write, subresource);

    ezUInt32 denseRowSize = FormatHelpers::GetRowPitch(width, texture->GetFormat());
    ezUInt32 denseSliceSize = FormatHelpers::GetDepthPitch(denseRowSize, height, texture->GetFormat());

    Util::CopyTextureRegion(
      source,
      0, 0, 0,
      denseRowSize, denseSliceSize,
      map->GetData(),
      x, y, z,
      map->GetRowPitch(), map->GetDepthPitch(),
      width, height, depth,
      texture->GetFormat());

    UnmapCore(texture, subresource);
  }
  else
  {
    ezUInt32 subresource = D3D11Util::ComputeSubresource(mipLevel, texture->GetMipLevels(), arrayLayer);
    D3D11_BOX resourceRegion = {
      x,
      (x + width),
      y,
      z,
      (y + height),
      (z + depth),
    };

    ezUInt32 srcRowPitch = FormatHelpers::GetRowPitch(width, texture->GetFormat());
    ezUInt32 srcDepthPitch = FormatHelpers::GetDepthPitch(srcRowPitch, height, texture->GetFormat());

    {
      ezLock lock(ImmediateContextMutex);
      ImmediateContext->UpdateSubresource(
        d3dTex->GetDeviceTexture(),
        subresource,
        &resourceRegion,
        source,
        srcRowPitch,
        srcDepthPitch);
    }
  }
}

void D3D11GraphicsDevice::UpdateBufferCore(RHIBuffer* buffer, ezUInt32 bufferOffset, ezUInt8* source, ezUInt32 size)
{
  D3D11DeviceBuffer* d3dBuffer = Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(buffer);
  if (size == 0)
  {
    return;
  }

  bool isDynamic = (buffer->GetUsage() & RHIBufferUsage::Dynamic) == RHIBufferUsage::Dynamic;
  bool isStaging = (buffer->GetUsage() & RHIBufferUsage::Staging) == RHIBufferUsage::Staging;
  bool isUniformBuffer = (buffer->GetUsage() & RHIBufferUsage::UniformBuffer) == RHIBufferUsage::UniformBuffer;
  bool updateFullBuffer = bufferOffset == 0 && size == buffer->GetSize();
  bool useUpdateSubresource = (!isDynamic && !isStaging) && (!isUniformBuffer || updateFullBuffer);
  bool useMap = (isDynamic && updateFullBuffer) || isStaging;

  if (useUpdateSubresource)
  {
    D3D11_BOX subregion = D3D11_BOX();

    subregion.left = bufferOffset;
    subregion.right = (size + bufferOffset);
    subregion.bottom = 1;
    subregion.back = 1;
    subregion.top = 0;
    subregion.front = 0;

    D3D11_BOX* subregionPtr = &subregion;

    if (isUniformBuffer)
    {
      subregionPtr = nullptr;
    }

    {
      ezLock lock(ImmediateContextMutex);
      ImmediateContext->UpdateSubresource(d3dBuffer->GetBuffer(), 0, subregionPtr, reinterpret_cast<void*>(source), 0, 0);
    }
  }
  else if (useMap)
  {
    RHIMappedResource* mr = MapCore(buffer, RHIMapMode::Write, 0);
    if (size < 1024)
    {
      ezMemoryUtils::Copy(reinterpret_cast<ezUInt8*>(mr->GetData()) + bufferOffset, source, size);
    }
    else
    {
      ezMemoryUtils::CopyOverlapped(
        reinterpret_cast<ezUInt8*>(mr->GetData()) + bufferOffset,
        source,
        //buffer->GetSize(),
        size);
    }
    UnmapCore(buffer, 0);
  }
  else
  {
    D3D11DeviceBuffer* staging = GetFreeStagingBuffer(size);
    UpdateBuffer(staging, 0, source, size);
    D3D11_BOX sourceRegion = D3D11_BOX{0, 0, 0, size, 1, 1};

    {
      ezLock lock(ImmediateContextMutex);
      ImmediateContext->CopySubresourceRegion(
        d3dBuffer->GetBuffer(), 0, bufferOffset, 0, 0,
        staging->GetBuffer(), 0,
        &sourceRegion);
    }


    {
      ezLock lock(StagingResourceMutex);
      AvailableStagingBuffers.PushBack(staging);
    }
  }
}

void D3D11GraphicsDevice::DisposeCore()
{
  for (RHIBuffer* buffer : AvailableStagingBuffers)
  {
    buffer->Dispose();
  }
  AvailableStagingBuffers.Clear();


  if (MainSwapchain != nullptr)
  {
    MainSwapchain->Dispose();
    EZ_DEFAULT_DELETE(MainSwapchain);
  }

  ResourceFactory->Dispose();
  EZ_DEFAULT_DELETE(ResourceFactory);

  ImmediateContext->Release();

  ID3D11Debug* deviceDebug = nullptr;
  HRESULT hr = Device->QueryInterface<ID3D11Debug>(&deviceDebug);

  Device->Release();
  DXGIAdapter->Release();

  IDXGIDebug1* dxgiDebug = nullptr;
  hr = DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), reinterpret_cast<void**>(&dxgiDebug));

  // Report live objects using DXGI if available (DXGIGetDebugInterface1 will fail on pre Windows 8 OS).
  if (SUCCEEDED(hr))
  {
    if (deviceDebug != nullptr)
    {
      deviceDebug->Release();
    }

    dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_ALL | DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
    dxgiDebug->Release();
  }
  else
  {
    // Need to enable native debugging to see live objects in VisualStudio console.
    if (deviceDebug != nullptr)
    {
      deviceDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
      deviceDebug->Release();
    }
  }
}

ezEnum<RHITextureSampleCount> D3D11GraphicsDevice::GetSampleCountLimit(ezEnum<RHIPixelFormat> format, bool depthFormat)
{
  DXGI_FORMAT dxgiFormat = D3D11Formats::ToDxgiFormat(format, depthFormat);
  if (CheckFormatMultisample(dxgiFormat, 32))
  {
    return RHITextureSampleCount::Count32;
  }
  else if (CheckFormatMultisample(dxgiFormat, 16))
  {
    return RHITextureSampleCount::Count16;
  }
  else if (CheckFormatMultisample(dxgiFormat, 8))
  {
    return RHITextureSampleCount::Count8;
  }
  else if (CheckFormatMultisample(dxgiFormat, 4))
  {
    return RHITextureSampleCount::Count4;
  }
  else if (CheckFormatMultisample(dxgiFormat, 2))
  {
    return RHITextureSampleCount::Count2;
  }

  return RHITextureSampleCount::Count1;
}

bool D3D11GraphicsDevice::GetPixelFormatSupportCore(ezEnum<RHIPixelFormat> format, ezEnum<RHITextureType> type, ezBitflags<RHITextureUsage> usage, RHIPixelFormatProperties& properties)
{
  if (D3D11Formats::IsUnsupportedFormat(format))
  {
    properties = RHIPixelFormatProperties();
    return false;
  }

  DXGI_FORMAT dxgiFormat = D3D11Formats::ToDxgiFormat(format, (usage & RHITextureUsage::DepthStencil) != 0);

  D3D11_FORMAT_SUPPORT fs;

  HRESULT hr = Device->CheckFormatSupport(dxgiFormat, (ezUInt32*)&fs);

  if ((usage & RHITextureUsage::RenderTarget) != 0 && (fs & RHITextureUsage::RenderTarget) == 0 ||
      (usage & RHITextureUsage::DepthStencil) != 0 && (fs & RHITextureUsage::DepthStencil) == 0 ||
      (usage & RHITextureUsage::Sampled) != 0 && (fs & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE) == 0 ||
      (usage & RHITextureUsage::Cubemap) != 0 && (fs & D3D11_FORMAT_SUPPORT_TEXTURECUBE) == 0 ||
      (usage & RHITextureUsage::Storage) != 0 && (fs & D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW) == 0)
  {
    properties = RHIPixelFormatProperties();
    return false;
  }

  const ezUInt32 MaxTextureDimension = 16384;
  const ezUInt32 MaxVolumeExtent = 2048;

  ezUInt32 sampleCounts = 0;
  if (CheckFormatMultisample(dxgiFormat, 1))
  {
    sampleCounts |= (1 << 0);
  }
  if (CheckFormatMultisample(dxgiFormat, 2))
  {
    sampleCounts |= (1 << 1);
  }
  if (CheckFormatMultisample(dxgiFormat, 4))
  {
    sampleCounts |= (1 << 2);
  }
  if (CheckFormatMultisample(dxgiFormat, 8))
  {
    sampleCounts |= (1 << 3);
  }
  if (CheckFormatMultisample(dxgiFormat, 16))
  {
    sampleCounts |= (1 << 4);
  }
  if (CheckFormatMultisample(dxgiFormat, 32))
  {
    sampleCounts |= (1 << 5);
  }

  properties = RHIPixelFormatProperties(
    MaxTextureDimension,
    type == RHITextureType::Texture1D ? 1 : MaxTextureDimension,
    type != RHITextureType::Texture3D ? 1 : MaxVolumeExtent,
    ezMath::MaxValue<ezUInt32>(),
    type == RHITextureType::Texture3D ? 1 : MaxVolumeExtent,
    sampleCounts);
  return true;
}

ezResult D3D11GraphicsDevice::InitializeDevice(IDXGIAdapter* dxgiAdapter, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, ezUInt32 flags)
{
  D3D_FEATURE_LEVEL FeatureLevels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3};

  for (ezUInt32 FeatureLevelIdx = 0; FeatureLevelIdx < EZ_ARRAY_SIZE(FeatureLevels); FeatureLevelIdx++)
  {
    if (SUCCEEDED(D3D11CreateDevice(dxgiAdapter, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, FeatureLevels, 1, D3D11_SDK_VERSION, &device, (D3D_FEATURE_LEVEL*)&FeatureLevel, &immediateContext)))
      break;
  }

  if (immediateContext == nullptr)
    return EZ_FAILURE;
  return EZ_SUCCESS;
}

bool D3D11GraphicsDevice::CheckFormatMultisample(DXGI_FORMAT format, ezUInt32 sampleCount)
{
  ezUInt32 numQualityLevels = 0;
  HRESULT hr = Device->CheckMultisampleQualityLevels(format, sampleCount, &numQualityLevels);
  return numQualityLevels != 0;
}

D3D11DeviceBuffer* D3D11GraphicsDevice::GetFreeStagingBuffer(ezUInt32 size)
{
  ezLock lock(StagingResourceMutex);
  {
    for (D3D11DeviceBuffer* buffer : AvailableStagingBuffers)
    {
      if (buffer->GetSize() >= size)
      {
        AvailableStagingBuffers.RemoveAndCopy(buffer);
        return buffer;
      }
    }
  }

  RHIBuffer* staging = ResourceFactory->CreateBuffer(RHIBufferDescription(size, RHIBufferUsage::Staging));

  return Util::AssertSubtype<RHIBuffer, D3D11DeviceBuffer>(staging);
}

RHID3D11DeviceOptions D3D11GraphicsDevice::MergeOptions(const RHIGraphicsDeviceOptions& options, RHID3D11DeviceOptions& d3D11DeviceOptions)
{
  if (options.Debug)
  {
    d3D11DeviceOptions.DeviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
  }

  return d3D11DeviceOptions;
}


EZ_STATICLINK_FILE(RHI, RHI_Backends_D3D11_Implementation_D3D11GraphicsDevice);
