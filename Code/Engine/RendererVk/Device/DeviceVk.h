#pragma once

#include <Foundation/Types/Bitflags.h>
#include <Foundation/Types/UniquePtr.h>
#include <RendererVk/RendererVkDLL.h>
#include <RendererFoundation/Device/Device.h>

#include <vulkan/vulkan.h>

class ezGALPassVk;

EZ_DEFINE_AS_POD_TYPE(VkQueue);

/// \brief The Vk device implementation of the graphics abstraction layer.
class EZ_RENDERERVK_DLL ezGALDeviceVk : public ezGALDevice
{
private:
  friend ezInternal::NewInstance<ezGALDevice> CreateVkDevice(ezAllocatorBase* pAllocator, const ezGALDeviceCreationDescription& Description);
  ezGALDeviceVk(const ezGALDeviceCreationDescription& Description);

public:
  virtual ~ezGALDeviceVk();

public:

  void ReportLiveGpuObjects();

  // These functions need to be implemented by a render API abstraction
protected:
  // Init & shutdown functions

  /// \brief Internal version of device init that allows to modify device creation flags and graphics adapter.
  ///
  /// \param pUsedAdapter
  ///   Null means default adapter.
  ezResult InitPlatform(ezUInt32 flags, void* pUsedAdapter);

  virtual ezResult InitPlatform() override;
  virtual ezResult ShutdownPlatform() override;

  // Pipeline & Pass functions

  virtual void BeginPipelinePlatform(const char* szName) override;
  virtual void EndPipelinePlatform() override;

  virtual ezGALPass* BeginPassPlatform(const char* szName) override;
  virtual void EndPassPlatform(ezGALPass* pPass) override;


  // State creation functions

  virtual ezGALBlendState* CreateBlendStatePlatform(const ezGALBlendStateCreationDescription& Description) override;
  virtual void DestroyBlendStatePlatform(ezGALBlendState* pBlendState) override;

  virtual ezGALDepthStencilState* CreateDepthStencilStatePlatform(const ezGALDepthStencilStateCreationDescription& Description) override;
  virtual void DestroyDepthStencilStatePlatform(ezGALDepthStencilState* pDepthStencilState) override;

  virtual ezGALRasterizerState* CreateRasterizerStatePlatform(const ezGALRasterizerStateCreationDescription& Description) override;
  virtual void DestroyRasterizerStatePlatform(ezGALRasterizerState* pRasterizerState) override;

  virtual ezGALSamplerState* CreateSamplerStatePlatform(const ezGALSamplerStateCreationDescription& Description) override;
  virtual void DestroySamplerStatePlatform(ezGALSamplerState* pSamplerState) override;


  // Resource creation functions

  virtual ezGALShader* CreateShaderPlatform(const ezGALShaderCreationDescription& Description) override;
  virtual void DestroyShaderPlatform(ezGALShader* pShader) override;

  virtual ezGALBuffer* CreateBufferPlatform(const ezGALBufferCreationDescription& Description, ezArrayPtr<const ezUInt8> pInitialData) override;
  virtual void DestroyBufferPlatform(ezGALBuffer* pBuffer) override;

  virtual ezGALTexture* CreateTexturePlatform(const ezGALTextureCreationDescription& Description, ezArrayPtr<ezGALSystemMemoryDescription> pInitialData) override;
  virtual void DestroyTexturePlatform(ezGALTexture* pTexture) override;

  virtual ezGALResourceView* CreateResourceViewPlatform(ezGALResourceBase* pResource, const ezGALResourceViewCreationDescription& Description) override;
  virtual void DestroyResourceViewPlatform(ezGALResourceView* pResourceView) override;

  virtual ezGALRenderTargetView* CreateRenderTargetViewPlatform(ezGALTexture* pTexture, const ezGALRenderTargetViewCreationDescription& Description) override;
  virtual void DestroyRenderTargetViewPlatform(ezGALRenderTargetView* pRenderTargetView) override;

  ezGALUnorderedAccessView* CreateUnorderedAccessViewPlatform(ezGALResourceBase* pResource, const ezGALUnorderedAccessViewCreationDescription& Description) override;
  virtual void DestroyUnorderedAccessViewPlatform(ezGALUnorderedAccessView* pResource) override;

  // Other rendering creation functions

  virtual ezGALSwapChain* CreateSwapChainPlatform(const ezGALSwapChainCreationDescription& Description) override;
  virtual void DestroySwapChainPlatform(ezGALSwapChain* pSwapChain) override;

  virtual ezGALFence* CreateFencePlatform() override;
  virtual void DestroyFencePlatform(ezGALFence* pFence) override;

  virtual ezGALQuery* CreateQueryPlatform(const ezGALQueryCreationDescription& Description) override;
  virtual void DestroyQueryPlatform(ezGALQuery* pQuery) override;

  virtual ezGALVertexDeclaration* CreateVertexDeclarationPlatform(const ezGALVertexDeclarationCreationDescription& Description) override;
  virtual void DestroyVertexDeclarationPlatform(ezGALVertexDeclaration* pVertexDeclaration) override;

  // Timestamp functions

  virtual ezGALTimestampHandle GetTimestampPlatform() override;
  virtual ezResult GetTimestampResultPlatform(ezGALTimestampHandle hTimestamp, ezTime& result) override;

  // Swap chain functions

  virtual void PresentPlatform(ezGALSwapChain* pSwapChain, bool bVSync) override;

  // Misc functions

  virtual void BeginFramePlatform() override;
  virtual void EndFramePlatform() override;

  virtual void SetPrimarySwapChainPlatform(ezGALSwapChain* pSwapChain) override;

  virtual void FillCapabilitiesPlatform() override;

  /// \endcond


  // Internal
public:
  EZ_ALWAYS_INLINE VkDevice GetVkDevice() const;
  EZ_ALWAYS_INLINE VkInstance GetVkInstance() const;
  EZ_ALWAYS_INLINE VkPhysicalDevice GetVkPhysicalDevice() const;
  EZ_ALWAYS_INLINE const ezStaticArray<ezInt32, ezInternal::Vk::QueueType::Count>& GetVkQueueFamilyIndices() const;
  EZ_ALWAYS_INLINE ezInt32 GetVkPresentQueueFamilyIndex() const;

  static PFN_vkDebugUtilsMessengerCallbackEXT s_VkDebugCallback;

private:

  ezResult CreateInstance();
  ezResult SetupDebugMessenger();
  ezResult SetupPhysicalDevice();
  ezResult CreateLogicalDevice();

private:
  friend class ezGALCommandEncoderImplVk;

  VkInstance m_VkInstance;
  VkPhysicalDevice m_VkPhysicalDevice;
  VkDevice m_VkDevice;

  VkDebugUtilsMessengerEXT m_VkDebugMessenger;

  ezStaticArray<VkQueue, ezInternal::Vk::QueueType::Count> m_VkQueues;
  ezStaticArray<ezInt32, ezInternal::Vk::QueueType::Count> m_QueueFamilyIndices;
  ezInt32 m_VkPresentQueueFamilyIndex;
  VkQueue m_VkPresentQueue;
};

#include <RendererVk/Device/Implementation/DeviceVk_inl.h>
