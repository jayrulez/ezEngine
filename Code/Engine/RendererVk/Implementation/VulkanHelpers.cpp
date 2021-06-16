#include <RendererVk/Implementation/VulkanHelpers.h>

using QueueType = ezInternal::Vk::QueueType;

const ezDynamicArray<const char*> ezInternal::Vk::GetValidationLayers()
{
  ezDynamicArray<const char*> validationLayers;

  validationLayers.PushBack("VK_LAYER_KHRONOS_validation");

  return validationLayers;
}

const ezDynamicArray<const char*> ezInternal::Vk::GetRequiredExtensions(bool debug)
{
  ezDynamicArray<const char*> extensions;

  if (debug)
    extensions.PushBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  extensions.PushBack(VK_KHR_SURFACE_EXTENSION_NAME);

  extensions.PushBack(win32SurfaceExtension); // todo: cross platform

  return extensions;
}

const ezDynamicArray<const char*> ezInternal::Vk::GetRequiredDeviceExtensions()
{
  ezDynamicArray<const char*> extensions;

  extensions.PushBack(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  return extensions;
}

bool ezInternal::Vk::CheckValidationLayerSupport()
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

bool ezInternal::Vk::SupportsRequiredExtensions(bool debug)
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

bool ezInternal::Vk::SupportsRequiredDeviceExtensions(VkPhysicalDevice device)
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

VKAPI_ATTR VkBool32 VKAPI_CALL ezInternal::Vk::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
  if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) != 0)
  {
    ezLog::Debug(pCallbackData->pMessage);
  }
  else if ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) != 0)
  {
    ezLog::Info(pCallbackData->pMessage);
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

void ezInternal::Vk::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallback;
  createInfo.pUserData = nullptr;
}

VkResult ezInternal::Vk::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
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

void ezInternal::Vk::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr)
  {
    func(instance, debugMessenger, pAllocator);
  }
}

const ezStaticArray<ezInt32, QueueType::Count> ezInternal::Vk::FindQueueFamilyIndices(VkPhysicalDevice device)
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

ezInt32 ezInternal::Vk::FindPresentQueueFamilyIndex(VkPhysicalDevice device)
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

bool ezInternal::Vk::CheckPhysicalDevice(VkPhysicalDevice device)
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
