#include <RendererVkPCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <Foundation/Configuration/Startup.h>
#include <RendererVk/CommandEncoder/CommandEncoderImplVk.h>
#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Device/PassVk.h>
#include <RendererVk/Device/SwapChainVk.h>
#include <RendererVk/Resources/BufferVk.h>
#include <RendererVk/Resources/FenceVk.h>
#include <RendererVk/Resources/QueryVk.h>
#include <RendererVk/Resources/RenderTargetViewVk.h>
#include <RendererVk/Resources/ResourceViewVk.h>
#include <RendererVk/Resources/TextureVk.h>
#include <RendererVk/Resources/UnorderedAccessViewVk.h>
#include <RendererVk/Shader/ShaderVk.h>
#include <RendererVk/Shader/VertexDeclarationVk.h>
#include <RendererVk/State/StateVk.h>
#include <RendererFoundation/CommandEncoder/RenderCommandEncoder.h>
#include <RendererFoundation/Device/DeviceFactory.h>


ezInternal::NewInstance<ezGALDevice> CreateVkDevice(ezAllocatorBase* pAllocator, const ezGALDeviceCreationDescription& Description)
{
  return EZ_NEW(pAllocator, ezGALDeviceVk, Description);
}

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(RendererVk, DeviceFactory)

ON_CORESYSTEMS_STARTUP
{
  ezGALDeviceFactory::RegisterCreatorFunc("Vk", &CreateVkDevice, "VULKAN", "ezShaderCompilerDXC");
}

ON_CORESYSTEMS_SHUTDOWN
{
  ezGALDeviceFactory::UnregisterCreatorFunc("Vk");
}

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on

ezGALDeviceVk::ezGALDeviceVk(const ezGALDeviceCreationDescription& Description)
  : ezGALDevice(Description)
{
}

ezGALDeviceVk::~ezGALDeviceVk() = default;

// Init & shutdown functions

ezResult ezGALDeviceVk::InitPlatform(ezUInt32 dwFlags, void* pUsedAdapter)
{
  return EZ_SUCCESS;
}

ezResult ezGALDeviceVk::InitPlatform()
{
  return InitPlatform(0, nullptr);
}

void ezGALDeviceVk::ReportLiveGpuObjects()
{

}

ezResult ezGALDeviceVk::ShutdownPlatform()
{
  return EZ_SUCCESS;
}

// Pipeline & Pass functions

void ezGALDeviceVk::BeginPipelinePlatform(const char* szName)
{
}

void ezGALDeviceVk::EndPipelinePlatform()
{
}

ezGALPass* ezGALDeviceVk::BeginPassPlatform(const char* szName)
{
  return nullptr;
}

void ezGALDeviceVk::EndPassPlatform(ezGALPass* pPass)
{
}

// State creation functions

ezGALBlendState* ezGALDeviceVk::CreateBlendStatePlatform(const ezGALBlendStateCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyBlendStatePlatform(ezGALBlendState* pBlendState)
{
}

ezGALDepthStencilState* ezGALDeviceVk::CreateDepthStencilStatePlatform(const ezGALDepthStencilStateCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyDepthStencilStatePlatform(ezGALDepthStencilState* pDepthStencilState)
{
}

ezGALRasterizerState* ezGALDeviceVk::CreateRasterizerStatePlatform(const ezGALRasterizerStateCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyRasterizerStatePlatform(ezGALRasterizerState* pRasterizerState)
{
}

ezGALSamplerState* ezGALDeviceVk::CreateSamplerStatePlatform(const ezGALSamplerStateCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroySamplerStatePlatform(ezGALSamplerState* pSamplerState)
{
}


// Resource creation functions

ezGALShader* ezGALDeviceVk::CreateShaderPlatform(const ezGALShaderCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyShaderPlatform(ezGALShader* pShader)
{
}

ezGALBuffer* ezGALDeviceVk::CreateBufferPlatform(const ezGALBufferCreationDescription& Description, ezArrayPtr<const ezUInt8> pInitialData)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyBufferPlatform(ezGALBuffer* pBuffer)
{
}

ezGALTexture* ezGALDeviceVk::CreateTexturePlatform(const ezGALTextureCreationDescription& Description, ezArrayPtr<ezGALSystemMemoryDescription> pInitialData)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyTexturePlatform(ezGALTexture* pTexture)
{
}

ezGALResourceView* ezGALDeviceVk::CreateResourceViewPlatform(ezGALResourceBase* pResource, const ezGALResourceViewCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyResourceViewPlatform(ezGALResourceView* pResourceView)
{
}

ezGALRenderTargetView* ezGALDeviceVk::CreateRenderTargetViewPlatform(ezGALTexture* pTexture, const ezGALRenderTargetViewCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyRenderTargetViewPlatform(ezGALRenderTargetView* pRenderTargetView)
{
}

ezGALUnorderedAccessView* ezGALDeviceVk::CreateUnorderedAccessViewPlatform(ezGALResourceBase* pTextureOfBuffer, const ezGALUnorderedAccessViewCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyUnorderedAccessViewPlatform(ezGALUnorderedAccessView* pUnorderedAccessView)
{
}



// Other rendering creation functions
ezGALSwapChain* ezGALDeviceVk::CreateSwapChainPlatform(const ezGALSwapChainCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroySwapChainPlatform(ezGALSwapChain* pSwapChain)
{
}

ezGALFence* ezGALDeviceVk::CreateFencePlatform()
{
  return nullptr;
}

void ezGALDeviceVk::DestroyFencePlatform(ezGALFence* pFence)
{
}

ezGALQuery* ezGALDeviceVk::CreateQueryPlatform(const ezGALQueryCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyQueryPlatform(ezGALQuery* pQuery)
{
}

ezGALVertexDeclaration* ezGALDeviceVk::CreateVertexDeclarationPlatform(const ezGALVertexDeclarationCreationDescription& Description)
{
  return nullptr;
}

void ezGALDeviceVk::DestroyVertexDeclarationPlatform(ezGALVertexDeclaration* pVertexDeclaration)
{
}

ezGALTimestampHandle ezGALDeviceVk::GetTimestampPlatform()
{
  return {0, 0};
}

ezResult ezGALDeviceVk::GetTimestampResultPlatform(ezGALTimestampHandle hTimestamp, ezTime& result)
{
  return EZ_SUCCESS;
}

// Swap chain functions

void ezGALDeviceVk::PresentPlatform(ezGALSwapChain* pSwapChain, bool bVSync)
{
}

// Misc functions

void ezGALDeviceVk::BeginFramePlatform()
{
}

void ezGALDeviceVk::EndFramePlatform()
{
}

void ezGALDeviceVk::SetPrimarySwapChainPlatform(ezGALSwapChain* pSwapChain)
{
}


void ezGALDeviceVk::FillCapabilitiesPlatform()
{
}

EZ_STATICLINK_FILE(RendererVk, RendererVk_Device_Implementation_DeviceVk);
