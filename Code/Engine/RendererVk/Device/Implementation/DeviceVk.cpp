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

EZ_DEFINE_AS_POD_TYPE(VkExtensionProperties);
EZ_DEFINE_AS_POD_TYPE(VkLayerProperties);
EZ_DEFINE_AS_POD_TYPE(VkPhysicalDevice);
EZ_DEFINE_AS_POD_TYPE(VkQueueFamilyProperties);
EZ_DEFINE_AS_POD_TYPE(VkDeviceQueueCreateInfo);

using QueueType = ezInternal::Vk::QueueType;

constexpr const char* win32SurfaceExtension = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;

const ezDynamicArray<const char*> GetValidationLayers()
{
  ezDynamicArray<const char*> validationLayers;

  validationLayers.PushBack("VK_LAYER_KHRONOS_validation");

  return validationLayers;
}

const ezDynamicArray<const char*> GetRequiredExtensions(bool debug)
{
  ezDynamicArray<const char*> extensions;

  if (debug)
    extensions.PushBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  extensions.PushBack(VK_KHR_SURFACE_EXTENSION_NAME);

  extensions.PushBack(win32SurfaceExtension); // todo: cross platform

  return extensions;
}

const ezDynamicArray<const char*> GetRequiredDeviceExtensions()
{
  ezDynamicArray<const char*> extensions;

  extensions.PushBack(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  return extensions;
}

bool CheckValidationLayerSupport()
{
  ezUInt32 layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  ezDynamicArray<VkLayerProperties> availableLayers;
  availableLayers.SetCountUninitialized(layerCount);

  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetData());

  for (const char* validationLayer : GetValidationLayers())
  {
    bool layerFound = false;

    for (const VkLayerProperties& layerProperties : availableLayers)
    {
      if (ezStringUtils::Compare(validationLayer, layerProperties.layerName) == 0)
      {
        layerFound = true;
        break;
      }
    }

    if (!layerFound)
    {
      return false;
    }
  }
  return true;
}

bool SupportsRequiredExtensions(bool debug)
{
  ezUInt32 extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  ezDynamicArray<VkExtensionProperties> availableExtensions;
  availableExtensions.SetCountUninitialized(extensionCount);

  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.GetData());

  for (const char* extension : GetRequiredExtensions(debug))
  {
    bool extensionFound = false;
    for (const VkExtensionProperties& extensionProperties : availableExtensions)
    {
      if (ezStringUtils::Compare(extension, extensionProperties.extensionName) == 0)
      {
        extensionFound = true;
        break;
      }
    }

    if (!extensionFound)
    {
      return false;
    }
  }

  return true;
}


bool SupportsRequiredDeviceExtensions(VkPhysicalDevice device)
{
  ezUInt32 extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  ezDynamicArray<VkExtensionProperties> availableExtensions;
  availableExtensions.SetCount(extensionCount);

  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.GetData());

  for (const char* extension : GetRequiredDeviceExtensions())
  {
    bool extensionFound = false;
    for (const VkExtensionProperties& extensionProperties : availableExtensions)
    {
      if (ezStringUtils::Compare(extension, extensionProperties.extensionName) == 0)
      {
        extensionFound = true;
        break;
      }
    }

    if (!extensionFound)
    {
      return false;
    }
  }

  return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
  if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) != 0)
  {
    //ezLog::Debug(pCallbackData->pMessage);
  }
  else if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) != 0)
  {
    //ezLog::Info(pCallbackData->pMessage);
  }
  else if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0)
  {
    ezLog::Warning(pCallbackData->pMessage);
  }
  else if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0)
  {
    ezLog::Error(pCallbackData->pMessage);
  }

  return VK_FALSE;
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallback;
  createInfo.pUserData = nullptr;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr)
  {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }
  else
  {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr)
  {
    func(instance, debugMessenger, pAllocator);
  }
}

const ezStaticArray<ezInt32, QueueType::Count> FindQueueFamilyIndices(VkPhysicalDevice device)
{
  ezStaticArray<ezInt32, QueueType::Count> indices;
  indices.SetCount(QueueType::Count);
  for (ezUInt32 i = 0; i < indices.GetCount(); i++)
  {
    indices[i] = -1;
  }

  ezUInt32 queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  ezDynamicArray<VkQueueFamilyProperties> queueFamilies;
  queueFamilies.SetCount(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.GetData());

  for (ezUInt32 i = 0; i < queueFamilies.GetCount(); i++)
  {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      indices[QueueType::Graphics] = i;
    }
    if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
    {
      indices[QueueType::Compute] = i;
    }
    if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
    {
      indices[QueueType::Copy] = i;
    }
  }

  return indices;
}

ezInt32 FindPresentQueueFamilyIndex(VkPhysicalDevice device)
{
  ezUInt32 queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  ezDynamicArray<VkQueueFamilyProperties> queueFamilies;
  queueFamilies.SetCountUninitialized(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.GetData());

  // todo: make this work cross platform
  for (ezUInt32 i = 0; i < queueFamilies.GetCount(); i++)
  {
    if (vkGetPhysicalDeviceWin32PresentationSupportKHR(device, i) == VK_TRUE)
      return i;
  }

  return -1;
}

bool CheckPhysicalDevice(VkPhysicalDevice device)
{
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);

  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  const ezStaticArray<ezInt32, QueueType::Count> queueFamilyIndices = FindQueueFamilyIndices(device);

  if (queueFamilyIndices[QueueType::Graphics] == -1 || queueFamilyIndices[QueueType::Compute] == -1 || queueFamilyIndices[QueueType::Copy] == -1)
  {
    return false;
  }

  if (!SupportsRequiredDeviceExtensions(device))
  {
    return false;
  }

  return true;
}

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
  for (ezUInt32 i = 0; i < m_VkQueues.GetCount(); i ++)
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
