#include <RHIVulkanPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <RHI/Instance/InstanceFactory.h>
#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/Instance/VKInstance.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

ezRHIInstance* CreateVKInstance()
{
  return EZ_DEFAULT_NEW(ezRHIVKInstance);
}

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(RHIVulkan, InstanceFactory)

ON_CORESYSTEMS_STARTUP
{
  ezRHIInstanceFactory::RegisterCreatorFunc(ezRHIApiType::Vulkan, &CreateVKInstance);
}

ON_CORESYSTEMS_SHUTDOWN
{
  ezRHIInstanceFactory::UnregisterCreatorFunc(ezRHIApiType::Vulkan);
}

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on

static bool SkipIt(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, const ezString& message)
{
  if (objectType == VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT && flags != VK_DEBUG_REPORT_ERROR_BIT_EXT)
    return true;
  static ezDynamicArray<ezString> mutedWarnings;

  if (mutedWarnings.IsEmpty())
  {
    mutedWarnings.PushBack("UNASSIGNED-CoreValidation-Shader-InconsistentSpirv");
    mutedWarnings.PushBack("VUID-vkCmdDrawIndexed-None-04007");
    mutedWarnings.PushBack("VUID-vkDestroyDevice-device-00378");
    mutedWarnings.PushBack("VUID-VkSubmitInfo-pWaitSemaphores-03243");
    mutedWarnings.PushBack("VUID-VkSubmitInfo-pSignalSemaphores-03244");
    mutedWarnings.PushBack("VUID-vkCmdPipelineBarrier-pDependencies-02285");
    mutedWarnings.PushBack("VUID-VkImageMemoryBarrier-oldLayout-01213");
    mutedWarnings.PushBack("VUID-vkCmdDrawIndexed-None-02721");
    mutedWarnings.PushBack("VUID-vkCmdDrawIndexed-None-02699");
    mutedWarnings.PushBack("VUID-vkCmdTraceRaysKHR-None-02699");
    mutedWarnings.PushBack("VUID-VkShaderModuleCreateInfo-pCode-04147");
  }

  for (auto& mutedWarning : mutedWarnings)
  {
    if (message.FindSubString(mutedWarning) != nullptr)
      return true;
  }
  return false;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
  VkDebugReportFlagsEXT flags,
  VkDebugReportObjectTypeEXT objectType,
  ezUInt64 object,
  size_t location,
  ezInt32 messageCode,
  const char* pLayerPrefix,
  const char* pMessage,
  void* pUserData)
{
  constexpr ezUInt32 errorLimit = 1024;
  static ezUInt32 errorCount = 0;
  if (errorCount >= errorLimit || SkipIt(flags, objectType, pMessage))
    return VK_FALSE;
  //#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  if (errorCount < errorLimit)
  {
    ezStringBuilder builder;
    builder.Format("{} {} {}", pLayerPrefix, to_string(static_cast<vk::DebugReportFlagBitsEXT>(flags)).data(), pMessage);

    ezLog::Debug(builder.GetData());
  }
  //#endif
  ++errorCount;
  return VK_FALSE;
}

ezRHIVKInstance::ezRHIVKInstance()
{
  PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_DynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
  VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

  auto layers = vk::enumerateInstanceLayerProperties();

  ezSet<ezString> reqLayers;
  static const bool debugEnabled = IsDebuggerPresent();
  if (debugEnabled)
    reqLayers.Insert("VK_LAYER_KHRONOS_validation");
  ezDynamicArray<const char*> foundLayers;
  for (const auto& layer : layers)
  {
    if (reqLayers.Contains(layer.layerName.data()))
      foundLayers.PushBack(layer.layerName);
  }

  auto extensions = vk::enumerateInstanceExtensionProperties();

  ezSet<ezString> reqExtensions;
  reqExtensions.Insert(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  reqExtensions.Insert(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(VK_USE_PLATFORM_WIN32_KHR)
  reqExtensions.Insert(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
  reqExtensions.Insert(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
  reqExtensions.Insert(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

  reqExtensions.Insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  reqExtensions.Insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

  ezDynamicArray<const char*> foundExtension;
  for (const auto& extension : extensions)
  {
    if (reqExtensions.Contains(extension.extensionName.data()))
      foundExtension.PushBack(extension.extensionName);
  }

  vk::ApplicationInfo appInfo = {};
  appInfo.apiVersion = VK_API_VERSION_1_2;

  vk::InstanceCreateInfo createInfo;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledLayerCount = foundLayers.GetCount();
  createInfo.ppEnabledLayerNames = foundLayers.GetData();
  createInfo.enabledExtensionCount = foundExtension.GetCount();
  createInfo.ppEnabledExtensionNames = foundExtension.GetData();

  m_Instance = vk::createInstanceUnique(createInfo);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance.get());
  if (debugEnabled)
  {
    vk::DebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
    callbackCreateInfo.flags = vk::DebugReportFlagBitsEXT::eWarning |
                               vk::DebugReportFlagBitsEXT::ePerformanceWarning |
                               vk::DebugReportFlagBitsEXT::eError |
                               vk::DebugReportFlagBitsEXT::eDebug;
    callbackCreateInfo.pfnCallback = &DebugReportCallback;
    callbackCreateInfo.pUserData = this;
    m_Callback = m_Instance->createDebugReportCallbackEXTUnique(callbackCreateInfo);
  }
}

ezDynamicArray<ezRHIAdapter*> ezRHIVKInstance::EnumerateAdapters()
{
  ezDynamicArray<ezRHIAdapter*> adapters;
  auto devices = m_Instance->enumeratePhysicalDevices();
  for (const auto& device : devices)
  {
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
        deviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
    {
      adapters.PushBack(EZ_DEFAULT_NEW(ezRHIVKAdapter, *this, device));
    }
  }
  return adapters;
}

vk::Instance& ezRHIVKInstance::GetInstance()
{
  return m_Instance.get();
}
