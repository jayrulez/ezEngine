
#pragma once

#include <Foundation/Containers/HashTable.h>
#include <Foundation/Containers/IdTable.h>
#include <Foundation/Memory/CommonAllocators.h>
#include <Foundation/Strings/HashedString.h>
#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Device/DeviceCapabilities.h>
#include <RHI/Renderer/Basics.h>

class ezColor;

/// \brief The ezRenderDevice class is the primary interface for interactions with rendering APIs
/// It contains a set of (non-virtual) functions to set state, create resources etc. which rely on
/// API specific implementations provided by protected virtual functions.
/// Redundant state changes are prevented at the platform independent level in the non-virtual functions.
class EZ_RHI_DLL ezRHIDevice
{
public:
  ezEvent<const ezRHIDeviceEvent&> m_Events;

  // Init & shutdown functions

  ezResult Init();
  ezResult Shutdown();

  // Pipeline & Pass functions

  void BeginPipeline(const char* szName);
  void EndPipeline();

  ezRHIPass* BeginPass(const char* szName);
  void EndPass(ezRHIPass* pPass);

  // State creation functions

  ezRHIBlendStateHandle CreateBlendState(const ezRHIBlendStateCreationDescription& Description);
  void DestroyBlendState(ezRHIBlendStateHandle hBlendState);

  ezRHIDepthStencilStateHandle CreateDepthStencilState(const ezRHIDepthStencilStateCreationDescription& Description);
  void DestroyDepthStencilState(ezRHIDepthStencilStateHandle hDepthStencilState);

  ezRHIRasterizerStateHandle CreateRasterizerState(const ezRHIRasterizerStateCreationDescription& Description);
  void DestroyRasterizerState(ezRHIRasterizerStateHandle hRasterizerState);

  ezRHISamplerStateHandle CreateSamplerState(const ezRHISamplerStateCreationDescription& Description);
  void DestroySamplerState(ezRHISamplerStateHandle hSamplerState);

  // Resource creation functions

  ezRHIShaderHandle CreateShader(const ezRHIShaderCreationDescription& Description);
  void DestroyShader(ezRHIShaderHandle hShader);

  ezRHIBufferHandle CreateBuffer(const ezRHIBufferCreationDescription& Description, ezArrayPtr<const ezUInt8> pInitialData = ezArrayPtr<const ezUInt8>());
  void DestroyBuffer(ezRHIBufferHandle hBuffer);

  // Helper functions for buffers (for common, simple use cases)

  ezRHIBufferHandle CreateVertexBuffer(ezUInt32 uiVertexSize, ezUInt32 uiVertexCount, ezArrayPtr<const ezUInt8> pInitialData = ezArrayPtr<const ezUInt8>());
  ezRHIBufferHandle CreateIndexBuffer(ezRHIIndexType::Enum IndexType, ezUInt32 uiIndexCount, ezArrayPtr<const ezUInt8> pInitialData = ezArrayPtr<const ezUInt8>());
  ezRHIBufferHandle CreateConstantBuffer(ezUInt32 uiBufferSize);

  ezRHITextureHandle CreateTexture(const ezRHITextureCreationDescription& Description, ezArrayPtr<ezRHISystemMemoryDescription> pInitialData = ezArrayPtr<ezRHISystemMemoryDescription>());
  ezResult ReplaceExisitingNativeObject(ezRHITextureHandle hTexture, void* pExisitingNativeObject);
  void DestroyTexture(ezRHITextureHandle hTexture);

  ezRHITextureHandle CreateProxyTexture(ezRHITextureHandle hParentTexture, ezUInt32 uiSlice);
  void DestroyProxyTexture(ezRHITextureHandle hProxyTexture);

  // Resource views
  ezRHIResourceViewHandle GetDefaultResourceView(ezRHITextureHandle hTexture);
  ezRHIResourceViewHandle GetDefaultResourceView(ezRHIBufferHandle hBuffer);

  ezRHIResourceViewHandle CreateResourceView(const ezRHIResourceViewCreationDescription& Description);
  void DestroyResourceView(ezRHIResourceViewHandle hResourceView);

  // Render target views
  ezRHIRenderTargetViewHandle GetDefaultRenderTargetView(ezRHITextureHandle hTexture);

  ezRHIRenderTargetViewHandle CreateRenderTargetView(const ezRHIRenderTargetViewCreationDescription& Description);
  void DestroyRenderTargetView(ezRHIRenderTargetViewHandle hRenderTargetView);

  // Unordered access views
  ezRHIUnorderedAccessViewHandle CreateUnorderedAccessView(const ezRHIUnorderedAccessViewCreationDescription& Description);
  void DestroyUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView);


  // Other rendering creation functions

  ezRHISwapChainHandle CreateSwapChain(const ezRHISwapChainCreationDescription& Description);
  void DestroySwapChain(ezRHISwapChainHandle hSwapChain);

  ezRHIFenceHandle CreateFence();
  void DestroyFence(ezRHIFenceHandle& hFence);

  ezRHIQueryHandle CreateQuery(const ezRHIQueryCreationDescription& Description);
  void DestroyQuery(ezRHIQueryHandle hQuery);

  ezRHIVertexDeclarationHandle CreateVertexDeclaration(const ezRHIVertexDeclarationCreationDescription& Description);
  void DestroyVertexDeclaration(ezRHIVertexDeclarationHandle hVertexDeclaration);

  // Timestamp functions

  ezResult GetTimestampResult(ezRHITimestampHandle hTimestamp, ezTime& result);

  /// \todo Map functions to save on memcpys

  // Swap chain functions

  void Present(ezRHISwapChainHandle hSwapChain, bool bVSync);
  ezRHITextureHandle GetBackBufferTextureFromSwapChain(ezRHISwapChainHandle hSwapChain);


  // Misc functions

  void BeginFrame();
  void EndFrame();

  void SetPrimarySwapChain(ezRHISwapChainHandle hSwapChain);
  ezRHISwapChainHandle GetPrimarySwapChain() const;

  ezRHITimestampHandle GetTimestamp();

  const ezRHIDeviceCreationDescription* GetDescription() const;


  const ezRHISwapChain* GetSwapChain(ezRHISwapChainHandle hSwapChain) const;
  const ezRHIShader* GetShader(ezRHIShaderHandle hShader) const;
  const ezRHITexture* GetTexture(ezRHITextureHandle hTexture) const;
  const ezRHIBuffer* GetBuffer(ezRHIBufferHandle hBuffer) const;
  const ezRHIDepthStencilState* GetDepthStencilState(ezRHIDepthStencilStateHandle hDepthStencilState) const;
  const ezRHIBlendState* GetBlendState(ezRHIBlendStateHandle hBlendState) const;
  const ezRHIRasterizerState* GetRasterizerState(ezRHIRasterizerStateHandle hRasterizerState) const;
  const ezRHIVertexDeclaration* GetVertexDeclaration(ezRHIVertexDeclarationHandle hVertexDeclaration) const;
  const ezRHISamplerState* GetSamplerState(ezRHISamplerStateHandle hSamplerState) const;
  const ezRHIResourceView* GetResourceView(ezRHIResourceViewHandle hResourceView) const;
  const ezRHIRenderTargetView* GetRenderTargetView(ezRHIRenderTargetViewHandle hRenderTargetView) const;
  const ezRHIUnorderedAccessView* GetUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView) const;
  const ezRHIFence* GetFence(ezRHIFenceHandle hFence) const;
  const ezRHIQuery* GetQuery(ezRHIQueryHandle hQuery) const;

  const ezRHIDeviceCapabilities& GetCapabilities() const;

  virtual ezUInt64 GetMemoryConsumptionForTexture(const ezRHITextureCreationDescription& Description) const;
  virtual ezUInt64 GetMemoryConsumptionForBuffer(const ezRHIBufferCreationDescription& Description) const;

  static void SetDefaultDevice(ezRHIDevice* pDefaultDevice);
  static ezRHIDevice* GetDefaultDevice();
  static bool HasDefaultDevice();

  // public in case someone external needs to lock multiple operations
  mutable ezMutex m_Mutex;

private:
  static ezRHIDevice* s_pDefaultDevice;

protected:
  ezRHIDevice(const ezRHIDeviceCreationDescription& Description);

  virtual ~ezRHIDevice();

  template <typename IdTableType, typename ReturnType>
  ReturnType* Get(typename IdTableType::TypeOfId hHandle, const IdTableType& IdTable) const;

  void DestroyViews(ezRHIResourceBase* pResource);

  template <typename HandleType>
  void AddDeadObject(ezUInt32 uiType, HandleType handle);

  template <typename HandleType>
  void ReviveDeadObject(ezUInt32 uiType, HandleType handle);

  void DestroyDeadObjects();

  /// \brief Asserts that either this device supports multi-threaded resource creation, or that this function is executed on the main thread.
  void VerifyMultithreadedAccess() const;

  ezProxyAllocator m_Allocator;
  ezLocalAllocatorWrapper m_AllocatorWrapper;

  using ShaderTable = ezIdTable<ezRHIShaderHandle::IdType, ezRHIShader*, ezLocalAllocatorWrapper>;
  using BlendStateTable = ezIdTable<ezRHIBlendStateHandle::IdType, ezRHIBlendState*, ezLocalAllocatorWrapper>;
  using DepthStencilStateTable = ezIdTable<ezRHIDepthStencilStateHandle::IdType, ezRHIDepthStencilState*, ezLocalAllocatorWrapper>;
  using RasterizerStateTable = ezIdTable<ezRHIRasterizerStateHandle::IdType, ezRHIRasterizerState*, ezLocalAllocatorWrapper>;
  using BufferTable = ezIdTable<ezRHIBufferHandle::IdType, ezRHIBuffer*, ezLocalAllocatorWrapper>;
  using TextureTable = ezIdTable<ezRHITextureHandle::IdType, ezRHITexture*, ezLocalAllocatorWrapper>;
  using ResourceViewTable = ezIdTable<ezRHIResourceViewHandle::IdType, ezRHIResourceView*, ezLocalAllocatorWrapper>;
  using SamplerStateTable = ezIdTable<ezRHISamplerStateHandle::IdType, ezRHISamplerState*, ezLocalAllocatorWrapper>;
  using RenderTargetViewTable = ezIdTable<ezRHIRenderTargetViewHandle::IdType, ezRHIRenderTargetView*, ezLocalAllocatorWrapper>;
  using UnorderedAccessViewTable = ezIdTable<ezRHIUnorderedAccessViewHandle::IdType, ezRHIUnorderedAccessView*, ezLocalAllocatorWrapper>;
  using SwapChainTable = ezIdTable<ezRHISwapChainHandle::IdType, ezRHISwapChain*, ezLocalAllocatorWrapper>;
  using FenceTable = ezIdTable<ezRHIFenceHandle::IdType, ezRHIFence*, ezLocalAllocatorWrapper>;
  using QueryTable = ezIdTable<ezRHIQueryHandle::IdType, ezRHIQuery*, ezLocalAllocatorWrapper>;
  using VertexDeclarationTable = ezIdTable<ezRHIVertexDeclarationHandle::IdType, ezRHIVertexDeclaration*, ezLocalAllocatorWrapper>;

  ShaderTable m_Shaders;
  BlendStateTable m_BlendStates;
  DepthStencilStateTable m_DepthStencilStates;
  RasterizerStateTable m_RasterizerStates;
  BufferTable m_Buffers;
  TextureTable m_Textures;
  ResourceViewTable m_ResourceViews;
  SamplerStateTable m_SamplerStates;
  RenderTargetViewTable m_RenderTargetViews;
  UnorderedAccessViewTable m_UnorderedAccessViews;
  SwapChainTable m_SwapChains;
  FenceTable m_Fences;
  QueryTable m_Queries;
  VertexDeclarationTable m_VertexDeclarations;


  // Hash tables used to prevent state object duplication
  ezHashTable<ezUInt32, ezRHIBlendStateHandle, ezHashHelper<ezUInt32>, ezLocalAllocatorWrapper> m_BlendStateTable;
  ezHashTable<ezUInt32, ezRHIDepthStencilStateHandle, ezHashHelper<ezUInt32>, ezLocalAllocatorWrapper> m_DepthStencilStateTable;
  ezHashTable<ezUInt32, ezRHIRasterizerStateHandle, ezHashHelper<ezUInt32>, ezLocalAllocatorWrapper> m_RasterizerStateTable;
  ezHashTable<ezUInt32, ezRHISamplerStateHandle, ezHashHelper<ezUInt32>, ezLocalAllocatorWrapper> m_SamplerStateTable;
  ezHashTable<ezUInt32, ezRHIVertexDeclarationHandle, ezHashHelper<ezUInt32>, ezLocalAllocatorWrapper> m_VertexDeclarationTable;

  struct DeadObject
  {
    EZ_DECLARE_POD_TYPE();

    ezUInt32 m_uiType;
    ezUInt32 m_uiHandle;
  };

  ezDynamicArray<DeadObject, ezLocalAllocatorWrapper> m_DeadObjects;

  ezRHIDeviceCreationDescription m_Description;

  ezRHISwapChainHandle m_hPrimarySwapChain;

  ezRHIDeviceCapabilities m_Capabilities;

  // Deactivate Doxygen document generation for the following block. (API abstraction only)
  /// \cond

  // These functions need to be implemented by a render API abstraction
protected:
  friend class ezMemoryUtils;

  // Init & shutdown functions

  virtual ezResult InitPlatform() = 0;
  virtual ezResult ShutdownPlatform() = 0;

  // Pipeline & Pass functions

  virtual void BeginPipelinePlatform(const char* szName) = 0;
  virtual void EndPipelinePlatform() = 0;

  virtual ezRHIPass* BeginPassPlatform(const char* szName) = 0;
  virtual void EndPassPlatform(ezRHIPass* pPass) = 0;

  // State creation functions

  virtual ezRHIBlendState* CreateBlendStatePlatform(const ezRHIBlendStateCreationDescription& Description) = 0;
  virtual void DestroyBlendStatePlatform(ezRHIBlendState* pBlendState) = 0;

  virtual ezRHIDepthStencilState* CreateDepthStencilStatePlatform(const ezRHIDepthStencilStateCreationDescription& Description) = 0;
  virtual void DestroyDepthStencilStatePlatform(ezRHIDepthStencilState* pDepthStencilState) = 0;

  virtual ezRHIRasterizerState* CreateRasterizerStatePlatform(const ezRHIRasterizerStateCreationDescription& Description) = 0;
  virtual void DestroyRasterizerStatePlatform(ezRHIRasterizerState* pRasterizerState) = 0;

  virtual ezRHISamplerState* CreateSamplerStatePlatform(const ezRHISamplerStateCreationDescription& Description) = 0;
  virtual void DestroySamplerStatePlatform(ezRHISamplerState* pSamplerState) = 0;

  // Resource creation functions

  virtual ezRHIShader* CreateShaderPlatform(const ezRHIShaderCreationDescription& Description) = 0;
  virtual void DestroyShaderPlatform(ezRHIShader* pShader) = 0;

  virtual ezRHIBuffer* CreateBufferPlatform(const ezRHIBufferCreationDescription& Description, ezArrayPtr<const ezUInt8> pInitialData) = 0;
  virtual void DestroyBufferPlatform(ezRHIBuffer* pBuffer) = 0;

  virtual ezRHITexture* CreateTexturePlatform(const ezRHITextureCreationDescription& Description, ezArrayPtr<ezRHISystemMemoryDescription> pInitialData) = 0;
  virtual void DestroyTexturePlatform(ezRHITexture* pTexture) = 0;

  virtual ezRHIResourceView* CreateResourceViewPlatform(ezRHIResourceBase* pResource, const ezRHIResourceViewCreationDescription& Description) = 0;
  virtual void DestroyResourceViewPlatform(ezRHIResourceView* pResourceView) = 0;

  virtual ezRHIRenderTargetView* CreateRenderTargetViewPlatform(ezRHITexture* pTexture, const ezRHIRenderTargetViewCreationDescription& Description) = 0;
  virtual void DestroyRenderTargetViewPlatform(ezRHIRenderTargetView* pRenderTargetView) = 0;

  virtual ezRHIUnorderedAccessView* CreateUnorderedAccessViewPlatform(ezRHIResourceBase* pResource, const ezRHIUnorderedAccessViewCreationDescription& Description) = 0;
  virtual void DestroyUnorderedAccessViewPlatform(ezRHIUnorderedAccessView* pUnorderedAccessView) = 0;

  // Other rendering creation functions

  virtual ezRHISwapChain* CreateSwapChainPlatform(const ezRHISwapChainCreationDescription& Description) = 0;
  virtual void DestroySwapChainPlatform(ezRHISwapChain* pSwapChain) = 0;

  virtual ezRHIFence* CreateFencePlatform() = 0;
  virtual void DestroyFencePlatform(ezRHIFence* pFence) = 0;

  virtual ezRHIQuery* CreateQueryPlatform(const ezRHIQueryCreationDescription& Description) = 0;
  virtual void DestroyQueryPlatform(ezRHIQuery* pQuery) = 0;

  virtual ezRHIVertexDeclaration* CreateVertexDeclarationPlatform(const ezRHIVertexDeclarationCreationDescription& Description) = 0;
  virtual void DestroyVertexDeclarationPlatform(ezRHIVertexDeclaration* pVertexDeclaration) = 0;

  // Timestamp functions

  virtual ezRHITimestampHandle GetTimestampPlatform() = 0;
  virtual ezResult GetTimestampResultPlatform(ezRHITimestampHandle hTimestamp, ezTime& result) = 0;

  // Swap chain functions

  virtual void PresentPlatform(ezRHISwapChain* pSwapChain, bool bVSync) = 0;

  // Misc functions

  virtual void BeginFramePlatform() = 0;
  virtual void EndFramePlatform() = 0;

  virtual void SetPrimarySwapChainPlatform(ezRHISwapChain* pSwapChain) = 0;

  virtual void FillCapabilitiesPlatform() = 0;

  /// \endcond

private:
  bool m_bBeginFrameCalled = false;
  bool m_bBeginPipelineCalled = false;
  bool m_bBeginPassCalled = false;
};

#include <RHI/Renderer/Device/Implementation/Device_inl.h>
