#pragma once

#include <RendererVk/RendererVkDLL.h>

namespace ezInternal::Vk
{
  constexpr const char* win32SurfaceExtension = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;

  const ezDynamicArray<const char*> GetValidationLayers();

  const ezDynamicArray<const char*> GetRequiredExtensions(bool debug);

  const ezDynamicArray<const char*> GetRequiredDeviceExtensions();

  bool CheckValidationLayerSupport();

  bool SupportsRequiredExtensions(bool debug);

  bool SupportsRequiredDeviceExtensions(VkPhysicalDevice device);

  VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

  void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

  VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

  void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

  const ezStaticArray<ezInt32, QueueType::Count> FindQueueFamilyIndices(VkPhysicalDevice device);

  ezInt32 FindPresentQueueFamilyIndex(VkPhysicalDevice device);

  bool CheckPhysicalDevice(VkPhysicalDevice device);
} // namespace ezInternal::Vk
