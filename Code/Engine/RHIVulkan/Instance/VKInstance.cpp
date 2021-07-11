#include <RHIVulkanPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/Instance/VKInstance.h>
#include <set>
#include <sstream>
#include <string>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

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


static bool SkipIt(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT object_type, const ezString& message)
{
  if (object_type == VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT && flags != VK_DEBUG_REPORT_ERROR_BIT_EXT)
    return true;
  static ezDynamicArray<ezString> muted_warnings;
  if (muted_warnings.IsEmpty())
  {
    muted_warnings.PushBack("UNASSIGNED-CoreValidation-Shader-InconsistentSpirv");
    muted_warnings.PushBack("VUID-vkCmdDrawIndexed-None-04007");
    muted_warnings.PushBack("VUID-vkDestroyDevice-device-00378");
    muted_warnings.PushBack("VUID-VkSubmitInfo-pWaitSemaphores-03243");
    muted_warnings.PushBack("VUID-VkSubmitInfo-pSignalSemaphores-03244");
    muted_warnings.PushBack("VUID-vkCmdPipelineBarrier-pDependencies-02285");
    muted_warnings.PushBack("VUID-VkImageMemoryBarrier-oldLayout-01213");
    muted_warnings.PushBack("VUID-vkCmdDrawIndexed-None-02721");
    muted_warnings.PushBack("VUID-vkCmdDrawIndexed-None-02699");
    muted_warnings.PushBack("VUID-vkCmdTraceRaysKHR-None-02699");
    muted_warnings.PushBack("VUID-VkShaderModuleCreateInfo-pCode-04147");
  }

  for (auto& str : muted_warnings)
  {
    if (message.FindSubString(str) != nullptr)
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
  //#ifdef _WIN32
  //  if (error_count < error_limit)
  //  {
  //    std::stringstream buf;
  //    buf << pLayerPrefix << " " << to_string(static_cast<vk::DebugReportFlagBitsEXT>(flags)) << " " << pMessage << std::endl;
  //    ezLog::Error(buf.str().c_str());
  //  }
  //#endif
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
  PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
  VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

  auto layers = vk::enumerateInstanceLayerProperties();

  ezSet<ezString> req_layers;
  static const bool debug_enabled = true; // IsDebuggerPresent();
  if (debug_enabled)
    req_layers.Insert("VK_LAYER_KHRONOS_validation");
  ezDynamicArray<const char*> found_layers;
  for (const auto& layer : layers)
  {
    if (req_layers.Contains(layer.layerName.data()))
      found_layers.PushBack(layer.layerName);
  }

  auto extensions = vk::enumerateInstanceExtensionProperties();

  ezSet<ezString> req_extension;
  req_extension.Insert(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  req_extension.Insert(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
  req_extension.Insert(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
  req_extension.Insert(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
  req_extension.Insert(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
  req_extension.Insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  req_extension.Insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

  ezDynamicArray<const char*> found_extension;
  for (const auto& extension : extensions)
  {
    if (req_extension.Contains(extension.extensionName.data()))
      found_extension.PushBack(extension.extensionName);
  }

  vk::ApplicationInfo app_info = {};
  app_info.apiVersion = VK_API_VERSION_1_2;

  vk::InstanceCreateInfo create_info;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledLayerCount = found_layers.GetCount();
  create_info.ppEnabledLayerNames = found_layers.GetData();
  create_info.enabledExtensionCount = found_extension.GetCount();
  create_info.ppEnabledExtensionNames = found_extension.GetData();

  m_instance = vk::createInstanceUnique(create_info);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance.get());
  if (debug_enabled)
  {
    vk::DebugReportCallbackCreateInfoEXT callback_create_info = {};
    callback_create_info.flags = vk::DebugReportFlagBitsEXT::eWarning |
                                 vk::DebugReportFlagBitsEXT::ePerformanceWarning |
                                 vk::DebugReportFlagBitsEXT::eError |
                                 vk::DebugReportFlagBitsEXT::eDebug;
    callback_create_info.pfnCallback = &DebugReportCallback;
    callback_create_info.pUserData = this;
    m_callback = m_instance->createDebugReportCallbackEXTUnique(callback_create_info);
  }
}

std::vector<std::shared_ptr<Adapter>> VKInstance::EnumerateAdapters()
{
  std::vector<std::shared_ptr<Adapter>> adapters;
  auto devices = m_instance->enumeratePhysicalDevices();
  for (const auto& device : devices)
  {
    vk::PhysicalDeviceProperties device_properties = device.getProperties();

    if (device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
        device_properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
    {
      adapters.emplace_back(std::make_shared<VKAdapter>(*this, device));
    }
  }
  return adapters;
}

vk::Instance& VKInstance::GetInstance()
{
  return m_instance.get();
}
