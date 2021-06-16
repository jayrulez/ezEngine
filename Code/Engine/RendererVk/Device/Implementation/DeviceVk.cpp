#include <RendererVkPCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <Foundation/Configuration/Startup.h>
#include <RendererFoundation/CommandEncoder/RenderCommandEncoder.h>
#include <RendererFoundation/Device/DeviceFactory.h>
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
#include <RendererVk/Device/Pipeline.h>

using namespace ezInternal::Vk;

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
  , m_VkDevice{VK_NULL_HANDLE}
  , m_VkInstance{VK_NULL_HANDLE}
  , m_VkPhysicalDevice{VK_NULL_HANDLE}
  , m_VkDebugMessenger{VK_NULL_HANDLE}
  , m_VkPresentQueueFamilyIndex{-1}
  , m_VkPresentQueue{VK_NULL_HANDLE}
{
  for (ezUInt32 i = 0; i < m_VkQueues.GetCount(); i++)
  {
    m_VkQueues[i] = VK_NULL_HANDLE;
  }
}

ezGALDeviceVk::~ezGALDeviceVk() = default;

// vk setup
ezResult ezGALDeviceVk::CreateInstance()
{
  if (!SupportsRequiredExtensions(m_Description.m_bDebugDevice))
  {
    ezLog::Error("Not all required extensions are supported.");
    return EZ_FAILURE;
  }

  if (m_Description.m_bDebugDevice)
  {
    if (!CheckValidationLayerSupport())
    {
      ezLog::Error("Validation layers requested but not available.");
      return EZ_FAILURE;
    }
  }

  ezUInt32 versionMajor = 1; //  reinterpret_cast<ezUInt32>(*(&BUILDSYSTEM_VERSION_MAJOR));
  ezUInt32 versionMinor = 0; //  reinterpret_cast<ezUInt32>(*(&BUILDSYSTEM_VERSION_MINOR));
  ezUInt32 versionPatch = 0; //  reinterpret_cast<ezUInt32>(*(&BUILDSYSTEM_VERSION_PATCH));

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "ezEngine";
  appInfo.applicationVersion = VK_MAKE_VERSION(versionMajor, versionMinor, versionPatch);
  appInfo.pEngineName = "ezEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(versionMajor, versionMinor, versionPatch);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  const ezDynamicArray<const char*> requiredExtensions = GetRequiredExtensions(m_Description.m_bDebugDevice);
  createInfo.enabledExtensionCount = requiredExtensions.GetCount();
  createInfo.ppEnabledExtensionNames = requiredExtensions.GetData();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  const ezDynamicArray<const char*> validationLayers = GetValidationLayers();
  if (m_Description.m_bDebugDevice)
  {
    PopulateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.enabledLayerCount = validationLayers.GetCount();
    createInfo.ppEnabledLayerNames = validationLayers.GetData();
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
  }
  else
  {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }

  if (vkCreateInstance(&createInfo, nullptr, &m_VkInstance) != VK_SUCCESS)
  {
    ezLog::Error("Failed to create Vulkan instance.");
    return EZ_FAILURE;
  }
  return EZ_SUCCESS;
}
ezResult ezGALDeviceVk::SetupDebugMessenger()
{
  if (!m_Description.m_bDebugDevice)
  {
    return EZ_SUCCESS;
  }

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  PopulateDebugMessengerCreateInfo(createInfo);

  if (CreateDebugUtilsMessengerEXT(m_VkInstance, &createInfo, nullptr, &m_VkDebugMessenger) != VK_SUCCESS)
  {
    ezLog::Error("Failed to setup debug messenger.");
    return EZ_FAILURE;
  }
  return EZ_SUCCESS;
}

ezResult ezGALDeviceVk::SetupPhysicalDevice()
{
  ezUInt32 deviceCount = 0;
  vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);

  if (deviceCount == 0)
  {
    ezLog::Error("No device supporting Vulkan was found.");
    return EZ_FAILURE;
  }

  ezDynamicArray<VkPhysicalDevice> devices;
  devices.SetCountUninitialized(deviceCount);
  vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.GetData());

  for (const VkPhysicalDevice& device : devices)
  {
    if (CheckPhysicalDevice(device))
    {
      m_VkPhysicalDevice = device;
      break;
    }
  }

  if (m_VkPhysicalDevice == VK_NULL_HANDLE)
  {
    ezLog::Error("No suitable device was found.");
    return EZ_FAILURE;
  }

  return EZ_SUCCESS;
}

ezResult ezGALDeviceVk::CreateLogicalDevice()
{
  m_QueueFamilyIndices = FindQueueFamilyIndices(m_VkPhysicalDevice);

  m_VkPresentQueueFamilyIndex = FindPresentQueueFamilyIndex(m_VkPhysicalDevice);

  if (m_VkPresentQueueFamilyIndex < 0)
  {
    ezLog::Error("Could not find a queue family that supports present.");
    return EZ_FAILURE;
  }

  ezDynamicArray<ezInt32> allQueueFamilyIndices(m_QueueFamilyIndices);
  if (allQueueFamilyIndices.IndexOf(m_VkPresentQueueFamilyIndex) == ezInvalidIndex)
  {
    allQueueFamilyIndices.PushBack(m_VkPresentQueueFamilyIndex);
  }

  ezDynamicArray<VkDeviceQueueCreateInfo> queueCreateInfos;
  float queuePriority = 1.0f;

  ezDynamicArray<ezInt32> uniqueQueueFamilyIndices;
  for (const auto& familyIndex : allQueueFamilyIndices)
  {
    if (uniqueQueueFamilyIndices.IndexOf(familyIndex) == ezInvalidIndex)
    {
      uniqueQueueFamilyIndices.PushBack(familyIndex);
    }
  }

  for (ezUInt32 i = 0; i < uniqueQueueFamilyIndices.GetCount(); i++)
  {
    if (uniqueQueueFamilyIndices[i] == -1)
      continue;

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = uniqueQueueFamilyIndices[i];
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.PushBack(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfos.GetData();
  createInfo.queueCreateInfoCount = queueCreateInfos.GetCount();
  createInfo.pEnabledFeatures = &deviceFeatures;

  ezDynamicArray<const char*> requiredDeviceExtensions = GetRequiredDeviceExtensions();
  createInfo.enabledExtensionCount = requiredDeviceExtensions.GetCount();
  createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.GetData();

  const ezDynamicArray<const char*> validationLayers = GetValidationLayers();
  if (m_Description.m_bDebugDevice)
  {
    createInfo.enabledLayerCount = validationLayers.GetCount();
    createInfo.ppEnabledLayerNames = validationLayers.GetData();
  }
  else
  {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(m_VkPhysicalDevice, &createInfo, nullptr, &m_VkDevice) != VK_SUCCESS)
  {
    ezLog::Error("Failed to create logical device.");
    return EZ_FAILURE;
  }

  m_VkQueues.SetCount(QueueType::Count);

  for (ezUInt32 i = 0; i < QueueType::Count; i++)
  {
    if (m_QueueFamilyIndices[i] == -1)
      continue;

    vkGetDeviceQueue(m_VkDevice, m_QueueFamilyIndices[i], 0, &m_VkQueues[i]);
  }

  vkGetDeviceQueue(m_VkDevice, m_VkPresentQueueFamilyIndex, 0, &m_VkPresentQueue);

  return EZ_SUCCESS;
}

// Init & shutdown functions

ezResult ezGALDeviceVk::InitPlatform(ezUInt32 dwFlags, void* pUsedAdapter)
{
  EZ_LOG_BLOCK("ezGALDeviceVk::InitPlatform");

  EZ_SUCCEED_OR_RETURN(CreateInstance());
  EZ_SUCCEED_OR_RETURN(SetupDebugMessenger());
  EZ_SUCCEED_OR_RETURN(SetupPhysicalDevice());
  EZ_SUCCEED_OR_RETURN(CreateLogicalDevice());

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
  vkDestroyDevice(m_VkDevice, nullptr);

  DestroyDebugUtilsMessengerEXT(m_VkInstance, m_VkDebugMessenger, nullptr);

  vkDestroyInstance(m_VkInstance, nullptr);

  ReportLiveGpuObjects();

  return EZ_SUCCESS;
}

// Pipeline & Pass functions

void ezGALDeviceVk::BeginPipelinePlatform(const char* szName)
{
  m_pDefaultPass->m_pRenderCommandEncoder->PushMarker(szName);
}

void ezGALDeviceVk::EndPipelinePlatform()
{
  m_pDefaultPass->m_pRenderCommandEncoder->PopMarker();
}

ezGALPass* ezGALDeviceVk::BeginPassPlatform(const char* szName)
{
  m_pDefaultPass->BeginPass(szName);

  return m_pDefaultPass.Borrow();
}

void ezGALDeviceVk::EndPassPlatform(ezGALPass* pPass)
{
  EZ_ASSERT_DEV(m_pDefaultPass.Borrow() == pPass, "Invalid pass");

  m_pDefaultPass->EndPass();
}

// State creation functions

ezGALBlendState* ezGALDeviceVk::CreateBlendStatePlatform(const ezGALBlendStateCreationDescription& Description)
{
  ezGALBlendStateVk* pState = EZ_NEW(&m_Allocator, ezGALBlendStateVk, Description);

  if (pState->InitPlatform(this).Succeeded())
  {
    return pState;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pState);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroyBlendStatePlatform(ezGALBlendState* pBlendState)
{
  ezGALBlendStateVk* pState = static_cast<ezGALBlendStateVk*>(pBlendState);
  pState->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pState);
}

ezGALDepthStencilState* ezGALDeviceVk::CreateDepthStencilStatePlatform(const ezGALDepthStencilStateCreationDescription& Description)
{
  ezGALDepthStencilStateVk* pVkDepthStencilState = EZ_NEW(&m_Allocator, ezGALDepthStencilStateVk, Description);

  if (pVkDepthStencilState->InitPlatform(this).Succeeded())
  {
    return pVkDepthStencilState;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pVkDepthStencilState);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroyDepthStencilStatePlatform(ezGALDepthStencilState* pDepthStencilState)
{
  ezGALDepthStencilStateVk* pVkDepthStencilState = static_cast<ezGALDepthStencilStateVk*>(pDepthStencilState);
  pVkDepthStencilState->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkDepthStencilState);
}

ezGALRasterizerState* ezGALDeviceVk::CreateRasterizerStatePlatform(const ezGALRasterizerStateCreationDescription& Description)
{
  ezGALRasterizerStateVk* pVkRasterizerState = EZ_NEW(&m_Allocator, ezGALRasterizerStateVk, Description);

  if (pVkRasterizerState->InitPlatform(this).Succeeded())
  {
    return pVkRasterizerState;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pVkRasterizerState);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroyRasterizerStatePlatform(ezGALRasterizerState* pRasterizerState)
{
  ezGALRasterizerStateVk* pVkRasterizerState = static_cast<ezGALRasterizerStateVk*>(pRasterizerState);
  pVkRasterizerState->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkRasterizerState);
}

ezGALSamplerState* ezGALDeviceVk::CreateSamplerStatePlatform(const ezGALSamplerStateCreationDescription& Description)
{
  ezGALSamplerStateVk* pVkSamplerState = EZ_NEW(&m_Allocator, ezGALSamplerStateVk, Description);

  if (pVkSamplerState->InitPlatform(this).Succeeded())
  {
    return pVkSamplerState;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pVkSamplerState);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroySamplerStatePlatform(ezGALSamplerState* pSamplerState)
{
  ezGALSamplerStateVk* pVkSamplerState = static_cast<ezGALSamplerStateVk*>(pSamplerState);
  pVkSamplerState->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkSamplerState);
}


// Resource creation functions

ezGALShader* ezGALDeviceVk::CreateShaderPlatform(const ezGALShaderCreationDescription& Description)
{
  ezGALShaderVk* pShader = EZ_NEW(&m_Allocator, ezGALShaderVk, Description);

  if (!pShader->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pShader);
    return nullptr;
  }

  return pShader;
}

void ezGALDeviceVk::DestroyShaderPlatform(ezGALShader* pShader)
{
  ezGALShaderVk* pVkShader = static_cast<ezGALShaderVk*>(pShader);
  pVkShader->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkShader);
}

ezGALBuffer* ezGALDeviceVk::CreateBufferPlatform(const ezGALBufferCreationDescription& Description, ezArrayPtr<const ezUInt8> pInitialData)
{
  ezGALBufferVk* pBuffer = EZ_NEW(&m_Allocator, ezGALBufferVk, Description);

  if (!pBuffer->InitPlatform(this, pInitialData).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pBuffer);
    return nullptr;
  }

  return pBuffer;
}

void ezGALDeviceVk::DestroyBufferPlatform(ezGALBuffer* pBuffer)
{
  ezGALBufferVk* pVkBuffer = static_cast<ezGALBufferVk*>(pBuffer);
  pVkBuffer->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkBuffer);
}

ezGALTexture* ezGALDeviceVk::CreateTexturePlatform(const ezGALTextureCreationDescription& Description, ezArrayPtr<ezGALSystemMemoryDescription> pInitialData)
{
  ezGALTextureVk* pTexture = EZ_NEW(&m_Allocator, ezGALTextureVk, Description);

  if (!pTexture->InitPlatform(this, pInitialData).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pTexture);
    return nullptr;
  }

  return pTexture;
}

void ezGALDeviceVk::DestroyTexturePlatform(ezGALTexture* pTexture)
{
  ezGALTextureVk* pVkTexture = static_cast<ezGALTextureVk*>(pTexture);
  pVkTexture->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkTexture);
}

ezGALResourceView* ezGALDeviceVk::CreateResourceViewPlatform(ezGALResourceBase* pResource, const ezGALResourceViewCreationDescription& Description)
{
  ezGALResourceViewVk* pResourceView = EZ_NEW(&m_Allocator, ezGALResourceViewVk, pResource, Description);

  if (!pResourceView->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pResourceView);
    return nullptr;
  }

  return pResourceView;
}

void ezGALDeviceVk::DestroyResourceViewPlatform(ezGALResourceView* pResourceView)
{
  ezGALResourceViewVk* pVkResourceView = static_cast<ezGALResourceViewVk*>(pResourceView);
  pVkResourceView->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkResourceView);
}

ezGALRenderTargetView* ezGALDeviceVk::CreateRenderTargetViewPlatform(ezGALTexture* pTexture, const ezGALRenderTargetViewCreationDescription& Description)
{
  ezGALRenderTargetViewVk* pRTView = EZ_NEW(&m_Allocator, ezGALRenderTargetViewVk, pTexture, Description);

  if (!pRTView->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pRTView);
    return nullptr;
  }

  return pRTView;
}

void ezGALDeviceVk::DestroyRenderTargetViewPlatform(ezGALRenderTargetView* pRenderTargetView)
{
  ezGALRenderTargetViewVk* pVkRenderTargetView = static_cast<ezGALRenderTargetViewVk*>(pRenderTargetView);
  pVkRenderTargetView->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVkRenderTargetView);
}

ezGALUnorderedAccessView* ezGALDeviceVk::CreateUnorderedAccessViewPlatform(ezGALResourceBase* pTextureOfBuffer, const ezGALUnorderedAccessViewCreationDescription& Description)
{
  ezGALUnorderedAccessViewVk* pUnorderedAccessView = EZ_NEW(&m_Allocator, ezGALUnorderedAccessViewVk, pTextureOfBuffer, Description);

  if (!pUnorderedAccessView->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pUnorderedAccessView);
    return nullptr;
  }

  return pUnorderedAccessView;
}

void ezGALDeviceVk::DestroyUnorderedAccessViewPlatform(ezGALUnorderedAccessView* pUnorderedAccessView)
{
  ezGALUnorderedAccessViewVk* pUnorderedAccessViewVk = static_cast<ezGALUnorderedAccessViewVk*>(pUnorderedAccessView);
  pUnorderedAccessViewVk->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pUnorderedAccessViewVk);
}



// Other rendering creation functions
ezGALSwapChain* ezGALDeviceVk::CreateSwapChainPlatform(const ezGALSwapChainCreationDescription& Description)
{
  ezGALSwapChainVk* pSwapChain = EZ_NEW(&m_Allocator, ezGALSwapChainVk, Description);

  if (!pSwapChain->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pSwapChain);
    return nullptr;
  }

  return pSwapChain;
}

void ezGALDeviceVk::DestroySwapChainPlatform(ezGALSwapChain* pSwapChain)
{
  ezGALSwapChainVk* pSwapChainVk = static_cast<ezGALSwapChainVk*>(pSwapChain);
  pSwapChainVk->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pSwapChainVk);
}

ezGALFence* ezGALDeviceVk::CreateFencePlatform()
{
  ezGALFenceVk* pFence = EZ_NEW(&m_Allocator, ezGALFenceVk);

  if (!pFence->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pFence);
    return nullptr;
  }

  return pFence;
}

void ezGALDeviceVk::DestroyFencePlatform(ezGALFence* pFence)
{
  ezGALFenceVk* pFenceVk = static_cast<ezGALFenceVk*>(pFence);
  pFenceVk->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pFenceVk);
}

ezGALQuery* ezGALDeviceVk::CreateQueryPlatform(const ezGALQueryCreationDescription& Description)
{
  ezGALQueryVk* pQuery = EZ_NEW(&m_Allocator, ezGALQueryVk, Description);

  if (!pQuery->InitPlatform(this).Succeeded())
  {
    EZ_DELETE(&m_Allocator, pQuery);
    return nullptr;
  }

  return pQuery;
}

void ezGALDeviceVk::DestroyQueryPlatform(ezGALQuery* pQuery)
{
  ezGALQueryVk* pQueryVk = static_cast<ezGALQueryVk*>(pQuery);
  pQueryVk->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pQueryVk);
}

ezGALVertexDeclaration* ezGALDeviceVk::CreateVertexDeclarationPlatform(const ezGALVertexDeclarationCreationDescription& Description)
{
  ezGALVertexDeclarationVk* pVertexDeclaration = EZ_NEW(&m_Allocator, ezGALVertexDeclarationVk, Description);

  if (pVertexDeclaration->InitPlatform(this).Succeeded())
  {
    return pVertexDeclaration;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pVertexDeclaration);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroyVertexDeclarationPlatform(ezGALVertexDeclaration* pVertexDeclaration)
{
  ezGALVertexDeclarationVk* pVertexDeclarationVk = static_cast<ezGALVertexDeclarationVk*>(pVertexDeclaration);
  pVertexDeclarationVk->DeInitPlatform(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pVertexDeclarationVk);
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

Pipeline* ezGALDeviceVk::CreatePipeline(const PipelineStateDesc& desc)
{
  Pipeline* pPipeline = EZ_NEW(&m_Allocator, Pipeline, desc);

  if (pPipeline->Init(this).Succeeded())
  {
    return pPipeline;
  }
  else
  {
    EZ_DELETE(&m_Allocator, pPipeline);
    return nullptr;
  }
}

void ezGALDeviceVk::DestroyPipeline(Pipeline* pPipeline)
{
  pPipeline->Destroy(this).IgnoreResult();
  EZ_DELETE(&m_Allocator, pPipeline);
}

EZ_STATICLINK_FILE(RendererVk, RendererVk_Device_Implementation_DeviceVk);
