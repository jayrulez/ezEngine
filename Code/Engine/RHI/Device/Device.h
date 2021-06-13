#pragma once

#include <Foundation/Memory/CommonAllocators.h>
#include <RHI/Descriptors/Descriptors.h>

#include <RHI/RHIDLL.h>

struct ezRHIQueueType
{
  enum Enum
  {
    Graphics,
    Compute,

    ENUM_COUNT
  };
};

class EZ_RHI_DLL ezRHIDevice
{
protected:
  friend class ezMemoryUtils;

  ezRHIDevice(const ezRHIDeviceDesc& desc);
  virtual ~ezRHIDevice();

public:
  virtual ezRHISwapChain* CreateSwapChain(const ezRHISwapChainDesc& desc) = 0;
  virtual bool CreateBuffer(const ezRHIGPUBufferDesc* pDesc, const ezRHISubresourceData* pInitialData, ezRHIGPUBuffer* pBuffer) const = 0;
  virtual bool CreateTexture(const ezRHITextureDesc* pDesc, const ezRHISubresourceData* pInitialData, ezRHITexture* pTexture) const = 0;
  virtual bool CreateShader(ezRHIShaderStage::Enum stage, const void* pShaderBytecode, ezUInt64 BytecodeLength, ezRHIShader* pShader) const = 0;
  virtual bool CreateSampler(const ezRHISamplerStateDesc* pSamplerDesc, ezRHISamplerState* pSamplerState) const = 0;
  virtual bool CreateQueryHeap(const ezRHIQueryHeapDesc* pDesc, ezRHIQueryHeap* pQueryHeap) const = 0;
  virtual bool CreatePipelineState(const ezRHIPipelineStateDesc* pDesc, ezRHIPipelineState* pso) const = 0;
  virtual bool CreateRenderPass(const ezRHIRenderPassDesc* pDesc, ezRHIRenderPass* renderpass) const = 0;

  virtual ezInt32 CreateSubresource(ezRHITexture* texture, ezRHISubresourceType::Enum type, ezUInt32 firstSlice, ezUInt32 sliceCount, ezUInt32 firstMip, ezUInt32 mipCount) const = 0;
  virtual ezInt32 CreateSubresource(ezRHIGPUBuffer* buffer, ezRHISubresourceType::Enum type, ezUInt64 offset, ezUInt64 size = ~0) const = 0;

  virtual ezInt32 GetDescriptorIndex(const ezRHIResourceBase* resource, ezRHISubresourceType::Enum type, ezInt32 subresource = -1) const { return -1; };
  virtual ezInt32 GetDescriptorIndex(const ezRHISamplerState* sampler) const { return -1; };

  virtual void WriteShadingRateValue(ezRHIShadingRate::Enum rate, void* dest) const {};

  virtual void Map(const ezRHIResourceBase* resource, ezRHIMapping* mapping) const = 0;
  virtual void Unmap(const ezRHIResourceBase* resource) const = 0;
  virtual void QueryRead(const ezRHIQueryHeap* heap, ezUInt32 index, ezUInt32 count, ezUInt64* results) const = 0;

  virtual void SetCommonSampler(const ezRHIStaticSampler* sam) = 0;

  virtual void SetName(ezRHIResourceBase* pResource, const char* name) = 0;

  // Begin a new command list for GPU command recording.
  //	This will be valid until SubmitCommandLists() is called.
  virtual ezRHICommandList* BeginCommandList(ezRHIQueueType::Enum queue = ezRHIQueueType::Graphics) = 0;
  // Submit all command list that were used with BeginCommandList before this call.
  //	This will make every command list to be in "available" state and restarts them
  virtual void SubmitCommandLists() = 0;

  virtual void WaitForGPU() const = 0;
  virtual void ClearPipelineStateCache(){};

  constexpr ezUInt64 GetFrameCount() const { return m_FrameCount; }

  inline bool CheckCapability(ezRHIDeviceCapabilityFlags::Enum capability) const { return m_Capabilities.IsSet(capability); }

  static constexpr ezUInt32 GetBufferCount() { return s_BufferCount; }

  constexpr bool IsDebugDevice() const { return m_DebugDevice; }

  constexpr ezUInt64 GetShaderIdentifierSize() const { return m_ShaderIdentifierSize; }
  constexpr ezUInt32 GetVariableRateShadingTileSize() const { return m_VariableRateShadingTileSize; }
  constexpr ezUInt64 GetTimestampFrequency() const { return m_TimestampFrequency; }

  virtual ezRHIShaderFormat::Enum GetShaderFormat() const { return ezRHIShaderFormat::None; }

  virtual ezRHITexture* GetBackBuffer(const ezRHISwapChain* swapchain) const = 0;

protected:
  static const ezUInt32 s_BufferCount = 2;
  ezUInt64 m_FrameCount = 0;
  bool m_DebugDevice = false;
  ezBitflags<ezRHIDeviceCapabilityFlags> m_Capabilities;
  ezUInt64 m_ShaderIdentifierSize = 0;
  ezUInt32 m_VariableRateShadingTileSize = 0;
  ezUInt64 m_TimestampFrequency = 0;

  ezProxyAllocator m_Allocator;
  ezLocalAllocatorWrapper m_AllocatorWrapper;
  ezRHIDeviceDesc m_Desc;

private:

  private:
  static ezRHIDevice* s_pDefaultDevice;

  public:
  static void SetDefaultDevice(ezRHIDevice* pDefaultDevice);
    static ezRHIDevice* GetDefaultDevice();
  static bool HasDefaultDevice();
};

#include <RHI/Device/Implementation/Device_inl.h>
