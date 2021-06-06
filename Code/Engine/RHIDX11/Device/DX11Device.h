#pragma once

#include <RHI/Device/Device.h>
#include <RHIDX11/RHIDX11DLL.h>

#include <DXGI1_3.h>
#include <d3d11_3.h>
#include <wrl/client.h>

class EZ_RHIDX11_DLL ezRHIDX11Device : public ezRHIDevice
{
public:
  friend ezInternal::NewInstance<ezRHIDevice> CreateDX11Device(ezAllocatorBase* pAllocator, const ezRHIDeviceDesc& desc);
  ezRHIDX11Device(const ezRHIDeviceDesc& desc);

  bool CreateSwapChain(const ezRHISwapChainDesc*, ezRHISwapChain* swapChain) const override;
  bool CreateBuffer(const ezRHIGPUBufferDesc* pDesc, const ezRHISubresourceData* pInitialData, ezRHIGPUBuffer* pBuffer) const override;
  bool CreateTexture(const ezRHITextureDesc* pDesc, const ezRHISubresourceData* pInitialData, ezRHITexture* pTexture) const override;
  bool CreateShader(ezRHIShaderStage::Enum stage, const void* pShaderBytecode, ezUInt64 BytecodeLength, ezRHIShader* pShader) const override;
  bool CreateSampler(const ezRHISamplerStateDesc* pSamplerDesc, ezRHISamplerState* pSamplerState) const override;
  bool CreateQueryHeap(const ezRHIGPUQueryHeapDesc* pDesc, ezRHIGPUQueryHeap* pQueryHeap) const override;
  bool CreatePipelineState(const ezRHIPipelineStateDesc* pDesc, ezRHIPipelineState* pso) const override;
  bool CreateRenderPass(const ezRHIRenderPassDesc* pDesc, ezRHIRenderPass* renderpass) const override;

  ezInt32 CreateSubresource(ezRHITexture* texture, ezRHISubresourceType::Enum type, ezUInt32 firstSlice, ezUInt32 sliceCount, ezUInt32 firstMip, ezUInt32 mipCount) const override;
  ezInt32 CreateSubresource(ezRHIGPUBuffer* buffer, ezRHISubresourceType::Enum type, ezUInt64 offset, ezUInt64 size = ~0) const override;

  void Map(const ezRHIResourceBase* resource, ezRHIMapping* mapping) const override;
  void Unmap(const ezRHIResourceBase* resource) const override;
  void QueryRead(const ezRHIGPUQueryHeap* resource, ezUInt32 index, ezUInt32 count, ezUInt64* results) const override;

  void SetCommonSampler(const ezRHIStaticSampler* samplerState) override;

  void SetName(ezRHIResourceBase* pResource, const char* name) override;

  void WaitForGPU() const override;

  ezRHICommandList* BeginCommandList(ezRHIQueueType::Enum queue = ezRHIQueueType::Graphics) override;
  void SubmitCommandLists() override;

  ezRHIShaderFormat::Enum GetShaderFormat() const override { return ezRHIShaderFormat::HLSL5; }

  ezRHITexture* GetBackBuffer(const ezRHISwapChain* swapchain) const override;

public:
  EZ_ALWAYS_INLINE Microsoft::WRL::ComPtr<IDXGIFactory2> GetDXGIFactory() const
  {
    return m_pDXGIFactory;
  }
  EZ_ALWAYS_INLINE Microsoft::WRL::ComPtr<ID3D11Device> GetD3D11Device() const
  {
    return m_pDevice;
  }

private:
  Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
  Microsoft::WRL::ComPtr<IDXGIFactory2> m_pDXGIFactory;
};
