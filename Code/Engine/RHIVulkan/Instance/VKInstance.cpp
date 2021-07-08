#include <RHIVulkanPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/Instance/VKInstance.h>
#include <set>
#include <sstream>
#include <string>

//ULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

std::shared_ptr<Instance> CreateVKInstance()
{
  return std::make_shared<VKInstance>();
}

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(RHIVulkan, InstanceFactory)

ON_CORESYSTEMS_STARTUP
{
  InstanceFactory::RegisterCreatorFunc(ApiType::kVulkan, &CreateVKInstance);
}

ON_CORESYSTEMS_SHUTDOWN
{
  InstanceFactory::UnregisterCreatorFunc(ApiType::kVulkan);
}

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on

static bool SkipIt(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT object_type, const std::string& message)
{
  if (object_type == VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT && flags != VK_DEBUG_REPORT_ERROR_BIT_EXT)
    return true;
  static std::vector<std::string> muted_warnings = {
    "UNASSIGNED-CoreValidation-Shader-InconsistentSpirv",
    "VUID-vkCmdDrawIndexed-None-04007",
    "VUID-vkDestroyDevice-device-00378",
    "VUID-VkSubmitInfo-pWaitSemaphores-03243",
    "VUID-VkSubmitInfo-pSignalSemaphores-03244",
    "VUID-vkCmdPipelineBarrier-pDependencies-02285",
    "VUID-VkImageMemoryBarrier-oldLayout-01213",
    "VUID-vkCmdDrawIndexed-None-02721",
    "VUID-vkCmdDrawIndexed-None-02699",
    "VUID-vkCmdTraceRaysKHR-None-02699",
    "VUID-VkShaderModuleCreateInfo-pCode-04147",
  };
  for (auto& str : muted_warnings)
  {
    if (message.find(str) != std::string::npos)
      return true;
  }
  return false;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
  VkDebugReportFlagsEXT flags,
  VkDebugReportObjectTypeEXT objectType,
  uint64_t object,
  size_t location,
  int32_t messageCode,
  const char* pLayerPrefix,
  const char* pMessage,
  void* pUserData)
{
  constexpr size_t error_limit = 1024;
  static size_t error_count = 0;
  if (error_count >= error_limit || SkipIt(flags, objectType, pMessage))
    return VK_FALSE;
  if (error_count < error_limit)
  {
    ezStringBuilder builder;
    builder.Format("{} {} {}", pLayerPrefix, static_cast<VkDebugReportFlagBitsEXT>(flags), pMessage);

    ezLog::Debug(builder.GetData());
  }

  ++error_count;
  return VK_FALSE;
}

VKInstance::VKInstance()
{
  if (volkInitialize() != VK_SUCCESS)
  {
    EZ_REPORT_FAILURE("Volk initialization failed.");
  }

  ezUInt32 layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  ezDynamicArray<VkLayerProperties> availableLayers;
  availableLayers.SetCountUninitialized(layerCount);

  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetData());

  std::set<std::string> req_layers;
  static const bool debug_enabled = true; //IsDebuggerPresent();
  if (debug_enabled)
    req_layers.insert("VK_LAYER_KHRONOS_validation");
  std::vector<const char*> found_layers;
  for (const auto& layerProperties : availableLayers)
  {
    if (req_layers.count(layerProperties.layerName))
      found_layers.push_back(layerProperties.layerName);
  }

  ezUInt32 extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  ezDynamicArray<VkExtensionProperties> availableExtensions;
  availableExtensions.SetCountUninitialized(extensionCount);

  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.GetData());

  std::set<std::string> req_extension = {
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
    VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
  };
  std::vector<const char*> found_extension;
  for (const auto& extension : availableExtensions)
  {
    if (req_extension.count(extension.extensionName))
      found_extension.push_back(extension.extensionName);
  }

  ezUInt32 versionMajor = 1; //  reinterpret_cast<ezUInt32>(*(&BUILDSYSTEM_VERSION_MAJOR));
  ezUInt32 versionMinor = 0; //  reinterpret_cast<ezUInt32>(*(&BUILDSYSTEM_VERSION_MINOR));
  ezUInt32 versionPatch = 0; //  reinterpret_cast<ezUInt32>(*(&BUILDSYSTEM_VERSION_PATCH));

  VkApplicationInfo app_info = {};
  app_info.apiVersion = VK_API_VERSION_1_2;
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "ezEngine";
  app_info.applicationVersion = VK_MAKE_VERSION(versionMajor, versionMinor, versionPatch);
  app_info.pEngineName = "ezEngine";
  app_info.engineVersion = VK_MAKE_VERSION(versionMajor, versionMinor, versionPatch);
  app_info.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledLayerCount = static_cast<uint32_t>(found_layers.size());
  create_info.ppEnabledLayerNames = found_layers.data();
  create_info.enabledExtensionCount = static_cast<uint32_t>(found_extension.size());
  create_info.ppEnabledExtensionNames = found_extension.data();

  if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
  {
    EZ_REPORT_FAILURE("Failed to create Vulkan instance.");
  }

  volkLoadInstance(m_instance);

  if (debug_enabled)
  {
    VkDebugReportCallbackCreateInfoEXT callback_create_info = {};
    callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    callback_create_info.flags = VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                 VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                                 VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                 VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    callback_create_info.pfnCallback = &DebugReportCallback;
    callback_create_info.pUserData = this;

    vkCreateDebugReportCallbackEXT(m_instance, &callback_create_info, nullptr, &m_callback);
  }
}

std::vector<std::shared_ptr<Adapter>> VKInstance::EnumerateAdapters()
{
  std::vector<std::shared_ptr<Adapter>> adapters;

  ezUInt32 physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);

  if (physicalDeviceCount == 0)
  {
    EZ_REPORT_FAILURE("No device supporting Vulkan was found.");
    return {};
  }

  ezDynamicArray<VkPhysicalDevice> physicalDevices;
  physicalDevices.SetCountUninitialized(physicalDeviceCount);
  vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.GetData());

  for (const VkPhysicalDevice& physicalDevice : physicalDevices)
  {
    VkPhysicalDeviceProperties properties = {};

    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    if (properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
        properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
    {
      adapters.emplace_back(std::make_shared<VKAdapter>(*this, physicalDevice));
    }
  }

  return adapters;
}

VkInstance& VKInstance::GetInstance()
{
  return m_instance;
}
