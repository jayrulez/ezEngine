#include <RHIDX11PCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <Foundation/Configuration/Startup.h>
#include <RHIDX11/Device/DX11Device.h>
#include <RHIDX11/Device/DX11SwapChain.h>

// These will let the driver select the dedicated GPU in favour of the integrated one:
extern "C"
{
  _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
  _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
ezInternal::NewInstance<ezRHIDevice> CreateDX11Device(ezAllocatorBase* pAllocator, const ezRHIDeviceDesc& desc)
{
  return EZ_NEW(pAllocator, ezRHIDX11Device, desc);
}

ezRHIDX11Device::ezRHIDX11Device(const ezRHIDeviceDesc& desc)
  : ezRHIDevice(desc)
{
}

ezRHIDX11Device::~ezRHIDX11Device()
{
}

ezRHISwapChain* ezRHIDX11Device::CreateSwapChain(const ezRHISwapChainDesc& desc)
{
  ezRHIDX11SwapChain* pSwapChain = EZ_NEW(&m_Allocator, ezRHIDX11SwapChain, desc);

  if (!pSwapChain->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pSwapChain);
    return nullptr;
  }

  return pSwapChain;
}

bool ezRHIDX11Device::CreateBuffer(const ezRHIGPUBufferDesc* pDesc, const ezRHISubresourceData* pInitialData, ezRHIGPUBuffer* pBuffer) const
{
  return false;
}

bool ezRHIDX11Device::CreateTexture(const ezRHITextureDesc* pDesc, const ezRHISubresourceData* pInitialData, ezRHITexture* pTexture) const
{
  return false;
}

bool ezRHIDX11Device::CreateShader(ezRHIShaderStage::Enum stage, const void* pShaderBytecode, ezUInt64 BytecodeLength, ezRHIShader* pShader) const
{
  return false;
}

bool ezRHIDX11Device::CreateSampler(const ezRHISamplerStateDesc* pSamplerDesc, ezRHISamplerState* pSamplerState) const
{
  return false;
}

bool ezRHIDX11Device::CreateQueryHeap(const ezRHIQueryHeapDesc* pDesc, ezRHIQueryHeap* pQueryHeap) const
{
  return false;
}

bool ezRHIDX11Device::CreatePipelineState(const ezRHIPipelineStateDesc* pDesc, ezRHIPipelineState* pso) const
{
  return false;
}

bool ezRHIDX11Device::CreateRenderPass(const ezRHIRenderPassDesc* pDesc, ezRHIRenderPass* renderpass) const
{
  return false;
}

ezInt32 ezRHIDX11Device::CreateSubresource(ezRHITexture* texture, ezRHISubresourceType::Enum type, ezUInt32 firstSlice, ezUInt32 sliceCount, ezUInt32 firstMip, ezUInt32 mipCount) const
{
  return ezInt32();
}

ezInt32 ezRHIDX11Device::CreateSubresource(ezRHIGPUBuffer* buffer, ezRHISubresourceType::Enum type, ezUInt64 offset, ezUInt64 size) const
{
  return ezInt32();
}

void ezRHIDX11Device::Map(const ezRHIResourceBase* resource, ezRHIMapping* mapping) const
{
}

void ezRHIDX11Device::Unmap(const ezRHIResourceBase* resource) const
{
}

void ezRHIDX11Device::QueryRead(const ezRHIQueryHeap* resource, ezUInt32 index, ezUInt32 count, ezUInt64* results) const
{
}

void ezRHIDX11Device::SetCommonSampler(const ezRHIStaticSampler* samplerState)
{
}

void ezRHIDX11Device::SetName(ezRHIResourceBase* pResource, const char* name)
{
}

void ezRHIDX11Device::WaitForGPU() const
{
}

ezRHICommandList* ezRHIDX11Device::BeginCommandList(ezRHIQueueType::Enum queue)
{
  return nullptr;
}

void ezRHIDX11Device::SubmitCommandLists()
{
}

ezRHITexture* ezRHIDX11Device::GetBackBuffer(const ezRHISwapChain* swapchain) const
{
  return nullptr;
}
