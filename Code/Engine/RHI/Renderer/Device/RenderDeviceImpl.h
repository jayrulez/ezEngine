#pragma once

#include <RHI/Renderer/Device/RenderDevice.h>

#include <RHI/Instance/Instance.h>

class EZ_RHI_DLL ezRHIRenderDeviceImpl : public ezRHIRenderDevice
{
public:
  EZ_ALWAYS_INLINE std::shared_ptr<Device> GetDevice() const;
  EZ_ALWAYS_INLINE std::shared_ptr<CommandQueue> GetCommandQueue() const;

public:
  // Init & shutdown functions

  virtual ezResult Init() override;
  virtual ezResult Shutdown() override;

  // Pipeline & Pass functions

  virtual void BeginPipeline(const char* szName) override;
  virtual void EndPipeline() override;

  virtual ezRHIPass* BeginPass(const char* szName) override;
  virtual void EndPass(ezRHIPass* pPass) override;

  // State creation functions

  virtual ezRHIBlendStateHandle CreateBlendState(const ezRHIBlendStateCreationDescription& Description) override;
  virtual void DestroyBlendState(ezRHIBlendStateHandle hBlendState) override;

  virtual ezRHIDepthStencilStateHandle CreateDepthStencilState(const ezRHIDepthStencilStateCreationDescription& Description) override;
  virtual void DestroyDepthStencilState(ezRHIDepthStencilStateHandle hDepthStencilState) override;

  virtual ezRHIRasterizerStateHandle CreateRasterizerState(const ezRHIRasterizerStateCreationDescription& Description) override;
  virtual void DestroyRasterizerState(ezRHIRasterizerStateHandle hRasterizerState) override;

  virtual ezRHISamplerStateHandle CreateSamplerState(const ezRHISamplerStateCreationDescription& Description) override;
  virtual void DestroySamplerState(ezRHISamplerStateHandle hSamplerState) override;

  // Resource creation functions

  virtual ezRHIShaderHandle CreateShader(const ezRHIShaderCreationDescription& Description) override;
  virtual void DestroyShader(ezRHIShaderHandle hShader) override;

  virtual ezRHIBufferHandle CreateBuffer(const ezRHIBufferCreationDescription& Description, ezArrayPtr<const ezUInt8> pInitialData = ezArrayPtr<const ezUInt8>()) override;
  virtual void DestroyBuffer(ezRHIBufferHandle hBuffer) override;

  // Helper functions for buffers (for common, simple use cases)

  virtual ezRHIBufferHandle CreateVertexBuffer(ezUInt32 uiVertexSize, ezUInt32 uiVertexCount, ezArrayPtr<const ezUInt8> pInitialData = ezArrayPtr<const ezUInt8>()) override;
  virtual ezRHIBufferHandle CreateIndexBuffer(ezRHIIndexType::Enum IndexType, ezUInt32 uiIndexCount, ezArrayPtr<const ezUInt8> pInitialData = ezArrayPtr<const ezUInt8>()) override;
  virtual ezRHIBufferHandle CreateConstantBuffer(ezUInt32 uiBufferSize);

  virtual ezRHITextureHandle CreateTexture(const ezRHITextureCreationDescription& Description, ezArrayPtr<ezRHISystemMemoryDescription> pInitialData = ezArrayPtr<ezRHISystemMemoryDescription>()) override;
  virtual ezResult ReplaceExisitingNativeObject(ezRHITextureHandle hTexture, void* pExisitingNativeObject) override;
  virtual void DestroyTexture(ezRHITextureHandle hTexture);

  virtual ezRHITextureHandle CreateProxyTexture(ezRHITextureHandle hParentTexture, ezUInt32 uiSlice) override;
  virtual void DestroyProxyTexture(ezRHITextureHandle hProxyTexture) override;

  // Resource views
  virtual ezRHIResourceViewHandle GetDefaultResourceView(ezRHITextureHandle hTexture) override;
  virtual ezRHIResourceViewHandle GetDefaultResourceView(ezRHIBufferHandle hBuffer) override;

  virtual ezRHIResourceViewHandle CreateResourceView(const ezRHIResourceViewCreationDescription& Description) override;
  virtual void DestroyResourceView(ezRHIResourceViewHandle hResourceView) override;

  // Render target views
  virtual ezRHIRenderTargetViewHandle GetDefaultRenderTargetView(ezRHITextureHandle hTexture) override;

  virtual ezRHIRenderTargetViewHandle CreateRenderTargetView(const ezRHIRenderTargetViewCreationDescription& Description) override;
  virtual void DestroyRenderTargetView(ezRHIRenderTargetViewHandle hRenderTargetView) override;

  // Unordered access views
  virtual ezRHIUnorderedAccessViewHandle CreateUnorderedAccessView(const ezRHIUnorderedAccessViewCreationDescription& Description) override;
  virtual void DestroyUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView) override;


  // Other rendering creation functions

  virtual ezRHISwapChainHandle CreateSwapChain(const ezRHISwapChainCreationDescription& Description) override;
  virtual void DestroySwapChain(ezRHISwapChainHandle hSwapChain) override;

  virtual ezRHIFenceHandle CreateFence() override;
  virtual void DestroyFence(ezRHIFenceHandle& hFence) override;

  virtual ezRHIQueryHandle CreateQuery(const ezRHIQueryCreationDescription& Description) override;
  virtual void DestroyQuery(ezRHIQueryHandle hQuery) override;

  virtual ezRHIVertexDeclarationHandle CreateVertexDeclaration(const ezRHIVertexDeclarationCreationDescription& Description) override;
  virtual void DestroyVertexDeclaration(ezRHIVertexDeclarationHandle hVertexDeclaration) override;

  // Timestamp functions

  virtual ezResult GetTimestampResult(ezRHITimestampHandle hTimestamp, ezTime& result) override;

  /// \todo Map functions to save on memcpys

  // Swap chain functions

  virtual void Present(ezRHISwapChainHandle hSwapChain, bool bVSync) override;
  virtual ezRHITextureHandle GetBackBufferTextureFromSwapChain(ezRHISwapChainHandle hSwapChain) override;


  // Misc functions

  virtual void BeginFrame() override;
  virtual void EndFrame() override;

  virtual void SetPrimarySwapChain(ezRHISwapChainHandle hSwapChain) override;
  virtual ezRHISwapChainHandle GetPrimarySwapChain() const override;

  virtual ezRHITimestampHandle GetTimestamp() override;

  virtual const ezRHIRenderDeviceCreationDescription* GetDescription() const override;


  virtual const ezRHISwapChain* GetSwapChain(ezRHISwapChainHandle hSwapChain) const override;
  virtual const ezRHIShader* GetShader(ezRHIShaderHandle hShader) const override;
  virtual const ezRHITexture* GetTexture(ezRHITextureHandle hTexture) const override;
  virtual const ezRHIBuffer* GetBuffer(ezRHIBufferHandle hBuffer) const override;
  virtual const ezRHIDepthStencilState* GetDepthStencilState(ezRHIDepthStencilStateHandle hDepthStencilState) const override;
  virtual const ezRHIBlendState* GetBlendState(ezRHIBlendStateHandle hBlendState) const override;
  virtual const ezRHIRasterizerState* GetRasterizerState(ezRHIRasterizerStateHandle hRasterizerState) const override;
  virtual const ezRHIVertexDeclaration* GetVertexDeclaration(ezRHIVertexDeclarationHandle hVertexDeclaration) const override;
  virtual const ezRHISamplerState* GetSamplerState(ezRHISamplerStateHandle hSamplerState) const override;
  virtual const ezRHIResourceView* GetResourceView(ezRHIResourceViewHandle hResourceView) const override;
  virtual const ezRHIRenderTargetView* GetRenderTargetView(ezRHIRenderTargetViewHandle hRenderTargetView) const override;
  virtual const ezRHIUnorderedAccessView* GetUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView) const override;
  virtual const ezRHIFence* GetFence(ezRHIFenceHandle hFence) const override;
  virtual const ezRHIQuery* GetQuery(ezRHIQueryHandle hQuery) const override;

  virtual const ezRHIRenderDeviceCapabilities& GetCapabilities() const override;

  virtual ezUInt64 GetMemoryConsumptionForTexture(const ezRHITextureCreationDescription& Description) const override;
  virtual ezUInt64 GetMemoryConsumptionForBuffer(const ezRHIBufferCreationDescription& Description) const override;

protected:
  friend ezInternal::NewInstance<ezRHIRenderDevice> CreateRenderDevice(ezAllocatorBase* pAllocator, const ezRHIRenderDeviceCreationDescription& desc);
  ezRHIRenderDeviceImpl(const ezRHIRenderDeviceCreationDescription& Description);

  virtual ~ezRHIRenderDeviceImpl();

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

  ezRHIRenderDeviceCreationDescription m_Description;

  ezRHISwapChainHandle m_hPrimarySwapChain;

  ezRHIRenderDeviceCapabilities m_Capabilities;

  // Deactivate Doxygen document generation for the following block. (API abstraction only)
  /// \cond

  // These functions need to be implemented by a render API abstraction
protected:
  friend class ezMemoryUtils;

  void FillCapabilities();
  void WaitForIdle();

  /// \endcond

private:
  bool m_bBeginFrameCalled = false;
  bool m_bBeginPipelineCalled = false;
  bool m_bBeginPassCalled = false;

private:
  bool m_bVsync = true;
  ezUInt32 m_uiFrameCount = 0;
  ezUInt32 m_uiFrameIndex = 0;
  ezUInt32 m_uiWidth = 0;
  ezUInt32 m_uiHeight = 0;

  std::shared_ptr<Instance> m_pInstance;
  std::shared_ptr<Adapter> m_pAdapter;
  std::shared_ptr<Device> m_pDevice;
  std::shared_ptr<CommandQueue> m_pCommandQueue;

  std::shared_ptr<Swapchain> m_pPrimarySwapChain;

  std::vector<std::shared_ptr<CommandList>> m_BarrierCommandLists;
  std::vector<ezUInt64> m_pFrameFenceValues;
  ezUInt64 m_uiFenceValue = 0;

  std::shared_ptr<Fence> m_pFence;
  ezRHIFenceHandle m_hFence;

  std::vector<std::shared_ptr<CommandList>> m_CommandListPool;
  ezDeque<std::pair<ezUInt64 /*fence_value*/, ezUInt64 /*offset*/>> m_FenceValueByCmd;
  //std::unique_ptr<ObjectCache> m_object_cache;
};

#include <RHI/Renderer/Device/Implementation/RenderDeviceImpl_inl.h>
