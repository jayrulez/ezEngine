#pragma once

#include <Foundation/Containers/HashTable.h>
#include <Foundation/Containers/IdTable.h>
#include <Foundation/Memory/CommonAllocators.h>
#include <Foundation/Strings/HashedString.h>
#include <RHI/Renderer/Basics.h>
#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Device/DeviceCapabilities.h>
#include <RHI/Instance/QueryInterface.h>

class ezColor;

/// \brief The ezRenderDevice class is the primary interface for interactions with rendering APIs
class EZ_RHI_DLL ezRHIRenderDevice : public QueryInterface
{
public:
  ezEvent<const ezRHIRenderDeviceEvent&> m_Events;

  // Init & shutdown functions

 virtual ezResult Init() = 0;
  virtual ezResult Shutdown() = 0;

  // Pipeline & Pass functions

 virtual void BeginPipeline(const char* szName) = 0;
  virtual void EndPipeline() = 0;

  virtual ezRHIPass* BeginPass(const char* szName) = 0;
  virtual void EndPass(ezRHIPass* pPass) = 0;

  // State creation functions

 virtual ezRHIBlendStateHandle CreateBlendState(const ezRHIBlendStateCreationDescription& Description) = 0;
  virtual void DestroyBlendState(ezRHIBlendStateHandle hBlendState) = 0;

 virtual ezRHIDepthStencilStateHandle CreateDepthStencilState(const ezRHIDepthStencilStateCreationDescription& Description) = 0;
  virtual void DestroyDepthStencilState(ezRHIDepthStencilStateHandle hDepthStencilState) = 0;

 virtual ezRHIRasterizerStateHandle CreateRasterizerState(const ezRHIRasterizerStateCreationDescription& Description) = 0;
  virtual void DestroyRasterizerState(ezRHIRasterizerStateHandle hRasterizerState) = 0;

 virtual ezRHISamplerStateHandle CreateSamplerState(const ezRHISamplerStateCreationDescription& Description) = 0;
  virtual void DestroySamplerState(ezRHISamplerStateHandle hSamplerState) = 0;

  // Resource creation functions

  virtual ezRHIShaderHandle CreateShader(const ezRHIShaderCreationDescription& Description) = 0;
  virtual void DestroyShader(ezRHIShaderHandle hShader) = 0;

  virtual ezRHIBufferHandle CreateBuffer(const ezRHIBufferCreationDescription& Description, ezArrayPtr<const ezUInt8> pInitialData = ezArrayPtr<const ezUInt8>()) = 0;
  virtual void DestroyBuffer(ezRHIBufferHandle hBuffer) = 0;

  // Helper functions for buffers (for common, simple use cases)

 virtual ezRHIBufferHandle CreateVertexBuffer(ezUInt32 uiVertexSize, ezUInt32 uiVertexCount, ezArrayPtr<const ezUInt8> pInitialData = ezArrayPtr<const ezUInt8>()) = 0;
  virtual ezRHIBufferHandle CreateIndexBuffer(ezRHIIndexType::Enum IndexType, ezUInt32 uiIndexCount, ezArrayPtr<const ezUInt8> pInitialData = ezArrayPtr<const ezUInt8>()) = 0;
  virtual ezRHIBufferHandle CreateConstantBuffer(ezUInt32 uiBufferSize) = 0;

 virtual ezRHITextureHandle CreateTexture(const ezRHITextureCreationDescription& Description, ezArrayPtr<ezRHISystemMemoryDescription> pInitialData = ezArrayPtr<ezRHISystemMemoryDescription>()) = 0;
  virtual ezResult ReplaceExisitingNativeObject(ezRHITextureHandle hTexture, void* pExisitingNativeObject) = 0;
  virtual void DestroyTexture(ezRHITextureHandle hTexture) = 0;

  virtual ezRHITextureHandle CreateProxyTexture(ezRHITextureHandle hParentTexture, ezUInt32 uiSlice) = 0;
  virtual void DestroyProxyTexture(ezRHITextureHandle hProxyTexture) = 0;

  // Resource views
  virtual ezRHIResourceViewHandle GetDefaultResourceView(ezRHITextureHandle hTexture) = 0;
  virtual ezRHIResourceViewHandle GetDefaultResourceView(ezRHIBufferHandle hBuffer) = 0;

  virtual ezRHIResourceViewHandle CreateResourceView(const ezRHIResourceViewCreationDescription& Description) = 0;
  virtual void DestroyResourceView(ezRHIResourceViewHandle hResourceView) = 0;

  // Render target views
  virtual ezRHIRenderTargetViewHandle GetDefaultRenderTargetView(ezRHITextureHandle hTexture) = 0;

  virtual ezRHIRenderTargetViewHandle CreateRenderTargetView(const ezRHIRenderTargetViewCreationDescription& Description) = 0;
  virtual void DestroyRenderTargetView(ezRHIRenderTargetViewHandle hRenderTargetView) = 0;

  // Unordered access views
  virtual ezRHIUnorderedAccessViewHandle CreateUnorderedAccessView(const ezRHIUnorderedAccessViewCreationDescription& Description) = 0;
  virtual void DestroyUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView) = 0;


  // Other rendering creation functions

  virtual ezRHISwapChainHandle CreateSwapChain(const ezRHISwapChainCreationDescription& Description) = 0;
  virtual void DestroySwapChain(ezRHISwapChainHandle hSwapChain) = 0;

 virtual ezRHIFenceHandle CreateFence() = 0;
  virtual void DestroyFence(ezRHIFenceHandle& hFence) = 0;

  virtual ezRHIQueryHandle CreateQuery(const ezRHIQueryCreationDescription& Description) = 0;
  virtual void DestroyQuery(ezRHIQueryHandle hQuery) = 0;

 virtual ezRHIVertexDeclarationHandle CreateVertexDeclaration(const ezRHIVertexDeclarationCreationDescription& Description) = 0;
  virtual void DestroyVertexDeclaration(ezRHIVertexDeclarationHandle hVertexDeclaration) = 0;

  // Timestamp functions

  virtual ezResult GetTimestampResult(ezRHITimestampHandle hTimestamp, ezTime& result) = 0;

  /// \todo Map functions to save on memcpys

  // Swap chain functions

  virtual void Present(ezRHISwapChainHandle hSwapChain, bool bVSync) = 0;
  virtual ezRHITextureHandle GetBackBufferTextureFromSwapChain(ezRHISwapChainHandle hSwapChain) = 0;


  // Misc functions

  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;

  virtual void SetPrimarySwapChain(ezRHISwapChainHandle hSwapChain) = 0;
  virtual ezRHISwapChainHandle GetPrimarySwapChain() const = 0;

  virtual ezRHITimestampHandle GetTimestamp() = 0;

  virtual const ezRHIRenderDeviceCreationDescription* GetDescription() const = 0;


  virtual const ezRHISwapChain* GetSwapChain(ezRHISwapChainHandle hSwapChain) const = 0;
  virtual const ezRHIShader* GetShader(ezRHIShaderHandle hShader) const = 0;
  virtual const ezRHITexture* GetTexture(ezRHITextureHandle hTexture) const = 0;
  virtual const ezRHIBuffer* GetBuffer(ezRHIBufferHandle hBuffer) const = 0;
  virtual const ezRHIDepthStencilState* GetDepthStencilState(ezRHIDepthStencilStateHandle hDepthStencilState) const = 0;
  virtual const ezRHIBlendState* GetBlendState(ezRHIBlendStateHandle hBlendState) const = 0;
  virtual const ezRHIRasterizerState* GetRasterizerState(ezRHIRasterizerStateHandle hRasterizerState) const = 0;
  virtual const ezRHIVertexDeclaration* GetVertexDeclaration(ezRHIVertexDeclarationHandle hVertexDeclaration) const = 0;
  virtual const ezRHISamplerState* GetSamplerState(ezRHISamplerStateHandle hSamplerState) const = 0;
  virtual const ezRHIResourceView* GetResourceView(ezRHIResourceViewHandle hResourceView) const = 0;
  virtual const ezRHIRenderTargetView* GetRenderTargetView(ezRHIRenderTargetViewHandle hRenderTargetView) const = 0;
  virtual const ezRHIUnorderedAccessView* GetUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView) const = 0;
  virtual const ezRHIFence* GetFence(ezRHIFenceHandle hFence) const = 0;
  virtual const ezRHIQuery* GetQuery(ezRHIQueryHandle hQuery) const = 0;

 virtual const ezRHIRenderDeviceCapabilities& GetCapabilities() const = 0;

  virtual ezUInt64 GetMemoryConsumptionForTexture(const ezRHITextureCreationDescription& Description) const = 0;
  virtual ezUInt64 GetMemoryConsumptionForBuffer(const ezRHIBufferCreationDescription& Description) const = 0;

  static void SetDefaultDevice(ezRHIRenderDevice* pDefaultDevice);
  static ezRHIRenderDevice* GetDefaultDevice();
  static bool HasDefaultDevice() ;

  // public in case someone external needs to lock multiple operations
  mutable ezMutex m_Mutex;

private:
  static ezRHIRenderDevice* s_pDefaultDevice;

protected:
  friend class ezMemoryUtils;
  ezRHIRenderDevice() = default;

  virtual ~ezRHIRenderDevice() = default;
};

// static
EZ_ALWAYS_INLINE void ezRHIRenderDevice::SetDefaultDevice(ezRHIRenderDevice* pDefaultDevice)
{
  s_pDefaultDevice = pDefaultDevice;
}

// static
EZ_ALWAYS_INLINE ezRHIRenderDevice* ezRHIRenderDevice::GetDefaultDevice()
{
  EZ_ASSERT_DEBUG(s_pDefaultDevice != nullptr, "Default device not set.");
  return s_pDefaultDevice;
}

// static
EZ_ALWAYS_INLINE bool ezRHIRenderDevice::HasDefaultDevice()
{
  return s_pDefaultDevice != nullptr;
}

EZ_RHI_DLL ezInternal::NewInstance<ezRHIRenderDevice> CreateRenderDevice(ezAllocatorBase* pAllocator, const ezRHIRenderDeviceCreationDescription& desc);
EZ_RHI_DLL void DestroyRenderDevice(ezRHIRenderDevice* pRenderDevice);


