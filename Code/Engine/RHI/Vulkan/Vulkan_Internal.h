#pragma once
#include <RHI/Descriptors/Descriptors.h>
#include <RHI/GraphicsDevice.h>
#include <RHI/RHIDLL.h>
#include <RHI/RHIInternal.h>
#include <RHI/RHIPCH.h>
#include <RHI/Resources/Resources.h>

#ifdef _WIN32
#  define VK_USE_PLATFORM_WIN32_KHR
#endif // _WIN32

#define VK_ENABLE_BETA_EXTENSIONS
#include <RHI/Vulkan/ThirdParty/vk_mem_alloc.h>
#include <vulkan/vulkan.h>


#include <deque>
#include <mutex>


EZ_DEFINE_AS_POD_TYPE(VkDescriptorSetLayoutBinding);
EZ_DEFINE_AS_POD_TYPE(VkImageMemoryBarrier);
EZ_DEFINE_AS_POD_TYPE(VkWriteDescriptorSet);
EZ_DEFINE_AS_POD_TYPE(VkDescriptorBufferInfo);
EZ_DEFINE_AS_POD_TYPE(VkDescriptorImageInfo);
EZ_DEFINE_AS_POD_TYPE(VkWriteDescriptorSetAccelerationStructureNV);

EZ_DEFINE_AS_POD_TYPE(VkExtensionProperties);
EZ_DEFINE_AS_POD_TYPE(VkLayerProperties);
EZ_DEFINE_AS_POD_TYPE(VkQueueFamilyProperties);
EZ_DEFINE_AS_POD_TYPE(VkSurfaceFormatKHR);
EZ_DEFINE_AS_POD_TYPE(VkPresentModeKHR);
EZ_DEFINE_AS_POD_TYPE(VkAccelerationStructureCreateGeometryTypeInfoKHR);
EZ_DEFINE_AS_POD_TYPE(VkPipelineShaderStageCreateInfo);
EZ_DEFINE_AS_POD_TYPE(VkRayTracingShaderGroupCreateInfoKHR);
EZ_DEFINE_AS_POD_TYPE(VkDescriptorUpdateTemplateEntry);
EZ_DEFINE_AS_POD_TYPE(VkPushConstantRange);
EZ_DEFINE_AS_POD_TYPE(VkAccelerationStructureGeometryKHR);
EZ_DEFINE_AS_POD_TYPE(VkAccelerationStructureBuildOffsetInfoKHR);
EZ_DEFINE_AS_POD_TYPE(VkImageViewType);
EZ_DEFINE_AS_POD_TYPE(VkVertexInputBindingDescription);
EZ_DEFINE_AS_POD_TYPE(VkVertexInputAttributeDescription);
EZ_DEFINE_AS_POD_TYPE(VkDeviceQueueCreateInfo);
EZ_DEFINE_AS_POD_TYPE(VkBufferImageCopy);

class GraphicsDevice_Vulkan;
struct QueueFamilyIndices;



namespace Vulkan_Internal
{

  static const size_t timestamp_query_count = 1024;
  static const size_t occlusion_query_count = 1024;

  static PFN_vkCreateRayTracingPipelinesKHR createRayTracingPipelinesKHR;
  static PFN_vkCreateAccelerationStructureKHR createAccelerationStructureKHR;
  static PFN_vkBindAccelerationStructureMemoryKHR bindAccelerationStructureMemoryKHR;
  static PFN_vkDestroyAccelerationStructureKHR destroyAccelerationStructureKHR;
  static PFN_vkGetAccelerationStructureMemoryRequirementsKHR getAccelerationStructureMemoryRequirementsKHR;
  static PFN_vkGetAccelerationStructureDeviceAddressKHR getAccelerationStructureDeviceAddressKHR;
  static PFN_vkGetRayTracingShaderGroupHandlesKHR getRayTracingShaderGroupHandlesKHR;
  static PFN_vkCmdBuildAccelerationStructureKHR cmdBuildAccelerationStructureKHR;
  static PFN_vkCmdTraceRaysKHR cmdTraceRaysKHR;

  static PFN_vkCmdDrawMeshTasksNV cmdDrawMeshTasksNV;
  static PFN_vkCmdDrawMeshTasksIndirectNV cmdDrawMeshTasksIndirectNV;

  struct VulkanAllocationHandler
  {
    VmaAllocator allocator = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkInstance instance;
    ezUInt64 framecount = 0;
    std::mutex destroylocker;
    std::deque<std::pair<std::pair<VkImage, VmaAllocation>, ezUInt64>> destroyer_images;
    std::deque<std::pair<VkImageView, ezUInt64>> destroyer_imageviews;
    std::deque<std::pair<std::pair<VkBuffer, VmaAllocation>, ezUInt64>> destroyer_buffers;
    std::deque<std::pair<VkBufferView, ezUInt64>> destroyer_bufferviews;
    std::deque<std::pair<VkAccelerationStructureKHR, ezUInt64>> destroyer_bvhs;
    std::deque<std::pair<VkSampler, ezUInt64>> destroyer_samplers;
    std::deque<std::pair<VkDescriptorPool, ezUInt64>> destroyer_descriptorPools;
    std::deque<std::pair<VkDescriptorSetLayout, ezUInt64>> destroyer_descriptorSetLayouts;
    std::deque<std::pair<VkDescriptorUpdateTemplate, ezUInt64>> destroyer_descriptorUpdateTemplates;
    std::deque<std::pair<VkShaderModule, ezUInt64>> destroyer_shadermodules;
    std::deque<std::pair<VkPipelineLayout, ezUInt64>> destroyer_pipelineLayouts;
    std::deque<std::pair<VkPipeline, ezUInt64>> destroyer_pipelines;
    std::deque<std::pair<VkRenderPass, ezUInt64>> destroyer_renderpasses;
    std::deque<std::pair<VkFramebuffer, ezUInt64>> destroyer_framebuffers;
    std::deque<std::pair<ezUInt32, ezUInt64>> destroyer_queries_occlusion;
    std::deque<std::pair<ezUInt32, ezUInt64>> destroyer_queries_timestamp;

    ThreadSafeRingBuffer<ezUInt32, timestamp_query_count> free_timestampqueries;
    ThreadSafeRingBuffer<ezUInt32, occlusion_query_count> free_occlusionqueries;

    ~VulkanAllocationHandler()
    {
      Update(~0, 0); // destroy all remaining
      vmaDestroyAllocator(allocator);
      vkDestroyDevice(device, nullptr);
      vkDestroyInstance(instance, nullptr);
    }

    // Deferred destroy of resources that the GPU is already finished with:
    void Update(ezUInt64 FRAMECOUNT, ezUInt32 BACKBUFFER_COUNT)
    {
      destroylocker.lock();
      framecount = FRAMECOUNT;
      while (!destroyer_images.empty())
      {
        if (destroyer_images.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_images.front();
          destroyer_images.pop_front();
          vmaDestroyImage(allocator, item.first.first, item.first.second);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_imageviews.empty())
      {
        if (destroyer_imageviews.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_imageviews.front();
          destroyer_imageviews.pop_front();
          vkDestroyImageView(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_buffers.empty())
      {
        if (destroyer_buffers.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_buffers.front();
          destroyer_buffers.pop_front();
          vmaDestroyBuffer(allocator, item.first.first, item.first.second);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_bufferviews.empty())
      {
        if (destroyer_bufferviews.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_bufferviews.front();
          destroyer_bufferviews.pop_front();
          vkDestroyBufferView(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_bvhs.empty())
      {
        if (destroyer_bvhs.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_bvhs.front();
          destroyer_bvhs.pop_front();
          destroyAccelerationStructureKHR(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_samplers.empty())
      {
        if (destroyer_samplers.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_samplers.front();
          destroyer_samplers.pop_front();
          vkDestroySampler(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_descriptorPools.empty())
      {
        if (destroyer_descriptorPools.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_descriptorPools.front();
          destroyer_descriptorPools.pop_front();
          vkDestroyDescriptorPool(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_descriptorSetLayouts.empty())
      {
        if (destroyer_descriptorSetLayouts.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_descriptorSetLayouts.front();
          destroyer_descriptorSetLayouts.pop_front();
          vkDestroyDescriptorSetLayout(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_descriptorUpdateTemplates.empty())
      {
        if (destroyer_descriptorUpdateTemplates.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_descriptorUpdateTemplates.front();
          destroyer_descriptorUpdateTemplates.pop_front();
          vkDestroyDescriptorUpdateTemplate(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_shadermodules.empty())
      {
        if (destroyer_shadermodules.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_shadermodules.front();
          destroyer_shadermodules.pop_front();
          vkDestroyShaderModule(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_pipelineLayouts.empty())
      {
        if (destroyer_pipelineLayouts.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_pipelineLayouts.front();
          destroyer_pipelineLayouts.pop_front();
          vkDestroyPipelineLayout(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_pipelines.empty())
      {
        if (destroyer_pipelines.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_pipelines.front();
          destroyer_pipelines.pop_front();
          vkDestroyPipeline(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_renderpasses.empty())
      {
        if (destroyer_renderpasses.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_renderpasses.front();
          destroyer_renderpasses.pop_front();
          vkDestroyRenderPass(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_framebuffers.empty())
      {
        if (destroyer_framebuffers.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_framebuffers.front();
          destroyer_framebuffers.pop_front();
          vkDestroyFramebuffer(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_queries_occlusion.empty())
      {
        if (destroyer_queries_occlusion.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_queries_occlusion.front();
          destroyer_queries_occlusion.pop_front();
          free_occlusionqueries.push_back(item.first);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_queries_timestamp.empty())
      {
        if (destroyer_queries_timestamp.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_queries_timestamp.front();
          destroyer_queries_timestamp.pop_front();
          free_timestampqueries.push_back(item.first);
        }
        else
        {
          break;
        }
      }
      destroylocker.unlock();
    }
  };



  // Converters:
  VkFormat _ConvertFormat(ezEnum<ezRHIFormat> value);
  VkCompareOp _ConvertComparisonFunc(ezEnum<ezRHIComparisonFunc> value);
  VkBlendFactor _ConvertBlend(ezEnum<ezRHIBlendFactor> value);
  VkBlendOp _ConvertBlendOp(ezEnum<ezRHIBlendOp> value);
  VkSamplerAddressMode _ConvertTextureAddressMode(ezEnum<ezRHITextureAddressMode> value);
  VkStencilOp _ConvertStencilOp(ezEnum<ezRHIStencilOp> value);
  constexpr VkImageLayout _ConvertImageLayout(IMAGE_LAYOUT value);
  VkShaderStageFlags _ConvertStageFlags(ezEnum<ezRHIShaderStage> value);


  VkAccessFlags _ParseImageLayout(IMAGE_LAYOUT value);
  VkAccessFlags _ParseBufferState(BUFFER_STATE value);

  // Extension functions:
  PFN_vkSetDebugUtilsObjectNameEXT setDebugUtilsObjectNameEXT = nullptr;
  PFN_vkCmdBeginDebugUtilsLabelEXT cmdBeginDebugUtilsLabelEXT = nullptr;
  PFN_vkCmdEndDebugUtilsLabelEXT cmdEndDebugUtilsLabelEXT = nullptr;
  PFN_vkCmdInsertDebugUtilsLabelEXT cmdInsertDebugUtilsLabelEXT = nullptr;

  bool checkDeviceExtensionSupport(const char* checkExtension,
    const ezDynamicArray<VkExtensionProperties>& available_deviceExtensions);

  // Validation layer helpers:
  const ezDynamicArray<const char*> getValidationLayers();

  bool checkValidationLayerSupport();

  VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    ezUInt64 obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData);
  VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger);
  void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);

  VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);
  void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);

  // Queue families:
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

  // Swapchain helpers:
  struct SwapChainSupportDetails
  {
    VkSurfaceCapabilitiesKHR capabilities;
    ezDynamicArray<VkSurfaceFormatKHR> formats;
    ezDynamicArray<VkPresentModeKHR> presentModes;
  };
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

  ezUInt32 findMemoryType(VkPhysicalDevice device, ezUInt32 typeFilter, VkMemoryPropertyFlags properties);

  // Device selection helpers:
  const ezDynamicArray<const char*> getRequiredDeviceExtensions();

  bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);



  // Memory tools:

  inline size_t Align(size_t uLocation, size_t uAlign)
  {
    if ((0 == uAlign) || (uAlign & (uAlign - 1)))
    {
      EZ_ASSERT_ALWAYS(false, "");
    }

    return ((uLocation + (uAlign - 1)) & ~(uAlign - 1));
  }


  // Destroyers:
  struct Buffer_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    VmaAllocation allocation = nullptr;
    VkBuffer resource = VK_NULL_HANDLE;
    VkBufferView cbv = VK_NULL_HANDLE;
    VkBufferView srv = VK_NULL_HANDLE;
    VkBufferView uav = VK_NULL_HANDLE;
    ezDynamicArray<VkBufferView> subresources_srv;
    ezDynamicArray<VkBufferView> subresources_uav;

    GraphicsDevice::GPUAllocation dynamic[COMMANDLIST_COUNT];

    ~Buffer_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_buffers.push_back(std::make_pair(std::make_pair(resource, allocation), framecount));
      if (cbv)
        allocationhandler->destroyer_bufferviews.push_back(std::make_pair(cbv, framecount));
      if (srv)
        allocationhandler->destroyer_bufferviews.push_back(std::make_pair(srv, framecount));
      if (uav)
        allocationhandler->destroyer_bufferviews.push_back(std::make_pair(uav, framecount));
      for (auto x : subresources_srv)
      {
        allocationhandler->destroyer_bufferviews.push_back(std::make_pair(x, framecount));
      }
      for (auto x : subresources_uav)
      {
        allocationhandler->destroyer_bufferviews.push_back(std::make_pair(x, framecount));
      }
      allocationhandler->destroylocker.unlock();
    }
  };
  struct Texture_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    VmaAllocation allocation = nullptr;
    VkImage resource = VK_NULL_HANDLE;
    VkBuffer staging_resource = VK_NULL_HANDLE;
    VkImageView srv = VK_NULL_HANDLE;
    VkImageView uav = VK_NULL_HANDLE;
    VkImageView rtv = VK_NULL_HANDLE;
    VkImageView dsv = VK_NULL_HANDLE;
    ezDynamicArray<VkImageView> subresources_srv;
    ezDynamicArray<VkImageView> subresources_uav;
    ezDynamicArray<VkImageView> subresources_rtv;
    ezDynamicArray<VkImageView> subresources_dsv;

    VkSubresourceLayout subresourcelayout = {};

    ~Texture_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_images.push_back(std::make_pair(std::make_pair(resource, allocation), framecount));
      if (staging_resource)
        allocationhandler->destroyer_buffers.push_back(std::make_pair(std::make_pair(staging_resource, allocation), framecount));
      if (srv)
        allocationhandler->destroyer_imageviews.push_back(std::make_pair(srv, framecount));
      if (uav)
        allocationhandler->destroyer_imageviews.push_back(std::make_pair(uav, framecount));
      if (srv)
        allocationhandler->destroyer_imageviews.push_back(std::make_pair(rtv, framecount));
      if (uav)
        allocationhandler->destroyer_imageviews.push_back(std::make_pair(dsv, framecount));
      for (auto x : subresources_srv)
      {
        allocationhandler->destroyer_imageviews.push_back(std::make_pair(x, framecount));
      }
      for (auto x : subresources_uav)
      {
        allocationhandler->destroyer_imageviews.push_back(std::make_pair(x, framecount));
      }
      for (auto x : subresources_rtv)
      {
        allocationhandler->destroyer_imageviews.push_back(std::make_pair(x, framecount));
      }
      for (auto x : subresources_dsv)
      {
        allocationhandler->destroyer_imageviews.push_back(std::make_pair(x, framecount));
      }
      allocationhandler->destroylocker.unlock();
    }
  };
  struct Sampler_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    VkSampler resource = VK_NULL_HANDLE;

    ~Sampler_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (resource)
        allocationhandler->destroyer_samplers.push_back(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct Query_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    ezRHIGPUQueryType::Enum query_type = ezRHIGPUQueryType::GPU_QUERY_TYPE_INVALID;
    ezUInt32 query_index = ~0;

    ~Query_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      if (query_index != ~0)
      {
        allocationhandler->destroylocker.lock();
        ezUInt64 framecount = allocationhandler->framecount;
        switch (query_type)
        {
          case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION:
          case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
            allocationhandler->destroyer_queries_occlusion.push_back(std::make_pair(query_index, framecount));
            break;
          case ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP:
            allocationhandler->destroyer_queries_timestamp.push_back(std::make_pair(query_index, framecount));
            break;
        }
        allocationhandler->destroylocker.unlock();
      }
    }
  };
  struct Shader_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    VkShaderModule shaderModule = VK_NULL_HANDLE;
    VkPipeline pipeline_cs = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout_cs = VK_NULL_HANDLE;
    VkPipelineShaderStageCreateInfo stageInfo = {};
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    ezDynamicArray<VkDescriptorSetLayoutBinding> layoutBindings;
    ezDynamicArray<VkImageViewType> imageViewTypes;

    ~Shader_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (shaderModule)
        allocationhandler->destroyer_shadermodules.push_back(std::make_pair(shaderModule, framecount));
      if (pipeline_cs)
        allocationhandler->destroyer_pipelines.push_back(std::make_pair(pipeline_cs, framecount));
      if (pipelineLayout_cs)
        allocationhandler->destroyer_pipelineLayouts.push_back(std::make_pair(pipelineLayout_cs, framecount));
      if (descriptorSetLayout)
        allocationhandler->destroyer_descriptorSetLayouts.push_back(std::make_pair(descriptorSetLayout, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct PipelineState_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    ezDynamicArray<VkDescriptorSetLayoutBinding> layoutBindings;
    ezDynamicArray<VkImageViewType> imageViewTypes;

    ~PipelineState_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (pipelineLayout)
        allocationhandler->destroyer_pipelineLayouts.push_back(std::make_pair(pipelineLayout, framecount));
      if (descriptorSetLayout)
        allocationhandler->destroyer_descriptorSetLayouts.push_back(std::make_pair(descriptorSetLayout, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct RenderPass_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    VkRenderPass renderpass = VK_NULL_HANDLE;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    VkRenderPassBeginInfo beginInfo;
    VkClearValue clearColors[9] = {};

    ~RenderPass_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (renderpass)
        allocationhandler->destroyer_renderpasses.push_back(std::make_pair(renderpass, framecount));
      if (framebuffer)
        allocationhandler->destroyer_framebuffers.push_back(std::make_pair(framebuffer, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct BVH_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    VmaAllocation allocation = nullptr;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkAccelerationStructureKHR resource = VK_NULL_HANDLE;

    VkAccelerationStructureCreateInfoKHR info = {};
    ezDynamicArray<VkAccelerationStructureCreateGeometryTypeInfoKHR> geometries;
    VkDeviceSize scratch_offset = 0;
    VkDeviceAddress as_address = 0;

    ~BVH_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (buffer)
        allocationhandler->destroyer_buffers.push_back(std::make_pair(std::make_pair(buffer, allocation), framecount));
      if (resource)
        allocationhandler->destroyer_bvhs.push_back(std::make_pair(resource, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct RTPipelineState_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    VkPipeline pipeline;

    ~RTPipelineState_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (pipeline)
        allocationhandler->destroyer_pipelines.push_back(std::make_pair(pipeline, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct DescriptorTable_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    VkDescriptorUpdateTemplate updatetemplate = VK_NULL_HANDLE;

    ezDynamicArray<size_t> resource_write_remap;
    ezDynamicArray<size_t> sampler_write_remap;

    struct Descriptor
    {
      EZ_DECLARE_POD_TYPE();
      union
      {
        VkDescriptorImageInfo imageinfo;
        VkDescriptorBufferInfo bufferInfo;
        VkBufferView bufferView;
        VkAccelerationStructureKHR accelerationStructure;
      };
    };
    ezDynamicArray<Descriptor> descriptors;

    ~DescriptorTable_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (layout)
        allocationhandler->destroyer_descriptorSetLayouts.push_back(std::make_pair(layout, framecount));
      if (updatetemplate)
        allocationhandler->destroyer_descriptorUpdateTemplates.push_back(std::make_pair(updatetemplate, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };
  struct RootSignature_Vulkan
  {
    std::shared_ptr<VulkanAllocationHandler> allocationhandler;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    bool dirty[COMMANDLIST_COUNT] = {};
    ezDynamicArray<const DescriptorTable*> last_tables[COMMANDLIST_COUNT];
    ezDynamicArray<VkDescriptorSet> last_descriptorsets[COMMANDLIST_COUNT];
    ezDynamicArray<const GPUBuffer*> root_descriptors[COMMANDLIST_COUNT];
    ezDynamicArray<ezUInt32> root_offsets[COMMANDLIST_COUNT];

    struct RootRemap
    {
      ezUInt32 space = 0;
      ezUInt32 binding = 0;
      ezUInt32 rangeIndex = 0;
    };
    ezDynamicArray<RootRemap> root_remap;

    ~RootSignature_Vulkan()
    {
      if (allocationhandler == nullptr)
        return;
      allocationhandler->destroylocker.lock();
      ezUInt64 framecount = allocationhandler->framecount;
      if (pipelineLayout)
        allocationhandler->destroyer_pipelineLayouts.push_back(std::make_pair(pipelineLayout, framecount));
      allocationhandler->destroylocker.unlock();
    }
  };

  Buffer_Vulkan* to_internal(const GPUBuffer* param)
  {
    return static_cast<Buffer_Vulkan*>(param->internal_state.get());
  }
  Texture_Vulkan* to_internal(const Texture* param)
  {
    return static_cast<Texture_Vulkan*>(param->internal_state.get());
  }
  Sampler_Vulkan* to_internal(const Sampler* param)
  {
    return static_cast<Sampler_Vulkan*>(param->internal_state.get());
  }
  Query_Vulkan* to_internal(const GPUQuery* param)
  {
    return static_cast<Query_Vulkan*>(param->internal_state.get());
  }
  Shader_Vulkan* to_internal(const Shader* param)
  {
    return static_cast<Shader_Vulkan*>(param->internal_state.get());
  }
  PipelineState_Vulkan* to_internal(const PipelineState* param)
  {
    return static_cast<PipelineState_Vulkan*>(param->internal_state.get());
  }
  RenderPass_Vulkan* to_internal(const RenderPass* param)
  {
    return static_cast<RenderPass_Vulkan*>(param->internal_state.get());
  }
  BVH_Vulkan* to_internal(const RaytracingAccelerationStructure* param)
  {
    return static_cast<BVH_Vulkan*>(param->internal_state.get());
  }
  RTPipelineState_Vulkan* to_internal(const RaytracingPipelineState* param)
  {
    return static_cast<RTPipelineState_Vulkan*>(param->internal_state.get());
  }
  DescriptorTable_Vulkan* to_internal(const DescriptorTable* param)
  {
    return static_cast<DescriptorTable_Vulkan*>(param->internal_state.get());
  }
  RootSignature_Vulkan* to_internal(const RootSignature* param)
  {
    return static_cast<RootSignature_Vulkan*>(param->internal_state.get());
  }
} // namespace Vulkan_Internal
