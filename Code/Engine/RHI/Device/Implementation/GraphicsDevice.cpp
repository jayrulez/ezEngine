#include <RHI/Device/GraphicsDevice.h>
#include <RHI/Util.h>

void RHIGraphicsDevice::ValidateUpdateTextureParameters(RHITexture* texture, ezUInt32 size, ezUInt32 x, ezUInt32 y, ezUInt32 z, ezUInt32 width, ezUInt32 height, ezUInt32 depth, ezUInt32 mipLevel, ezUInt32 arrayLayer)
{
  if (FormatHelpers::IsCompressedFormat(texture->GetFormat()))
  {
    if (x % 4 != 0 || y % 4 != 0 || height % 4 != 0 || width % 4 != 0)
    {
      ezUInt32 mipWidth;
      ezUInt32 mipHeight;
      ezUInt32 mipDepth;
      Util::GetMipDimensions(texture, mipLevel, mipWidth, mipHeight, mipDepth);
      if (width != mipWidth && height != mipHeight)
      {
        EZ_REPORT_FAILURE("Updates to block-compressed textures must use a region that is block-size aligned and sized.");
      }
    }
  }
  ezUInt32 expectedSize = FormatHelpers::GetRegionSize(width, height, depth, texture->GetFormat());
  if (size < expectedSize)
  {
    EZ_REPORT_FAILURE("The data size is less than expected for the given update region. At least {} bytes must be provided, but only {} were.", expectedSize, size);
  }

  // Compressed textures don't necessarily need to have a Texture.Width and Texture.Height that are a multiple of 4.
  // But the mipdata width and height *does* need to be a multiple of 4.
  ezUInt32 roundedTextureWidth, roundedTextureHeight;
  if (FormatHelpers::IsCompressedFormat(texture->GetFormat()))
  {
    roundedTextureWidth = (texture->GetWidth() + 3) / 4 * 4;
    roundedTextureHeight = (texture->GetHeight() + 3) / 4 * 4;
  }
  else
  {
    roundedTextureWidth = texture->GetWidth();
    roundedTextureHeight = texture->GetHeight();
  }

  if (x + width > roundedTextureWidth || y + height > roundedTextureHeight || z + depth > texture->GetDepth())
  {
    EZ_REPORT_FAILURE("The given region does not fit into the Texture.");
  }

  if (mipLevel >= texture->GetMipLevels())
  {
    EZ_REPORT_FAILURE("mipLevel ({}) must be less than the Texture's mip level count ({}).", mipLevel, texture->GetMipLevels());
  }

  ezUInt32 effectiveArrayLayers = texture->GetArrayLayers();
  if ((texture->GetUsage() & RHITextureUsage::Cubemap) != 0)
  {
    effectiveArrayLayers *= 6;
  }
  if (arrayLayer >= effectiveArrayLayers)
  {
    EZ_REPORT_FAILURE("arrayLayer ({}) must be less than the Texture's effective array layer count ({}).", arrayLayer, effectiveArrayLayers);
  }
}


bool RHIGraphicsDevice::GetSyncToVerticalBlank() const
{
  if (GetMainSwapchain() != nullptr)
    return GetMainSwapchain()->GetSyncToVerticalBlank();
  return false;
}

void RHIGraphicsDevice::SetSyncToVerticalBlank(bool value)
{
  if (GetMainSwapchain() == nullptr)
  {
    EZ_REPORT_FAILURE("This GraphicsDevice was created without a main Swapchain. This property cannot be set.");
  }

  GetMainSwapchain()->SetSyncToVerticalBlank(value);
}

ezUInt32 RHIGraphicsDevice::GetUniformBufferMinOffsetAlignment() const
{
  return GetUniformBufferMinOffsetAlignmentCore();
}

ezUInt32 RHIGraphicsDevice::GetStructuredBufferMinOffsetAlignment() const
{
  return GetStructuredBufferMinOffsetAlignmentCore();
}

void RHIGraphicsDevice::SubmitCommands(RHICommandList* commandList)
{
  SubmitCommandsCore(commandList, nullptr);
}

void RHIGraphicsDevice::SubmitCommands(RHICommandList* commandList, RHIFence* fence)
{
  SubmitCommandsCore(commandList, fence);
}

void RHIGraphicsDevice::WaitForFence(RHIFence* fence)
{
  if (!WaitForFence(fence, ezMath::MaxValue<ezUInt64>()))
  {
    EZ_REPORT_FAILURE("The operation timed out before the Fence was signaled.");
  }
}

bool RHIGraphicsDevice::WaitForFence(RHIFence* fence, ezUInt64 nanosecondTimeout)
{
  return WaitForFenceCore(fence, nanosecondTimeout);
}

void RHIGraphicsDevice::WaitForFences(ezDynamicArray<RHIFence*> fences, bool waitAll)
{
  if (!WaitForFences(fences, waitAll, ezMath::MaxValue<ezUInt64>()))
  {
    EZ_REPORT_FAILURE("The operation timed out before the Fence(s) were signaled.");
  }
}

bool RHIGraphicsDevice::WaitForFences(ezDynamicArray<RHIFence*> fences, bool waitAll, ezUInt64 nanosecondTimeout)
{
  return WaitForFencesCore(fences, waitAll, nanosecondTimeout);
}

void RHIGraphicsDevice::ResetFence(RHIFence* fence)
{
  ResetFenceCore(fence);
}

void RHIGraphicsDevice::SwapBuffers()
{
  if (GetMainSwapchain() == nullptr)
  {
    EZ_REPORT_FAILURE("This GraphicsDevice was created without a main Swapchain, so the requested operation cannot be performed.");
  }

  SwapBuffers(GetMainSwapchain());
}

void RHIGraphicsDevice::SwapBuffers(RHISwapchain* swapchain)
{
  SwapBuffersCore(swapchain);
}

RHIFramebuffer* RHIGraphicsDevice::GetSwapchainFramebuffer() const
{
  if (GetMainSwapchain() != nullptr)
  {
    return GetMainSwapchain()->GetFramebuffer();
  }

  return nullptr;
}

void RHIGraphicsDevice::ResizeMainWindow(ezUInt32 width, ezUInt32 height)
{
  if (GetMainSwapchain() == nullptr)
  {
    EZ_REPORT_FAILURE("This GraphicsDevice was created without a main Swapchain, so the requested operation cannot be performed.");
  }

  GetMainSwapchain()->Resize(width, height);
}

void RHIGraphicsDevice::WaitForIdle()
{
  WaitForIdleCore();
  FlushDeferredDisposals();
}

RHIMappedResource* RHIGraphicsDevice::Map(RHIResource* resource, ezEnum<RHIMapMode> mode)
{
  return Map(resource, mode, 0);
}

RHIMappedResource* RHIGraphicsDevice::Map(RHIResource* resource, ezEnum<RHIMapMode> mode, ezUInt32 subresource)
{
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  RHIBuffer* buffer = dynamic_cast<RHIBuffer*>(resource);
  if (buffer != nullptr)
  {
    if ((buffer->GetUsage() & RHIBufferUsage::Dynamic) != RHIBufferUsage::Dynamic && (buffer->GetUsage() & RHIBufferUsage::Staging) != RHIBufferUsage::Staging)
    {
      EZ_REPORT_FAILURE("Buffers must have the Staging or Dynamic usage flag to be mapped.");
    }
    if (subresource != 0)
    {
      EZ_REPORT_FAILURE("Subresource must be 0 for Buffer resources.");
    }
    if ((mode == RHIMapMode::Read || mode == RHIMapMode::ReadWrite) && (buffer->GetUsage() & RHIBufferUsage::Staging) == 0)
    {
      EZ_REPORT_FAILURE("RHIMapMode::Read and RHIMapMode::ReadWrite can only be used on buffers created with RHIBufferUsage::Staging.");
    }
  }
  else
  {
    RHITexture* tex = dynamic_cast<RHITexture*>(resource);
    if (tex != nullptr)
    {
      if ((tex->GetUsage() & RHITextureUsage::Staging) == 0)
      {
        EZ_REPORT_FAILURE("Texture must have the Staging usage flag to be mapped.");
      }
      if (subresource >= tex->GetArrayLayers() * tex->GetMipLevels())
      {
        EZ_REPORT_FAILURE("Subresource must be less than the number of subresources in the Texture being mapped.");
      }
    }
  }
#endif

  return MapCore(resource, mode, subresource);
}

void RHIGraphicsDevice::Unmap(RHIResource* resource)
{
  Unmap(resource, 0);
}

void RHIGraphicsDevice::Unmap(RHIResource* resource, ezUInt32 subresource)
{
  UnmapCore(resource, subresource);
}

void RHIGraphicsDevice::UpdateTexture(RHITexture* texture, ezUInt8* source, ezUInt32 size, ezUInt32 x, ezUInt32 y, ezUInt32 z, ezUInt32 width, ezUInt32 height, ezUInt32 depth, ezUInt32 mipLevel, ezUInt32 arrayLayer)
{
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  ValidateUpdateTextureParameters(texture, size, x, y, z, width, height, depth, mipLevel, arrayLayer);
#endif
  UpdateTextureCore(texture, source, size, x, y, z, width, height, depth, mipLevel, arrayLayer);
}

bool RHIGraphicsDevice::GetPixelFormatSupport(ezEnum<RHIPixelFormat> format, ezEnum<RHITextureType> type, ezEnum<RHITextureUsage> usage)
{
  RHIPixelFormatProperties properties;
  return GetPixelFormatSupportCore(format, type, usage, properties);
}

bool RHIGraphicsDevice::GetPixelFormatSupport(ezEnum<RHIPixelFormat> format, ezEnum<RHITextureType> type, ezEnum<RHITextureUsage> usage, RHIPixelFormatProperties& properties)
{
  return GetPixelFormatSupportCore(format, type, usage, properties);
}

RHISampler* RHIGraphicsDevice::GetPointSampler() const
{
  return PointSampler;
}

RHISampler* RHIGraphicsDevice::GetLinearSampler() const
{
  return LinearSampler;
}

RHISampler* RHIGraphicsDevice::GetAniso4xSampler()
{
  if (!GetFeatures().SamplerAnisotropySupported())
  {
    EZ_REPORT_FAILURE("GraphicsDevice.Aniso4xSampler cannot be used unless GraphicsDeviceFeatures.SamplerAnisotropy is supported.");
  }
  EZ_VERIFY(Aniso4xSampler != nullptr, "Aniso4xSampler is null.");
  return Aniso4xSampler;
}

void RHIGraphicsDevice::PostDeviceCreated()
{
  PointSampler = GetResourceFactory()->CreateSampler(RHISamplerDescription::Point);
  LinearSampler = GetResourceFactory()->CreateSampler(RHISamplerDescription::Linear);
  if (GetFeatures().SamplerAnisotropySupported())
  {
    Aniso4xSampler = GetResourceFactory()->CreateSampler(RHISamplerDescription::Aniso4x);
  }
}
