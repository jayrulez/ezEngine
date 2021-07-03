
/// \brief Used to guard ezRHIDevice functions from multi-threaded access and to verify that executing them on non-main-threads is allowed
#define EZ_GALDEVICE_LOCK_AND_CHECK()                                                                                                                \
  EZ_LOCK(m_Mutex);                                                                                                                                  \
  VerifyMultithreadedAccess()

EZ_ALWAYS_INLINE const ezRHIDeviceCreationDescription* ezRHIDevice::GetDescription() const
{
  return &m_Description;
}

EZ_ALWAYS_INLINE ezResult ezRHIDevice::GetTimestampResult(ezRHITimestampHandle hTimestamp, ezTime& result)
{
  return GetTimestampResultPlatform(hTimestamp, result);
}

EZ_ALWAYS_INLINE ezRHITimestampHandle ezRHIDevice::GetTimestamp()
{
  return GetTimestampPlatform();
}

EZ_ALWAYS_INLINE ezRHISwapChainHandle ezRHIDevice::GetPrimarySwapChain() const
{
  return m_hPrimarySwapChain;
}

template <typename IdTableType, typename ReturnType>
EZ_ALWAYS_INLINE ReturnType* ezRHIDevice::Get(typename IdTableType::TypeOfId hHandle, const IdTableType& IdTable) const
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ReturnType* pObject = nullptr;
  IdTable.TryGetValue(hHandle, pObject);
  return pObject;
}

inline const ezRHISwapChain* ezRHIDevice::GetSwapChain(ezRHISwapChainHandle hSwapChain) const
{
  return Get<SwapChainTable, ezRHISwapChain>(hSwapChain, m_SwapChains);
}

inline const ezRHIShader* ezRHIDevice::GetShader(ezRHIShaderHandle hShader) const
{
  return Get<ShaderTable, ezRHIShader>(hShader, m_Shaders);
}

inline const ezRHITexture* ezRHIDevice::GetTexture(ezRHITextureHandle hTexture) const
{
  return Get<TextureTable, ezRHITexture>(hTexture, m_Textures);
}

inline const ezRHIBuffer* ezRHIDevice::GetBuffer(ezRHIBufferHandle hBuffer) const
{
  return Get<BufferTable, ezRHIBuffer>(hBuffer, m_Buffers);
}

inline const ezRHIDepthStencilState* ezRHIDevice::GetDepthStencilState(ezRHIDepthStencilStateHandle hDepthStencilState) const
{
  return Get<DepthStencilStateTable, ezRHIDepthStencilState>(hDepthStencilState, m_DepthStencilStates);
}

inline const ezRHIBlendState* ezRHIDevice::GetBlendState(ezRHIBlendStateHandle hBlendState) const
{
  return Get<BlendStateTable, ezRHIBlendState>(hBlendState, m_BlendStates);
}

inline const ezRHIRasterizerState* ezRHIDevice::GetRasterizerState(ezRHIRasterizerStateHandle hRasterizerState) const
{
  return Get<RasterizerStateTable, ezRHIRasterizerState>(hRasterizerState, m_RasterizerStates);
}

inline const ezRHIVertexDeclaration* ezRHIDevice::GetVertexDeclaration(ezRHIVertexDeclarationHandle hVertexDeclaration) const
{
  return Get<VertexDeclarationTable, ezRHIVertexDeclaration>(hVertexDeclaration, m_VertexDeclarations);
}

inline const ezRHISamplerState* ezRHIDevice::GetSamplerState(ezRHISamplerStateHandle hSamplerState) const
{
  return Get<SamplerStateTable, ezRHISamplerState>(hSamplerState, m_SamplerStates);
}

inline const ezRHIResourceView* ezRHIDevice::GetResourceView(ezRHIResourceViewHandle hResourceView) const
{
  return Get<ResourceViewTable, ezRHIResourceView>(hResourceView, m_ResourceViews);
}

inline const ezRHIRenderTargetView* ezRHIDevice::GetRenderTargetView(ezRHIRenderTargetViewHandle hRenderTargetView) const
{
  return Get<RenderTargetViewTable, ezRHIRenderTargetView>(hRenderTargetView, m_RenderTargetViews);
}

inline const ezRHIUnorderedAccessView* ezRHIDevice::GetUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView) const
{
  return Get<UnorderedAccessViewTable, ezRHIUnorderedAccessView>(hUnorderedAccessView, m_UnorderedAccessViews);
}

inline const ezRHIFence* ezRHIDevice::GetFence(ezRHIFenceHandle hFence) const
{
  return Get<FenceTable, ezRHIFence>(hFence, m_Fences);
}

inline const ezRHIQuery* ezRHIDevice::GetQuery(ezRHIQueryHandle hQuery) const
{
  return Get<QueryTable, ezRHIQuery>(hQuery, m_Queries);
}

// static
EZ_ALWAYS_INLINE void ezRHIDevice::SetDefaultDevice(ezRHIDevice* pDefaultDevice)
{
  s_pDefaultDevice = pDefaultDevice;
}

// static
EZ_ALWAYS_INLINE ezRHIDevice* ezRHIDevice::GetDefaultDevice()
{
  EZ_ASSERT_DEBUG(s_pDefaultDevice != nullptr, "Default device not set.");
  return s_pDefaultDevice;
}

// static
EZ_ALWAYS_INLINE bool ezRHIDevice::HasDefaultDevice()
{
  return s_pDefaultDevice != nullptr;
}

template <typename HandleType>
EZ_FORCE_INLINE void ezRHIDevice::AddDeadObject(ezUInt32 uiType, HandleType handle)
{
  auto& deadObject = m_DeadObjects.ExpandAndGetRef();
  deadObject.m_uiType = uiType;
  deadObject.m_uiHandle = handle.GetInternalID().m_Data;
}

template <typename HandleType>
void ezRHIDevice::ReviveDeadObject(ezUInt32 uiType, HandleType handle)
{
  ezUInt32 uiHandle = handle.GetInternalID().m_Data;

  for (ezUInt32 i = 0; i < m_DeadObjects.GetCount(); ++i)
  {
    const auto& deadObject = m_DeadObjects[i];

    if (deadObject.m_uiType == uiType && deadObject.m_uiHandle == uiHandle)
    {
      m_DeadObjects.RemoveAtAndCopy(i);
      return;
    }
  }
}

EZ_ALWAYS_INLINE void ezRHIDevice::VerifyMultithreadedAccess() const
{
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  EZ_ASSERT_DEV(m_Capabilities.m_bMultithreadedResourceCreation || ezThreadUtils::IsMainThread(),
    "This device does not support multi-threaded resource creation, therefore this function can only be executed on the main thread.");
#endif
}
