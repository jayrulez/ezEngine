#include <RHIPCH.h>

#include <Foundation/Logging/Log.h>
#include <Foundation/Profiling/Profiling.h>
#include <RHI/Renderer/Device/Device.h>
#include <RHI/Renderer/Device/SwapChain.h>
#include <RHI/Renderer/Resources/Buffer.h>
#include <RHI/Renderer/Resources/ProxyTexture.h>
#include <RHI/Renderer/Resources/RenderTargetView.h>
#include <RHI/Renderer/Resources/ResourceView.h>
#include <RHI/Renderer/Resources/UnorderedAccesView.h>
#include <RHI/Renderer/Shader/VertexDeclaration.h>
#include <RHI/Renderer/State/State.h>

namespace
{
  struct GALObjectType
  {
    enum Enum
    {
      BlendState,
      DepthStencilState,
      RasterizerState,
      SamplerState,
      Shader,
      Buffer,
      Texture,
      ResourceView,
      RenderTargetView,
      UnorderedAccessView,
      SwapChain,
      Fence,
      Query,
      VertexDeclaration
    };
  };

  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIBlendStateHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIDepthStencilStateHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIRasterizerStateHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHISamplerStateHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIShaderHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIBufferHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHITextureHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIResourceViewHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIRenderTargetViewHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIUnorderedAccessViewHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHISwapChainHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIFenceHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIQueryHandle) == sizeof(ezUInt32));
  EZ_CHECK_AT_COMPILETIME(sizeof(ezRHIVertexDeclarationHandle) == sizeof(ezUInt32));
} // namespace

ezRHIDevice* ezRHIDevice::s_pDefaultDevice = nullptr;


ezRHIDevice::ezRHIDevice(const ezRHIDeviceCreationDescription& desc)
  : m_Allocator("GALDevice", ezFoundation::GetDefaultAllocator())
  , m_AllocatorWrapper(&m_Allocator)
  , m_Description(desc)
{
}

ezRHIDevice::~ezRHIDevice()
{
  // Check for object leaks
  {
    EZ_LOG_BLOCK("ezRHIDevice object leak report");

    if (!m_Shaders.IsEmpty())
      ezLog::Warning("{0} shaders have not been cleaned up", m_Shaders.GetCount());

    if (!m_BlendStates.IsEmpty())
      ezLog::Warning("{0} blend states have not been cleaned up", m_BlendStates.GetCount());

    if (!m_DepthStencilStates.IsEmpty())
      ezLog::Warning("{0} depth stencil states have not been cleaned up", m_DepthStencilStates.GetCount());

    if (!m_RasterizerStates.IsEmpty())
      ezLog::Warning("{0} rasterizer states have not been cleaned up", m_RasterizerStates.GetCount());

    if (!m_Buffers.IsEmpty())
      ezLog::Warning("{0} buffers have not been cleaned up", m_Buffers.GetCount());

    if (!m_Textures.IsEmpty())
      ezLog::Warning("{0} textures have not been cleaned up", m_Textures.GetCount());

    if (!m_ResourceViews.IsEmpty())
      ezLog::Warning("{0} resource views have not been cleaned up", m_ResourceViews.GetCount());

    if (!m_RenderTargetViews.IsEmpty())
      ezLog::Warning("{0} render target views have not been cleaned up", m_RenderTargetViews.GetCount());

    if (!m_UnorderedAccessViews.IsEmpty())
      ezLog::Warning("{0} unordered access views have not been cleaned up", m_UnorderedAccessViews.GetCount());

    if (!m_SwapChains.IsEmpty())
      ezLog::Warning("{0} swap chains have not been cleaned up", m_SwapChains.GetCount());

    if (!m_Fences.IsEmpty())
      ezLog::Warning("{0} fences have not been cleaned up", m_Fences.GetCount());

    if (!m_Queries.IsEmpty())
      ezLog::Warning("{0} queries have not been cleaned up", m_Queries.GetCount());

    if (!m_VertexDeclarations.IsEmpty())
      ezLog::Warning("{0} vertex declarations have not been cleaned up", m_VertexDeclarations.GetCount());
  }
}

ezResult ezRHIDevice::Init()
{
  EZ_LOG_BLOCK("ezRHIDevice::Init");

  ezResult PlatformInitResult = InitPlatform();

  if (PlatformInitResult == EZ_FAILURE)
  {
    return EZ_FAILURE;
  }

  // Fill the capabilities
  FillCapabilitiesPlatform();

  ezLog::Info("Adapter: '{}' - {} VRAM, {} Sys RAM, {} Shared RAM", m_Capabilities.m_sAdapterName, ezArgFileSize(m_Capabilities.m_uiDedicatedVRAM),
    ezArgFileSize(m_Capabilities.m_uiDedicatedSystemRAM), ezArgFileSize(m_Capabilities.m_uiSharedSystemRAM));

  if (!m_Capabilities.m_bHardwareAccelerated)
  {
    ezLog::Warning("Selected graphics adapter has no hardware acceleration.");
  }

  EZ_GALDEVICE_LOCK_AND_CHECK();

  // Create primary swapchain if requested
  if (m_Description.m_bCreatePrimarySwapChain)
  {
    ezRHISwapChainHandle hSwapChain = CreateSwapChain(m_Description.m_PrimarySwapChainDescription);

    if (hSwapChain.IsInvalidated())
    {
      ezLog::Error("Primary swap chain couldn't be created!");
      return EZ_FAILURE;
    }

    // And make it the primary swap chain
    SetPrimarySwapChain(hSwapChain);
  }

  ezProfilingSystem::InitializeGPUData();

  {
    ezRHIDeviceEvent e;
    e.m_pDevice = this;
    e.m_Type = ezRHIDeviceEvent::AfterInit;
    m_Events.Broadcast(e);
  }

  return EZ_SUCCESS;
}

ezResult ezRHIDevice::Shutdown()
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  EZ_LOG_BLOCK("ezRHIDevice::Shutdown");

  {
    ezRHIDeviceEvent e;
    e.m_pDevice = this;
    e.m_Type = ezRHIDeviceEvent::BeforeShutdown;
    m_Events.Broadcast(e);
  }

  // If we created a primary swap chain, release it
  if (!m_hPrimarySwapChain.IsInvalidated())
  {
    // DestroySwapChain usually warns for destroying the primary swap chain.
    auto handle = m_hPrimarySwapChain;
    m_hPrimarySwapChain.Invalidate();
    DestroySwapChain(handle);
  }

  DestroyDeadObjects();

  // make sure we are not listed as the default device anymore
  if (ezRHIDevice::HasDefaultDevice() && ezRHIDevice::GetDefaultDevice() == this)
  {
    ezRHIDevice::SetDefaultDevice(nullptr);
  }

  return ShutdownPlatform();
}

void ezRHIDevice::BeginPipeline(const char* szName)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  EZ_ASSERT_DEV(!m_bBeginPipelineCalled, "Nested Pipelines are not allowed: You must call ezRHIDevice::EndPipeline before you can call ezRHIDevice::BeginPipeline again");
  m_bBeginPipelineCalled = true;

  BeginPipelinePlatform(szName);
}

void ezRHIDevice::EndPipeline()
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  EZ_ASSERT_DEV(m_bBeginPipelineCalled, "You must have called ezRHIDevice::BeginPipeline before you can call ezRHIDevice::EndPipeline");
  m_bBeginPipelineCalled = false;

  EndPipelinePlatform();
}

ezRHIPass* ezRHIDevice::BeginPass(const char* szName)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  EZ_ASSERT_DEV(!m_bBeginPassCalled, "Nested Passes are not allowed: You must call ezRHIDevice::EndPass before you can call ezRHIDevice::BeginPass again");
  m_bBeginPassCalled = true;

  return BeginPassPlatform(szName);
}

void ezRHIDevice::EndPass(ezRHIPass* pPass)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  EZ_ASSERT_DEV(m_bBeginPassCalled, "You must have called ezRHIDevice::BeginPass before you can call ezRHIDevice::EndPass");
  m_bBeginPassCalled = false;

  EndPassPlatform(pPass);
}

ezRHIBlendStateHandle ezRHIDevice::CreateBlendState(const ezRHIBlendStateCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  // Hash desc and return potential existing one (including inc. refcount)
  ezUInt32 uiHash = desc.CalculateHash();

  {
    ezRHIBlendStateHandle hBlendState;
    if (m_BlendStateTable.TryGetValue(uiHash, hBlendState))
    {
      ezRHIBlendState* pBlendState = m_BlendStates[hBlendState];
      if (pBlendState->GetRefCount() == 0)
      {
        ReviveDeadObject(GALObjectType::BlendState, hBlendState);
      }

      pBlendState->AddRef();
      return hBlendState;
    }
  }

  ezRHIBlendState* pBlendState = CreateBlendStatePlatform(desc);

  if (pBlendState != nullptr)
  {
    EZ_ASSERT_DEBUG(pBlendState->GetDescription().CalculateHash() == uiHash, "BlendState hash doesn't match");

    pBlendState->AddRef();

    ezRHIBlendStateHandle hBlendState(m_BlendStates.Insert(pBlendState));
    m_BlendStateTable.Insert(uiHash, hBlendState);

    return hBlendState;
  }

  return ezRHIBlendStateHandle();
}

void ezRHIDevice::DestroyBlendState(ezRHIBlendStateHandle hBlendState)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIBlendState* pBlendState = nullptr;

  if (m_BlendStates.TryGetValue(hBlendState, pBlendState))
  {
    pBlendState->ReleaseRef();

    if (pBlendState->GetRefCount() == 0)
    {
      AddDeadObject(GALObjectType::BlendState, hBlendState);
    }
  }
  else
  {
    ezLog::Warning("DestroyBlendState called on invalid handle (double free?)");
  }
}

ezRHIDepthStencilStateHandle ezRHIDevice::CreateDepthStencilState(const ezRHIDepthStencilStateCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  // Hash desc and return potential existing one (including inc. refcount)
  ezUInt32 uiHash = desc.CalculateHash();

  {
    ezRHIDepthStencilStateHandle hDepthStencilState;
    if (m_DepthStencilStateTable.TryGetValue(uiHash, hDepthStencilState))
    {
      ezRHIDepthStencilState* pDepthStencilState = m_DepthStencilStates[hDepthStencilState];
      if (pDepthStencilState->GetRefCount() == 0)
      {
        ReviveDeadObject(GALObjectType::DepthStencilState, hDepthStencilState);
      }

      pDepthStencilState->AddRef();
      return hDepthStencilState;
    }
  }

  ezRHIDepthStencilState* pDepthStencilState = CreateDepthStencilStatePlatform(desc);

  if (pDepthStencilState != nullptr)
  {
    EZ_ASSERT_DEBUG(pDepthStencilState->GetDescription().CalculateHash() == uiHash, "DepthStencilState hash doesn't match");

    pDepthStencilState->AddRef();

    ezRHIDepthStencilStateHandle hDepthStencilState(m_DepthStencilStates.Insert(pDepthStencilState));
    m_DepthStencilStateTable.Insert(uiHash, hDepthStencilState);

    return hDepthStencilState;
  }

  return ezRHIDepthStencilStateHandle();
}

void ezRHIDevice::DestroyDepthStencilState(ezRHIDepthStencilStateHandle hDepthStencilState)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIDepthStencilState* pDepthStencilState = nullptr;

  if (m_DepthStencilStates.TryGetValue(hDepthStencilState, pDepthStencilState))
  {
    pDepthStencilState->ReleaseRef();

    if (pDepthStencilState->GetRefCount() == 0)
    {
      AddDeadObject(GALObjectType::DepthStencilState, hDepthStencilState);
    }
  }
  else
  {
    ezLog::Warning("DestroyDepthStencilState called on invalid handle (double free?)");
  }
}

ezRHIRasterizerStateHandle ezRHIDevice::CreateRasterizerState(const ezRHIRasterizerStateCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  // Hash desc and return potential existing one (including inc. refcount)
  ezUInt32 uiHash = desc.CalculateHash();

  {
    ezRHIRasterizerStateHandle hRasterizerState;
    if (m_RasterizerStateTable.TryGetValue(uiHash, hRasterizerState))
    {
      ezRHIRasterizerState* pRasterizerState = m_RasterizerStates[hRasterizerState];
      if (pRasterizerState->GetRefCount() == 0)
      {
        ReviveDeadObject(GALObjectType::RasterizerState, hRasterizerState);
      }

      pRasterizerState->AddRef();
      return hRasterizerState;
    }
  }

  ezRHIRasterizerState* pRasterizerState = CreateRasterizerStatePlatform(desc);

  if (pRasterizerState != nullptr)
  {
    EZ_ASSERT_DEBUG(pRasterizerState->GetDescription().CalculateHash() == uiHash, "RasterizerState hash doesn't match");

    pRasterizerState->AddRef();

    ezRHIRasterizerStateHandle hRasterizerState(m_RasterizerStates.Insert(pRasterizerState));
    m_RasterizerStateTable.Insert(uiHash, hRasterizerState);

    return hRasterizerState;
  }

  return ezRHIRasterizerStateHandle();
}

void ezRHIDevice::DestroyRasterizerState(ezRHIRasterizerStateHandle hRasterizerState)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIRasterizerState* pRasterizerState = nullptr;

  if (m_RasterizerStates.TryGetValue(hRasterizerState, pRasterizerState))
  {
    pRasterizerState->ReleaseRef();

    if (pRasterizerState->GetRefCount() == 0)
    {
      AddDeadObject(GALObjectType::RasterizerState, hRasterizerState);
    }
  }
  else
  {
    ezLog::Warning("DestroyRasterizerState called on invalid handle (double free?)");
  }
}

ezRHISamplerStateHandle ezRHIDevice::CreateSamplerState(const ezRHISamplerStateCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  /// \todo Platform independent validation

  // Hash desc and return potential existing one (including inc. refcount)
  ezUInt32 uiHash = desc.CalculateHash();

  {
    ezRHISamplerStateHandle hSamplerState;
    if (m_SamplerStateTable.TryGetValue(uiHash, hSamplerState))
    {
      ezRHISamplerState* pSamplerState = m_SamplerStates[hSamplerState];
      if (pSamplerState->GetRefCount() == 0)
      {
        ReviveDeadObject(GALObjectType::SamplerState, hSamplerState);
      }

      pSamplerState->AddRef();
      return hSamplerState;
    }
  }

  ezRHISamplerState* pSamplerState = CreateSamplerStatePlatform(desc);

  if (pSamplerState != nullptr)
  {
    EZ_ASSERT_DEBUG(pSamplerState->GetDescription().CalculateHash() == uiHash, "SamplerState hash doesn't match");

    pSamplerState->AddRef();

    ezRHISamplerStateHandle hSamplerState(m_SamplerStates.Insert(pSamplerState));
    m_SamplerStateTable.Insert(uiHash, hSamplerState);

    return hSamplerState;
  }

  return ezRHISamplerStateHandle();
}

void ezRHIDevice::DestroySamplerState(ezRHISamplerStateHandle hSamplerState)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHISamplerState* pSamplerState = nullptr;

  if (m_SamplerStates.TryGetValue(hSamplerState, pSamplerState))
  {
    pSamplerState->ReleaseRef();

    if (pSamplerState->GetRefCount() == 0)
    {
      AddDeadObject(GALObjectType::SamplerState, hSamplerState);
    }
  }
  else
  {
    ezLog::Warning("DestroySamplerState called on invalid handle (double free?)");
  }
}



ezRHIShaderHandle ezRHIDevice::CreateShader(const ezRHIShaderCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  bool bHasByteCodes = false;

  for (ezUInt32 uiStage = 0; uiStage < ezRHIShaderStage::ENUM_COUNT; uiStage++)
  {
    if (desc.HasByteCodeForStage((ezRHIShaderStage::Enum)uiStage))
    {
      bHasByteCodes = true;
      break;
    }
  }

  if (!bHasByteCodes)
  {
    ezLog::Error("Can't create a shader which supplies no bytecodes at all!");
    return ezRHIShaderHandle();
  }

  ezRHIShader* pShader = CreateShaderPlatform(desc);

  if (pShader == nullptr)
  {
    return ezRHIShaderHandle();
  }
  else
  {
    return ezRHIShaderHandle(m_Shaders.Insert(pShader));
  }
}

void ezRHIDevice::DestroyShader(ezRHIShaderHandle hShader)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIShader* pShader = nullptr;

  if (m_Shaders.TryGetValue(hShader, pShader))
  {
    AddDeadObject(GALObjectType::Shader, hShader);
  }
  else
  {
    ezLog::Warning("DestroyShader called on invalid handle (double free?)");
  }
}


ezRHIBufferHandle ezRHIDevice::CreateBuffer(const ezRHIBufferCreationDescription& desc, ezArrayPtr<const ezUInt8> pInitialData)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  if (desc.m_uiTotalSize == 0)
  {
    ezLog::Error("Trying to create a buffer with size of 0 is not possible!");
    return ezRHIBufferHandle();
  }

  if (desc.m_ResourceAccess.IsImmutable())
  {
    if (pInitialData.IsEmpty())
    {
      ezLog::Error("Trying to create an immutable buffer but not supplying initial data is not possible!");
      return ezRHIBufferHandle();
    }

    ezUInt32 uiBufferSize = desc.m_uiTotalSize;
    if (uiBufferSize != pInitialData.GetCount())
    {
      ezLog::Error("Trying to create a buffer with invalid initial data!");
      return ezRHIBufferHandle();
    }
  }

  /// \todo Platform independent validation (buffer type supported)

  ezRHIBuffer* pBuffer = CreateBufferPlatform(desc, pInitialData);

  if (pBuffer != nullptr)
  {
    ezRHIBufferHandle hBuffer(m_Buffers.Insert(pBuffer));

    // Create default resource view
    if (desc.m_bAllowShaderResourceView && desc.m_BufferType == ezRHIBufferType::Generic)
    {
      ezRHIResourceViewCreationDescription viewDesc;
      viewDesc.m_hBuffer = hBuffer;
      viewDesc.m_uiFirstElement = 0;
      viewDesc.m_uiNumElements = (desc.m_uiStructSize != 0) ? (desc.m_uiTotalSize / desc.m_uiStructSize) : desc.m_uiTotalSize;

      pBuffer->m_hDefaultResourceView = CreateResourceView(viewDesc);
    }

    return hBuffer;
  }

  return ezRHIBufferHandle();
}

void ezRHIDevice::DestroyBuffer(ezRHIBufferHandle hBuffer)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIBuffer* pBuffer = nullptr;

  if (m_Buffers.TryGetValue(hBuffer, pBuffer))
  {
    AddDeadObject(GALObjectType::Buffer, hBuffer);
  }
  else
  {
    ezLog::Warning("DestroyBuffer called on invalid handle (double free?)");
  }
}


// Helper functions for buffers (for common, simple use cases)
ezRHIBufferHandle ezRHIDevice::CreateVertexBuffer(ezUInt32 uiVertexSize, ezUInt32 uiVertexCount, ezArrayPtr<const ezUInt8> pInitialData)
{
  ezRHIBufferCreationDescription desc;
  desc.m_uiStructSize = uiVertexSize;
  desc.m_uiTotalSize = uiVertexSize * uiVertexCount;
  desc.m_BufferType = ezRHIBufferType::VertexBuffer;
  desc.m_ResourceAccess.m_bImmutable = !pInitialData.IsEmpty();

  return CreateBuffer(desc, pInitialData);
}

ezRHIBufferHandle ezRHIDevice::CreateIndexBuffer(ezRHIIndexType::Enum IndexType, ezUInt32 uiIndexCount, ezArrayPtr<const ezUInt8> pInitialData)
{
  ezRHIBufferCreationDescription desc;
  desc.m_uiStructSize = ezRHIIndexType::GetSize(IndexType);
  desc.m_uiTotalSize = desc.m_uiStructSize * uiIndexCount;
  desc.m_BufferType = ezRHIBufferType::IndexBuffer;
  desc.m_ResourceAccess.m_bImmutable = !pInitialData.IsEmpty();

  return CreateBuffer(desc, pInitialData);
}

ezRHIBufferHandle ezRHIDevice::CreateConstantBuffer(ezUInt32 uiBufferSize)
{
  ezRHIBufferCreationDescription desc;
  desc.m_uiStructSize = 0;
  desc.m_uiTotalSize = uiBufferSize;
  desc.m_BufferType = ezRHIBufferType::ConstantBuffer;
  desc.m_ResourceAccess.m_bImmutable = false;

  return CreateBuffer(desc);
}


ezRHITextureHandle ezRHIDevice::CreateTexture(const ezRHITextureCreationDescription& desc, ezArrayPtr<ezRHISystemMemoryDescription> pInitialData)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  /// \todo Platform independent validation (desc width & height < platform maximum, format, etc.)

  if (desc.m_ResourceAccess.IsImmutable() && (pInitialData.IsEmpty() || pInitialData.GetCount() < desc.m_uiMipLevelCount) &&
      !desc.m_bCreateRenderTarget)
  {
    ezLog::Error("Trying to create an immutable texture but not supplying initial data (or not enough data pointers) is not possible!");
    return ezRHITextureHandle();
  }

  if (desc.m_uiWidth == 0 || desc.m_uiHeight == 0)
  {
    ezLog::Error("Trying to create a texture with width or height == 0 is not possible!");
    return ezRHITextureHandle();
  }

  ezRHITexture* pTexture = CreateTexturePlatform(desc, pInitialData);

  if (pTexture != nullptr)
  {
    ezRHITextureHandle hTexture(m_Textures.Insert(pTexture));

    // Create default resource view
    if (desc.m_bAllowShaderResourceView)
    {
      ezRHIResourceViewCreationDescription viewDesc;
      viewDesc.m_hTexture = hTexture;
      viewDesc.m_uiArraySize = desc.m_uiArraySize;
      pTexture->m_hDefaultResourceView = CreateResourceView(viewDesc);
    }

    // Create default render target view
    if (desc.m_bCreateRenderTarget)
    {
      ezRHIRenderTargetViewCreationDescription rtDesc;
      rtDesc.m_hTexture = hTexture;
      rtDesc.m_uiFirstSlice = 0;
      rtDesc.m_uiSliceCount = desc.m_uiArraySize;

      pTexture->m_hDefaultRenderTargetView = CreateRenderTargetView(rtDesc);
    }

    return hTexture;
  }

  return ezRHITextureHandle();
}

ezResult ezRHIDevice::ReplaceExisitingNativeObject(ezRHITextureHandle hTexture, void* pExisitingNativeObject)
{
  ezRHITexture* pTexture = nullptr;
  if (m_Textures.TryGetValue(hTexture, pTexture))
  {
    for (auto it = pTexture->m_ResourceViews.GetIterator(); it.IsValid(); ++it)
    {
      ezRHIResourceView* pResourceView = nullptr;

      if (m_ResourceViews.TryGetValue(it.Value(), pResourceView))
      {
        EZ_VERIFY(pResourceView->DeInitPlatform(this).Succeeded(), "DeInitPlatform should never fail.");
      }
    }
    for (auto it = pTexture->m_RenderTargetViews.GetIterator(); it.IsValid(); ++it)
    {
      ezRHIRenderTargetView* pRenderTargetView = nullptr;

      if (m_RenderTargetViews.TryGetValue(it.Value(), pRenderTargetView))
      {
        EZ_VERIFY(pRenderTargetView->DeInitPlatform(this).Succeeded(), "DeInitPlatform should never fail.");
      }
    }
    for (auto it = pTexture->m_UnorderedAccessViews.GetIterator(); it.IsValid(); ++it)
    {
      ezRHIUnorderedAccessView* pUnorderedAccessView = nullptr;

      if (m_UnorderedAccessViews.TryGetValue(it.Value(), pUnorderedAccessView))
      {
        EZ_VERIFY(pUnorderedAccessView->DeInitPlatform(this).Succeeded(), "DeInitPlatform should never fail.");
      }
    }

    EZ_VERIFY(pTexture->DeInitPlatform(this).Succeeded(), "DeInitPlatform should never fail.");
    EZ_VERIFY(
      pTexture->ReplaceExisitingNativeObject(pExisitingNativeObject).Succeeded(), "Failed to replace native texture, make sure the input is valid.");
    EZ_VERIFY(pTexture->InitPlatform(this, {}).Succeeded(),
      "InitPlatform failed on a texture the previously succeded in the same call, is the new native object valid?");

    for (auto it = pTexture->m_ResourceViews.GetIterator(); it.IsValid(); ++it)
    {
      ezRHIResourceView* pResourceView = nullptr;

      if (m_ResourceViews.TryGetValue(it.Value(), pResourceView))
      {
        EZ_VERIFY(pResourceView->InitPlatform(this).Succeeded(),
          "InitPlatform failed on a resource view that previously succeded in the same call, is the new native object valid?");
      }
    }
    for (auto it = pTexture->m_RenderTargetViews.GetIterator(); it.IsValid(); ++it)
    {
      ezRHIRenderTargetView* pRenderTargetView = nullptr;

      if (m_RenderTargetViews.TryGetValue(it.Value(), pRenderTargetView))
      {
        EZ_VERIFY(pRenderTargetView->InitPlatform(this).Succeeded(),
          "InitPlatform failed on a render target view that previously succeded in the same call, is the new native object valid?");
      }
    }
    for (auto it = pTexture->m_UnorderedAccessViews.GetIterator(); it.IsValid(); ++it)
    {
      ezRHIUnorderedAccessView* pUnorderedAccessView = nullptr;

      if (m_UnorderedAccessViews.TryGetValue(it.Value(), pUnorderedAccessView))
      {
        EZ_VERIFY(pUnorderedAccessView->InitPlatform(this).Succeeded(),
          "InitPlatform failed on a unordered access view that previously succeded in the same call, is the new native object valid?");
      }
    }
    return EZ_SUCCESS;
  }
  else
  {
    ezLog::Warning("ReplaceExisitingNativeObject called on invalid handle");
    return EZ_FAILURE;
  }
}

void ezRHIDevice::DestroyTexture(ezRHITextureHandle hTexture)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHITexture* pTexture = nullptr;
  if (m_Textures.TryGetValue(hTexture, pTexture))
  {
    AddDeadObject(GALObjectType::Texture, hTexture);
  }
  else
  {
    ezLog::Warning("DestroyTexture called on invalid handle (double free?)");
  }
}

ezRHITextureHandle ezRHIDevice::CreateProxyTexture(ezRHITextureHandle hParentTexture, ezUInt32 uiSlice)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHITexture* pParentTexture = nullptr;

  if (!hParentTexture.IsInvalidated())
  {
    pParentTexture = Get<TextureTable, ezRHITexture>(hParentTexture, m_Textures);
  }

  if (pParentTexture == nullptr)
  {
    ezLog::Error("No valid texture handle given for proxy texture creation!");
    return ezRHITextureHandle();
  }

  const auto& parentDesc = pParentTexture->GetDescription();
  EZ_ASSERT_DEV(parentDesc.m_Type != ezRHITextureType::Texture2DProxy, "Can't create a proxy texture of a proxy texture.");
  EZ_ASSERT_DEV(parentDesc.m_Type == ezRHITextureType::TextureCube || parentDesc.m_uiArraySize > 1,
    "Proxy textures can only be created for cubemaps or array textures.");

  ezRHIProxyTexture* pProxyTexture = EZ_NEW(&m_Allocator, ezRHIProxyTexture, *pParentTexture);
  ezRHITextureHandle hProxyTexture(m_Textures.Insert(pProxyTexture));

  const auto& desc = pProxyTexture->GetDescription();

  // Create default resource view
  if (desc.m_bAllowShaderResourceView)
  {
    ezRHIResourceViewCreationDescription viewDesc;
    viewDesc.m_hTexture = hProxyTexture;
    viewDesc.m_uiFirstArraySlice = uiSlice;
    viewDesc.m_uiArraySize = 1;

    pProxyTexture->m_hDefaultResourceView = CreateResourceView(viewDesc);
  }

  // Create default render target view
  if (desc.m_bCreateRenderTarget)
  {
    ezRHIRenderTargetViewCreationDescription rtDesc;
    rtDesc.m_hTexture = hProxyTexture;
    rtDesc.m_uiFirstSlice = uiSlice;
    rtDesc.m_uiSliceCount = 1;

    pProxyTexture->m_hDefaultRenderTargetView = CreateRenderTargetView(rtDesc);
  }

  return hProxyTexture;
}

void ezRHIDevice::DestroyProxyTexture(ezRHITextureHandle hProxyTexture)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHITexture* pTexture = nullptr;
  if (m_Textures.TryGetValue(hProxyTexture, pTexture))
  {
    EZ_ASSERT_DEV(pTexture->GetDescription().m_Type == ezRHITextureType::Texture2DProxy, "Given texture is not a proxy texture");

    AddDeadObject(GALObjectType::Texture, hProxyTexture);
  }
  else
  {
    ezLog::Warning("DestroyProxyTexture called on invalid handle (double free?)");
  }
}

ezRHIResourceViewHandle ezRHIDevice::GetDefaultResourceView(ezRHITextureHandle hTexture)
{
  if (const ezRHITexture* pTexture = GetTexture(hTexture))
  {
    return pTexture->m_hDefaultResourceView;
  }

  return ezRHIResourceViewHandle();
}

ezRHIResourceViewHandle ezRHIDevice::GetDefaultResourceView(ezRHIBufferHandle hBuffer)
{
  if (const ezRHIBuffer* pBuffer = GetBuffer(hBuffer))
  {
    return pBuffer->m_hDefaultResourceView;
  }

  return ezRHIResourceViewHandle();
}

ezRHIResourceViewHandle ezRHIDevice::CreateResourceView(const ezRHIResourceViewCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIResourceBase* pResource = nullptr;

  if (!desc.m_hTexture.IsInvalidated())
    pResource = Get<TextureTable, ezRHITexture>(desc.m_hTexture, m_Textures);

  if (!desc.m_hBuffer.IsInvalidated())
    pResource = Get<BufferTable, ezRHIBuffer>(desc.m_hBuffer, m_Buffers);

  if (pResource == nullptr)
  {
    ezLog::Error("No valid texture handle or buffer handle given for resource view creation!");
    return ezRHIResourceViewHandle();
  }

  // Hash desc and return potential existing one
  ezUInt32 uiHash = desc.CalculateHash();

  {
    ezRHIResourceViewHandle hResourceView;
    if (pResource->m_ResourceViews.TryGetValue(uiHash, hResourceView))
    {
      return hResourceView;
    }
  }

  ezRHIResourceView* pResourceView = CreateResourceViewPlatform(pResource, desc);

  if (pResourceView != nullptr)
  {
    ezRHIResourceViewHandle hResourceView(m_ResourceViews.Insert(pResourceView));
    pResource->m_ResourceViews.Insert(uiHash, hResourceView);

    return hResourceView;
  }

  return ezRHIResourceViewHandle();
}

void ezRHIDevice::DestroyResourceView(ezRHIResourceViewHandle hResourceView)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIResourceView* pResourceView = nullptr;

  if (m_ResourceViews.TryGetValue(hResourceView, pResourceView))
  {
    AddDeadObject(GALObjectType::ResourceView, hResourceView);
  }
  else
  {
    ezLog::Warning("DestroyResourceView called on invalid handle (double free?)");
  }
}

ezRHIRenderTargetViewHandle ezRHIDevice::GetDefaultRenderTargetView(ezRHITextureHandle hTexture)
{
  if (const ezRHITexture* pTexture = GetTexture(hTexture))
  {
    return pTexture->m_hDefaultRenderTargetView;
  }

  return ezRHIRenderTargetViewHandle();
}

ezRHIRenderTargetViewHandle ezRHIDevice::CreateRenderTargetView(const ezRHIRenderTargetViewCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHITexture* pTexture = nullptr;

  if (!desc.m_hTexture.IsInvalidated())
    pTexture = Get<TextureTable, ezRHITexture>(desc.m_hTexture, m_Textures);

  if (pTexture == nullptr)
  {
    ezLog::Error("No valid texture handle given for render target view creation!");
    return ezRHIRenderTargetViewHandle();
  }

  /// \todo Platform independent validation

  // Hash desc and return potential existing one
  ezUInt32 uiHash = desc.CalculateHash();

  {
    ezRHIRenderTargetViewHandle hRenderTargetView;
    if (pTexture->m_RenderTargetViews.TryGetValue(uiHash, hRenderTargetView))
    {
      return hRenderTargetView;
    }
  }

  ezRHIRenderTargetView* pRenderTargetView = CreateRenderTargetViewPlatform(pTexture, desc);

  if (pRenderTargetView != nullptr)
  {
    ezRHIRenderTargetViewHandle hRenderTargetView(m_RenderTargetViews.Insert(pRenderTargetView));
    pTexture->m_RenderTargetViews.Insert(uiHash, hRenderTargetView);

    return hRenderTargetView;
  }

  return ezRHIRenderTargetViewHandle();
}

void ezRHIDevice::DestroyRenderTargetView(ezRHIRenderTargetViewHandle hRenderTargetView)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIRenderTargetView* pRenderTargetView = nullptr;

  if (m_RenderTargetViews.TryGetValue(hRenderTargetView, pRenderTargetView))
  {
    AddDeadObject(GALObjectType::RenderTargetView, hRenderTargetView);
  }
  else
  {
    ezLog::Warning("DestroyRenderTargetView called on invalid handle (double free?)");
  }
}

ezRHIUnorderedAccessViewHandle ezRHIDevice::CreateUnorderedAccessView(const ezRHIUnorderedAccessViewCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  if (!desc.m_hTexture.IsInvalidated() && !desc.m_hBuffer.IsInvalidated())
  {
    ezLog::Error("Can't pass both a texture and buffer to a ezRHIUnorderedAccessViewCreationDescription.");
    return ezRHIUnorderedAccessViewHandle();
  }

  ezRHIResourceBase* pResource = nullptr;
  ezRHITexture* pTexture = nullptr;
  ezRHIBuffer* pBuffer = nullptr;

  if (!desc.m_hTexture.IsInvalidated())
  {
    pResource = pTexture = Get<TextureTable, ezRHITexture>(desc.m_hTexture, m_Textures);
  }
  else if (!desc.m_hBuffer.IsInvalidated())
  {
    pResource = pBuffer = Get<BufferTable, ezRHIBuffer>(desc.m_hBuffer, m_Buffers);
  }

  if (pResource == nullptr)
  {
    ezLog::Error("No valid texture handle or buffer handle given for unordered access view creation!");
    return ezRHIUnorderedAccessViewHandle();
  }

  // Some platform independent validation.
  {
    if (pTexture)
    {
      // Is this really platform independent?
      if (pTexture->GetDescription().m_Type == ezRHITextureType::TextureCube)
      {
        ezLog::Error("Can't create unordered access view from cube textures.");
        return ezRHIUnorderedAccessViewHandle();
      }

      // Is this really platform independent?
      if (pTexture->GetDescription().m_SampleCount != ezRHIMSAASampleCount::None)
      {
        ezLog::Error("Can't create unordered access view on textures with multisampling.");
        return ezRHIUnorderedAccessViewHandle();
      }
    }
    else
    {
      if (desc.m_OverrideViewFormat == ezRHIResourceFormat::Invalid)
      {
        ezLog::Error("Invalid resource format is not allowed for buffer unordered access views!");
        return ezRHIUnorderedAccessViewHandle();
      }

      if (!pBuffer->GetDescription().m_bAllowRawViews && desc.m_bRawView)
      {
        ezLog::Error("Trying to create a raw view for a buffer with no raw view flag is invalid!");
        return ezRHIUnorderedAccessViewHandle();
      }
    }
  }

  // Hash desc and return potential existing one
  ezUInt32 uiHash = desc.CalculateHash();

  {
    ezRHIUnorderedAccessViewHandle hUnorderedAccessView;
    if (pResource->m_UnorderedAccessViews.TryGetValue(uiHash, hUnorderedAccessView))
    {
      return hUnorderedAccessView;
    }
  }

  ezRHIUnorderedAccessView* pUnorderedAccessViewView = CreateUnorderedAccessViewPlatform(pResource, desc);

  if (pUnorderedAccessViewView != nullptr)
  {
    ezRHIUnorderedAccessViewHandle hUnorderedAccessView(m_UnorderedAccessViews.Insert(pUnorderedAccessViewView));
    pResource->m_UnorderedAccessViews.Insert(uiHash, hUnorderedAccessView);

    return hUnorderedAccessView;
  }

  return ezRHIUnorderedAccessViewHandle();
}

void ezRHIDevice::DestroyUnorderedAccessView(ezRHIUnorderedAccessViewHandle hUnorderedAccessViewHandle)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIUnorderedAccessView* pUnorderedAccesssView = nullptr;

  if (m_UnorderedAccessViews.TryGetValue(hUnorderedAccessViewHandle, pUnorderedAccesssView))
  {
    AddDeadObject(GALObjectType::UnorderedAccessView, hUnorderedAccessViewHandle);
  }
  else
  {
    ezLog::Warning("DestroyUnorderedAccessView called on invalid handle (double free?)");
  }
}

ezRHISwapChainHandle ezRHIDevice::CreateSwapChain(const ezRHISwapChainCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  /// \todo Platform independent validation
  if (desc.m_pWindow == nullptr)
  {
    ezLog::Error("The desc for the swap chain creation contained an invalid (nullptr) window handle!");
    return ezRHISwapChainHandle();
  }


  ezRHISwapChain* pSwapChain = CreateSwapChainPlatform(desc);

  if (pSwapChain == nullptr)
  {
    return ezRHISwapChainHandle();
  }
  else
  {
    return ezRHISwapChainHandle(m_SwapChains.Insert(pSwapChain));
  }
}

void ezRHIDevice::DestroySwapChain(ezRHISwapChainHandle hSwapChain)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHISwapChain* pSwapChain = nullptr;

  if (hSwapChain == m_hPrimarySwapChain)
  {
    ezLog::Warning("DestroySwapChain called on primary swap chain!");
    m_hPrimarySwapChain.Invalidate();
  }

  if (m_SwapChains.TryGetValue(hSwapChain, pSwapChain))
  {
    AddDeadObject(GALObjectType::SwapChain, hSwapChain);
  }
  else
  {
    ezLog::Warning("DestroySwapChain called on invalid handle (double free?)");
  }
}

ezRHIFenceHandle ezRHIDevice::CreateFence()
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIFence* pFence = CreateFencePlatform();

  if (pFence == nullptr)
  {
    return ezRHIFenceHandle();
  }
  else
  {
    return ezRHIFenceHandle(m_Fences.Insert(pFence));
  }
}

void ezRHIDevice::DestroyFence(ezRHIFenceHandle& hFence)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIFence* pFence = nullptr;

  if (m_Fences.TryGetValue(hFence, pFence))
  {
    AddDeadObject(GALObjectType::Fence, hFence);
  }
  else
  {
    ezLog::Warning("DestroyFence called on invalid handle (double free?)");
  }
}

ezRHIQueryHandle ezRHIDevice::CreateQuery(const ezRHIQueryCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIQuery* pQuery = CreateQueryPlatform(desc);

  if (pQuery == nullptr)
  {
    return ezRHIQueryHandle();
  }
  else
  {
    return ezRHIQueryHandle(m_Queries.Insert(pQuery));
  }
}

void ezRHIDevice::DestroyQuery(ezRHIQueryHandle hQuery)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIQuery* pQuery = nullptr;

  if (m_Queries.TryGetValue(hQuery, pQuery))
  {
    AddDeadObject(GALObjectType::Query, hQuery);
  }
  else
  {
    ezLog::Warning("DestroyQuery called on invalid handle (double free?)");
  }
}

ezRHIVertexDeclarationHandle ezRHIDevice::CreateVertexDeclaration(const ezRHIVertexDeclarationCreationDescription& desc)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  /// \todo Platform independent validation

  // Hash desc and return potential existing one (including inc. refcount)
  ezUInt32 uiHash = desc.CalculateHash();

  {
    ezRHIVertexDeclarationHandle hVertexDeclaration;
    if (m_VertexDeclarationTable.TryGetValue(uiHash, hVertexDeclaration))
    {
      ezRHIVertexDeclaration* pVertexDeclaration = m_VertexDeclarations[hVertexDeclaration];
      if (pVertexDeclaration->GetRefCount() == 0)
      {
        ReviveDeadObject(GALObjectType::VertexDeclaration, hVertexDeclaration);
      }

      pVertexDeclaration->AddRef();
      return hVertexDeclaration;
    }
  }

  ezRHIVertexDeclaration* pVertexDeclaration = CreateVertexDeclarationPlatform(desc);

  if (pVertexDeclaration != nullptr)
  {
    pVertexDeclaration->AddRef();

    ezRHIVertexDeclarationHandle hVertexDeclaration(m_VertexDeclarations.Insert(pVertexDeclaration));
    m_VertexDeclarationTable.Insert(uiHash, hVertexDeclaration);

    return hVertexDeclaration;
  }

  return ezRHIVertexDeclarationHandle();
}

void ezRHIDevice::DestroyVertexDeclaration(ezRHIVertexDeclarationHandle hVertexDeclaration)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHIVertexDeclaration* pVertexDeclaration = nullptr;

  if (m_VertexDeclarations.TryGetValue(hVertexDeclaration, pVertexDeclaration))
  {
    pVertexDeclaration->ReleaseRef();

    if (pVertexDeclaration->GetRefCount() == 0)
    {
      AddDeadObject(GALObjectType::VertexDeclaration, hVertexDeclaration);
    }
  }
  else
  {
    ezLog::Warning("DestroyVertexDeclaration called on invalid handle (double free?)");
  }
}

// Swap chain functions

void ezRHIDevice::Present(ezRHISwapChainHandle hSwapChain, bool bVSync)
{
  EZ_ASSERT_DEV(m_bBeginFrameCalled, "You must have called ezRHIDevice::Begin before you can call this function");

  ezRHISwapChain* pSwapChain = nullptr;

  if (m_SwapChains.TryGetValue(hSwapChain, pSwapChain))
  {
    PresentPlatform(pSwapChain, bVSync);
  }
  else
  {
    EZ_REPORT_FAILURE("Swap chain handle invalid");
  }
}

ezRHITextureHandle ezRHIDevice::GetBackBufferTextureFromSwapChain(ezRHISwapChainHandle hSwapChain)
{
  ezRHISwapChain* pSwapChain = nullptr;

  if (m_SwapChains.TryGetValue(hSwapChain, pSwapChain))
  {
    return pSwapChain->GetBackBufferTexture();
  }
  else
  {
    EZ_REPORT_FAILURE("Swap chain handle invalid");
    return ezRHITextureHandle();
  }
}



// Misc functions

void ezRHIDevice::BeginFrame()
{
  {
    ezRHIDeviceEvent e;
    e.m_pDevice = this;
    e.m_Type = ezRHIDeviceEvent::BeforeBeginFrame;
    m_Events.Broadcast(e);
  }

  {
    EZ_GALDEVICE_LOCK_AND_CHECK();
    EZ_ASSERT_DEV(!m_bBeginFrameCalled, "You must call ezRHIDevice::EndFrame before you can call ezRHIDevice::BeginFrame again");
    m_bBeginFrameCalled = true;

    BeginFramePlatform();
  }

  // TODO: move to beginrendering/compute calls
  //m_pPrimaryContext->ClearStatisticsCounters();

  {
    ezRHIDeviceEvent e;
    e.m_pDevice = this;
    e.m_Type = ezRHIDeviceEvent::AfterBeginFrame;
    m_Events.Broadcast(e);
  }
}

void ezRHIDevice::EndFrame()
{
  {
    ezRHIDeviceEvent e;
    e.m_pDevice = this;
    e.m_Type = ezRHIDeviceEvent::BeforeEndFrame;
    m_Events.Broadcast(e);
  }

  {
    EZ_GALDEVICE_LOCK_AND_CHECK();
    EZ_ASSERT_DEV(m_bBeginFrameCalled, "You must have called ezRHIDevice::Begin before you can call ezRHIDevice::EndFrame");

    DestroyDeadObjects();

    EndFramePlatform();

    m_bBeginFrameCalled = false;
  }

  {
    ezRHIDeviceEvent e;
    e.m_pDevice = this;
    e.m_Type = ezRHIDeviceEvent::AfterEndFrame;
    m_Events.Broadcast(e);
  }
}

void ezRHIDevice::SetPrimarySwapChain(ezRHISwapChainHandle hSwapChain)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  ezRHISwapChain* pSwapChain = nullptr;

  if (m_SwapChains.TryGetValue(hSwapChain, pSwapChain))
  {
    SetPrimarySwapChainPlatform(pSwapChain); // Needs a return value?
    m_hPrimarySwapChain = hSwapChain;
  }
  else
  {
    ezLog::Error("Invalid swap chain handle given to SetPrimarySwapChain!");
  }
}

const ezRHIDeviceCapabilities& ezRHIDevice::GetCapabilities() const
{
  return m_Capabilities;
}

ezUInt64 ezRHIDevice::GetMemoryConsumptionForTexture(const ezRHITextureCreationDescription& desc) const
{
  // This generic implementation is only an approximation, but it can be overridden by specific devices
  // to give an accurate memory consumption figure.
  ezUInt64 uiMemory = ezUInt64(desc.m_uiWidth) * ezUInt64(desc.m_uiHeight) * ezUInt64(desc.m_uiDepth);
  uiMemory *= desc.m_uiArraySize;
  uiMemory *= ezRHIResourceFormat::GetBitsPerElement(desc.m_Format);
  uiMemory /= 8; // Bits per pixel
  uiMemory *= desc.m_SampleCount;

  // Also account for mip maps
  if (desc.m_uiMipLevelCount > 1)
  {
    uiMemory += static_cast<ezUInt64>((1.0 / 3.0) * uiMemory);
  }

  return uiMemory;
}


ezUInt64 ezRHIDevice::GetMemoryConsumptionForBuffer(const ezRHIBufferCreationDescription& desc) const
{
  return desc.m_uiTotalSize;
}


void ezRHIDevice::DestroyViews(ezRHIResourceBase* pResource)
{
  EZ_GALDEVICE_LOCK_AND_CHECK();

  for (auto it = pResource->m_ResourceViews.GetIterator(); it.IsValid(); ++it)
  {
    ezRHIResourceViewHandle hResourceView = it.Value();
    ezRHIResourceView* pResourceView = m_ResourceViews[hResourceView];

    m_ResourceViews.Remove(hResourceView);

    DestroyResourceViewPlatform(pResourceView);
  }
  pResource->m_ResourceViews.Clear();
  pResource->m_hDefaultResourceView.Invalidate();

  for (auto it = pResource->m_RenderTargetViews.GetIterator(); it.IsValid(); ++it)
  {
    ezRHIRenderTargetViewHandle hRenderTargetView = it.Value();
    ezRHIRenderTargetView* pRenderTargetView = m_RenderTargetViews[hRenderTargetView];

    m_RenderTargetViews.Remove(hRenderTargetView);

    DestroyRenderTargetViewPlatform(pRenderTargetView);
  }
  pResource->m_RenderTargetViews.Clear();
  pResource->m_hDefaultRenderTargetView.Invalidate();

  for (auto it = pResource->m_UnorderedAccessViews.GetIterator(); it.IsValid(); ++it)
  {
    ezRHIUnorderedAccessViewHandle hUnorderedAccessView = it.Value();
    ezRHIUnorderedAccessView* pUnorderedAccessView = m_UnorderedAccessViews[hUnorderedAccessView];

    m_UnorderedAccessViews.Remove(hUnorderedAccessView);

    DestroyUnorderedAccessViewPlatform(pUnorderedAccessView);
  }
  pResource->m_UnorderedAccessViews.Clear();
}

void ezRHIDevice::DestroyDeadObjects()
{
  // Can't use range based for here since new objects might be added during iteration
  for (ezUInt32 i = 0; i < m_DeadObjects.GetCount(); ++i)
  {
    const auto& deadObject = m_DeadObjects[i];

    switch (deadObject.m_uiType)
    {
      case GALObjectType::BlendState:
      {
        ezRHIBlendStateHandle hBlendState(ezRHI::ez16_16Id(deadObject.m_uiHandle));
        ezRHIBlendState* pBlendState = nullptr;

        EZ_VERIFY(m_BlendStates.Remove(hBlendState, &pBlendState), "BlendState not found in idTable");
        EZ_VERIFY(m_BlendStateTable.Remove(pBlendState->GetDescription().CalculateHash()), "BlendState not found in de-duplication table");

        DestroyBlendStatePlatform(pBlendState);

        break;
      }
      case GALObjectType::DepthStencilState:
      {
        ezRHIDepthStencilStateHandle hDepthStencilState(ezRHI::ez16_16Id(deadObject.m_uiHandle));
        ezRHIDepthStencilState* pDepthStencilState = nullptr;

        EZ_VERIFY(m_DepthStencilStates.Remove(hDepthStencilState, &pDepthStencilState), "DepthStencilState not found in idTable");
        EZ_VERIFY(m_DepthStencilStateTable.Remove(pDepthStencilState->GetDescription().CalculateHash()),
          "DepthStencilState not found in de-duplication table");

        DestroyDepthStencilStatePlatform(pDepthStencilState);

        break;
      }
      case GALObjectType::RasterizerState:
      {
        ezRHIRasterizerStateHandle hRasterizerState(ezRHI::ez16_16Id(deadObject.m_uiHandle));
        ezRHIRasterizerState* pRasterizerState = nullptr;

        EZ_VERIFY(m_RasterizerStates.Remove(hRasterizerState, &pRasterizerState), "RasterizerState not found in idTable");
        EZ_VERIFY(
          m_RasterizerStateTable.Remove(pRasterizerState->GetDescription().CalculateHash()), "RasterizerState not found in de-duplication table");

        DestroyRasterizerStatePlatform(pRasterizerState);

        break;
      }
      case GALObjectType::SamplerState:
      {
        ezRHISamplerStateHandle hSamplerState(ezRHI::ez16_16Id(deadObject.m_uiHandle));
        ezRHISamplerState* pSamplerState = nullptr;

        EZ_VERIFY(m_SamplerStates.Remove(hSamplerState, &pSamplerState), "SamplerState not found in idTable");
        EZ_VERIFY(m_SamplerStateTable.Remove(pSamplerState->GetDescription().CalculateHash()), "SamplerState not found in de-duplication table");

        DestroySamplerStatePlatform(pSamplerState);

        break;
      }
      case GALObjectType::Shader:
      {
        ezRHIShaderHandle hShader(ezRHI::ez18_14Id(deadObject.m_uiHandle));
        ezRHIShader* pShader = nullptr;

        m_Shaders.Remove(hShader, &pShader);

        DestroyShaderPlatform(pShader);

        break;
      }
      case GALObjectType::Buffer:
      {
        ezRHIBufferHandle hBuffer(ezRHI::ez18_14Id(deadObject.m_uiHandle));
        ezRHIBuffer* pBuffer = nullptr;

        m_Buffers.Remove(hBuffer, &pBuffer);

        DestroyViews(pBuffer);
        DestroyBufferPlatform(pBuffer);

        break;
      }
      case GALObjectType::Texture:
      {
        ezRHITextureHandle hTexture(ezRHI::ez18_14Id(deadObject.m_uiHandle));
        ezRHITexture* pTexture = nullptr;

        m_Textures.Remove(hTexture, &pTexture);

        DestroyViews(pTexture);
        DestroyTexturePlatform(pTexture);

        break;
      }
      case GALObjectType::ResourceView:
      {
        ezRHIResourceViewHandle hResourceView(ezRHI::ez18_14Id(deadObject.m_uiHandle));
        ezRHIResourceView* pResourceView = nullptr;

        m_ResourceViews.Remove(hResourceView, &pResourceView);

        ezRHIResourceBase* pResource = pResourceView->m_pResource;
        EZ_ASSERT_DEBUG(pResource != nullptr, "");

        EZ_VERIFY(pResource->m_ResourceViews.Remove(pResourceView->GetDescription().CalculateHash()), "");
        pResourceView->m_pResource = nullptr;

        DestroyResourceViewPlatform(pResourceView);

        break;
      }
      case GALObjectType::RenderTargetView:
      {
        ezRHIRenderTargetViewHandle hRenderTargetView(ezRHI::ez18_14Id(deadObject.m_uiHandle));
        ezRHIRenderTargetView* pRenderTargetView = nullptr;

        m_RenderTargetViews.Remove(hRenderTargetView, &pRenderTargetView);

        ezRHITexture* pTexture = pRenderTargetView->m_pTexture;
        EZ_ASSERT_DEBUG(pTexture != nullptr, "");
        EZ_VERIFY(pTexture->m_RenderTargetViews.Remove(pRenderTargetView->GetDescription().CalculateHash()), "");
        pRenderTargetView->m_pTexture = nullptr;

        DestroyRenderTargetViewPlatform(pRenderTargetView);

        break;
      }
      case GALObjectType::UnorderedAccessView:
      {
        ezRHIUnorderedAccessViewHandle hUnorderedAccessViewHandle(ezRHI::ez18_14Id(deadObject.m_uiHandle));
        ezRHIUnorderedAccessView* pUnorderedAccesssView = nullptr;

        m_UnorderedAccessViews.Remove(hUnorderedAccessViewHandle, &pUnorderedAccesssView);

        ezRHIResourceBase* pResource = pUnorderedAccesssView->m_pResource;
        EZ_ASSERT_DEBUG(pResource != nullptr, "");

        EZ_VERIFY(pResource->m_UnorderedAccessViews.Remove(pUnorderedAccesssView->GetDescription().CalculateHash()), "");
        pUnorderedAccesssView->m_pResource = nullptr;

        DestroyUnorderedAccessViewPlatform(pUnorderedAccesssView);

        break;
      }
      case GALObjectType::SwapChain:
      {
        ezRHISwapChainHandle hSwapChain(ezRHI::ez16_16Id(deadObject.m_uiHandle));
        ezRHISwapChain* pSwapChain = nullptr;

        m_SwapChains.Remove(hSwapChain, &pSwapChain);

        if (pSwapChain != nullptr)
        {
          DestroySwapChainPlatform(pSwapChain);
        }

        break;
      }
      case GALObjectType::Fence:
      {
        ezRHIFenceHandle hFence(ezRHI::ez20_12Id(deadObject.m_uiHandle));
        ezRHIFence* pFence = nullptr;

        m_Fences.Remove(hFence, &pFence);

        DestroyFencePlatform(pFence);

        break;
      }
      case GALObjectType::Query:
      {
        ezRHIQueryHandle hQuery(ezRHI::ez20_12Id(deadObject.m_uiHandle));
        ezRHIQuery* pQuery = nullptr;

        m_Queries.Remove(hQuery, &pQuery);

        DestroyQueryPlatform(pQuery);

        break;
      }
      case GALObjectType::VertexDeclaration:
      {
        ezRHIVertexDeclarationHandle hVertexDeclaration(ezRHI::ez18_14Id(deadObject.m_uiHandle));
        ezRHIVertexDeclaration* pVertexDeclaration = nullptr;

        m_VertexDeclarations.Remove(hVertexDeclaration, &pVertexDeclaration);
        m_VertexDeclarationTable.Remove(pVertexDeclaration->GetDescription().CalculateHash());

        DestroyVertexDeclarationPlatform(pVertexDeclaration);

        break;
      }
      default:
        EZ_ASSERT_NOT_IMPLEMENTED;
    }
  }

  m_DeadObjects.Clear();
}

EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Device_Implementation_Device);
