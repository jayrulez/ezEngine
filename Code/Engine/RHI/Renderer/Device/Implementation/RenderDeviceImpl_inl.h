
/// \brief Used to guard ezRHIRenderDevice functions from multi-threaded access and to verify that executing them on non-main-threads is allowed
#define EZ_RHIDEVICE_LOCK_AND_CHECK()                                                                                                                \
  EZ_LOCK(m_Mutex);                                                                                                                                  \
  VerifyMultithreadedAccess()

EZ_ALWAYS_INLINE const ezRHIRenderDeviceCreationDescription* ezRHIRenderDeviceImpl::GetDescription() const
{
  return &m_Description;
}

EZ_ALWAYS_INLINE ezResult ezRHIRenderDeviceImpl::GetTimestampResult(ezRHITimestampHandle hTimestamp, ezTime& result)
{
  //return GetTimestampResultPlatform(hTimestamp, result);
  return EZ_SUCCESS;
}

EZ_ALWAYS_INLINE ezRHITimestampHandle ezRHIRenderDeviceImpl::GetTimestamp()
{
  //return GetTimestampPlatform();
  return ezRHITimestampHandle();
}

EZ_ALWAYS_INLINE ezRHISwapChainHandle ezRHIRenderDeviceImpl::GetPrimarySwapChain() const
{
  return m_hPrimarySwapChain;
}

template <typename IdTableType, typename ReturnType>
EZ_ALWAYS_INLINE ReturnType* ezRHIRenderDeviceImpl::Get(typename IdTableType::TypeOfId hHandle, const IdTableType& IdTable) const
{
  EZ_RHIDEVICE_LOCK_AND_CHECK();

  ReturnType* pObject = nullptr;
  IdTable.TryGetValue(hHandle, pObject);
  return pObject;
}

inline const ezRHISwapChain* ezRHIRenderDeviceImpl::GetSwapChain(ezRHISwapChainHandle hSwapChain) const
{
  return Get<SwapChainTable, ezRHISwapChain>(hSwapChain, m_SwapChains);
}

inline const ezRHIShader* ezRHIRenderDeviceImpl::GetShader(ezRHIShaderHandle hShader) const
{
  return Get<ShaderTable, ezRHIShader>(hShader, m_Shaders);
}

inline const ezRHITexture* ezRHIRenderDeviceImpl::GetTexture(ezRHITextureHandle hTexture) const
{
  return Get<TextureTable, ezRHITexture>(hTexture, m_Textures);
}

inline const ezRHIBuffer* ezRHIRenderDeviceImpl::GetBuffer(ezRHIBufferHandle hBuffer) const
{
  return Get<BufferTable, ezRHIBuffer>(hBuffer, m_Buffers);
}

inline const ezRHIDepthStencilState* ezRHIRenderDeviceImpl::GetDepthStencilState(ezRHIDepthStencilStateHandle hDepthStencilState) const
{
  return Get<DepthStencilStateTable, ezRHIDepthStencilState>(hDepthStencilState, m_DepthStencilStates);
}

inline const ezRHIBlendState* ezRHIRenderDeviceImpl::GetBlendState(ezRHIBlendStateHandle hBlendState) const
{
  return Get<BlendStateTable, ezRHIBlendState>(hBlendState, m_BlendStates);
}

inline const ezRHIRasterizerState* ezRHIRenderDeviceImpl::GetRasterizerState(ezRHIRasterizerStateHandle hRasterizerState) const
{
  return Get<RasterizerStateTable, ezRHIRasterizerState>(hRasterizerState, m_RasterizerStates);
}

inline const ezRHIVertexDeclaration* ezRHIRenderDeviceImpl::GetVertexDeclaration(ezRHIVertexDeclarationHandle hVertexDeclaration) const
{
  return Get<VertexDeclarationTable, ezRHIVertexDeclaration>(hVertexDeclaration, m_VertexDeclarations);
}

inline const ezRHISamplerState* ezRHIRenderDeviceImpl::GetSamplerState(ezRHISamplerStateHandle hSamplerState) const
{
  return Get<SamplerStateTable, ezRHISamplerState>(hSamplerState, m_SamplerStates);
}

inline const ezRHIResourceView* ezRHIRenderDeviceImpl::GetResourceView(ezRHIResourceViewHandle hResourceView) const
{
  return Get<ResourceViewTable, ezRHIResourceView>(hResourceView, m_ResourceViews);
}

inline const ezRHIRenderTargetView* ezRHIRenderDeviceImpl::GetRenderTargetView(ezRHIRenderTargetViewHandle hRenderTargetView) const
{
  return Get<RenderTargetViewTable, ezRHIRenderTargetView>(hRenderTargetView, m_RenderTargetViews);
}

inline const ezRHIUnorderedAccessView* ezRHIRenderDeviceImpl::GetUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessView) const
{
  return Get<UnorderedAccessViewTable, ezRHIUnorderedAccessView>(hUnorderedAccessView, m_UnorderedAccessViews);
}

inline const ezRHIFence* ezRHIRenderDeviceImpl::GetFence(ezRHIFenceHandle hFence) const
{
  return Get<FenceTable, ezRHIFence>(hFence, m_Fences);
}

inline const ezRHIQuery* ezRHIRenderDeviceImpl::GetQuery(ezRHIQueryHandle hQuery) const
{
  return Get<QueryTable, ezRHIQuery>(hQuery, m_Queries);
}

template <typename HandleType>
EZ_FORCE_INLINE void ezRHIRenderDeviceImpl::AddDeadObject(ezUInt32 uiType, HandleType handle)
{
  auto& deadObject = m_DeadObjects.ExpandAndGetRef();
  deadObject.m_uiType = uiType;
  deadObject.m_uiHandle = handle.GetInternalID().m_Data;
}

template <typename HandleType>
void ezRHIRenderDeviceImpl::ReviveDeadObject(ezUInt32 uiType, HandleType handle)
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

EZ_ALWAYS_INLINE void ezRHIRenderDeviceImpl::VerifyMultithreadedAccess() const
{
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  EZ_ASSERT_DEV(m_Capabilities.m_bMultithreadedResourceCreation || ezThreadUtils::IsMainThread(),
    "This device does not support multi-threaded resource creation, therefore this function can only be executed on the main thread.");
#endif
}
EZ_ALWAYS_INLINE std::shared_ptr<Device> ezRHIRenderDeviceImpl::GetDevice() const
{
  return m_pDevice;
}
EZ_ALWAYS_INLINE std::shared_ptr<CommandQueue> ezRHIRenderDeviceImpl::GetCommandQueue() const
{
  return m_pCommandQueue;
}
