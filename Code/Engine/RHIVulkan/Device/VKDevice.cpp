#include <RHIVulkan/RHIVulkanPCH.h>

#include <RHI/Shader/ShaderBase.h>
#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/BindingSet/VKBindingSet.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/CommandList/VKCommandList.h>
#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>
#include <RHIVulkan/Framebuffer/VKFramebuffer.h>
#include <RHIVulkan/Instance/VKInstance.h>
#include <RHIVulkan/Memory/VKMemory.h>
#include <RHIVulkan/Pipeline/VKComputePipeline.h>
#include <RHIVulkan/Pipeline/VKGraphicsPipeline.h>
#include <RHIVulkan/Pipeline/VKRayTracingPipeline.h>
#include <RHIVulkan/Program/VKProgram.h>
#include <RHIVulkan/QueryHeap/VKQueryHeap.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <RHIVulkan/Swapchain/VKSwapchain.h>
#include <RHIVulkan/Utilities/VKUtility.h>
#include <RHIVulkan/View/VKView.h>
#include <set>

bool operator==(const VkExtent2D& lhs, const VkExtent2D& rhs)
{
  return lhs.width == rhs.width && lhs.height == rhs.height;
}

static VkIndexType GetVkIndexType(ezRHIResourceFormat::Enum format)
{
  VkFormat vk_format = VKUtils::ToVkFormat(format);
  switch (vk_format)
  {
    case VkFormat::VK_FORMAT_R16_UINT:
      return VkIndexType::VK_INDEX_TYPE_UINT16;
    case VkFormat::VK_FORMAT_R32_UINT:
      return VkIndexType::VK_INDEX_TYPE_UINT32;
    default:
      assert(false);
      return {};
  }
}

VkImageLayout ConvertState(ResourceState state)
{
  static std::pair<ResourceState, VkImageLayout> mapping[] = {
    {ResourceState::kCommon, VkImageLayout::VK_IMAGE_LAYOUT_GENERAL},
    {ResourceState::kRenderTarget, VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
    {ResourceState::kUnorderedAccess, VkImageLayout::VK_IMAGE_LAYOUT_GENERAL},
    {ResourceState::kDepthStencilWrite, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL},
    {ResourceState::kDepthStencilRead, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL},
    {ResourceState::kNonPixelShaderResource, VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
    {ResourceState::kPixelShaderResource, VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
    {ResourceState::kCopyDest, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL},
    {ResourceState::kCopySource, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL},
    {ResourceState::kShadingRateSource, VkImageLayout::VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR},
    {ResourceState::kPresent, VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
    {ResourceState::kUndefined, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED},
  };
  for (const auto& m : mapping)
  {
    if (state & m.first)
    {
      assert(state == m.first);
      return m.second;
    }
  }
  assert(false);
  return VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;
}

VKDevice::VKDevice(VKAdapter& adapter)
  : m_adapter(adapter)
  , m_physical_device(adapter.GetPhysicalDevice())
  , m_gpu_descriptor_pool(*this)
{
  ezUInt32 queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queueFamilyCount, nullptr);

  ezDynamicArray<VkQueueFamilyProperties> queue_families;
  queue_families.SetCount(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queueFamilyCount, queue_families.GetData());

  auto has_all_bits = [](auto flags, auto bits) {
    return (flags & bits) == bits;
  };
  auto has_any_bits = [](auto flags, auto bits) {
    return flags & bits;
  };
  for (ezUInt32 i = 0; i < (ezUInt32)queue_families.GetCount(); ++i)
  {
    const auto& queue = queue_families[i];
    if (queue.queueCount > 0 && has_all_bits(queue.queueFlags, VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT | VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT | VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT))
    {
      m_queues_info[CommandListType::kGraphics].queue_family_index = i;
      m_queues_info[CommandListType::kGraphics].queue_count = queue.queueCount;
    }
    else if (queue.queueCount > 0 && has_all_bits(queue.queueFlags, VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT | VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT) && !has_any_bits(queue.queueFlags, VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT))
    {
      m_queues_info[CommandListType::kCompute].queue_family_index = i;
      m_queues_info[CommandListType::kCompute].queue_count = queue.queueCount;
    }
    else if (queue.queueCount > 0 && has_all_bits(queue.queueFlags, VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT) && !has_any_bits(queue.queueFlags, VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT | VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT))
    {
      m_queues_info[CommandListType::kCopy].queue_family_index = i;
      m_queues_info[CommandListType::kCopy].queue_count = queue.queueCount;
    }
  }

  ezUInt32 extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extensionCount, nullptr);

  ezDynamicArray<VkExtensionProperties> extensions;
  extensions.SetCount(extensionCount);
  vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extensionCount, extensions.GetData());

  std::set<std::string> req_extension = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
    VK_KHR_RAY_QUERY_EXTENSION_NAME,
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
    VK_KHR_MAINTENANCE3_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
    VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
    VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
    VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
    VK_NV_MESH_SHADER_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
  };

  std::vector<const char*> found_extension;
  for (const auto& extension : extensions)
  {
    if (req_extension.count(extension.extensionName))
      found_extension.push_back(extension.extensionName);

    if (std::string(extension.extensionName) == VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)
      m_is_variable_rate_shading_supported = true;
    if (std::string(extension.extensionName) == VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)
      m_is_dxr_supported = true;
    if (std::string(extension.extensionName) == VK_NV_MESH_SHADER_EXTENSION_NAME)
      m_is_mesh_shading_supported = true;
    if (std::string(extension.extensionName) == VK_KHR_RAY_QUERY_EXTENSION_NAME)
      m_is_ray_query_supported = true;
  }

  void* device_create_info_next = nullptr;
  auto add_extension = [&](auto& extension) {
    extension.pNext = device_create_info_next;
    device_create_info_next = &extension;
  };

  if (m_is_variable_rate_shading_supported)
  {
    std::map<ShadingRate, VkExtent2D> shading_rate_palette = {
      {ShadingRate::k1x1, {1, 1}},
      {ShadingRate::k1x2, {1, 2}},
      {ShadingRate::k2x1, {2, 1}},
      {ShadingRate::k2x2, {2, 2}},
      {ShadingRate::k2x4, {2, 4}},
      {ShadingRate::k4x2, {4, 2}},
      {ShadingRate::k4x4, {4, 4}},
    };

    ezUInt32 fragmentShadingRatesCount = 0;
    vkGetPhysicalDeviceFragmentShadingRatesKHR(m_adapter.GetPhysicalDevice(), &fragmentShadingRatesCount, nullptr);

    ezDynamicArray<VkPhysicalDeviceFragmentShadingRateKHR> fragment_shading_rates;
    fragment_shading_rates.SetCount(fragmentShadingRatesCount);
    vkGetPhysicalDeviceFragmentShadingRatesKHR(m_adapter.GetPhysicalDevice(), &fragmentShadingRatesCount, fragment_shading_rates.GetData());

    for (const auto& fragment_shading_rate : fragment_shading_rates)
    {
      VkExtent2D size = fragment_shading_rate.fragmentSize;
      uint8_t shading_rate = ((size.width >> 1) << 2) | (size.height >> 1);
      assert((1 << ((shading_rate >> 2) & 3)) == size.width);
      assert((1 << (shading_rate & 3)) == size.height);
      assert(shading_rate_palette.at((ShadingRate)shading_rate) == size);
      shading_rate_palette.erase((ShadingRate)shading_rate);
    }
    assert(shading_rate_palette.empty());

    VkPhysicalDeviceFragmentShadingRatePropertiesKHR shading_rate_image_properties = {};
    VkPhysicalDeviceProperties2 device_props2 = {};
    device_props2.pNext = &shading_rate_image_properties;
    vkGetPhysicalDeviceProperties2(m_adapter.GetPhysicalDevice(), &device_props2);
    assert(shading_rate_image_properties.minFragmentShadingRateAttachmentTexelSize == shading_rate_image_properties.maxFragmentShadingRateAttachmentTexelSize);
    assert(shading_rate_image_properties.minFragmentShadingRateAttachmentTexelSize.width == shading_rate_image_properties.minFragmentShadingRateAttachmentTexelSize.height);
    assert(shading_rate_image_properties.maxFragmentShadingRateAttachmentTexelSize.width == shading_rate_image_properties.maxFragmentShadingRateAttachmentTexelSize.height);
    m_shading_rate_image_tile_size = shading_rate_image_properties.maxFragmentShadingRateAttachmentTexelSize.width;

    VkPhysicalDeviceFragmentShadingRateFeaturesKHR fragment_shading_rate_features = {};
    fragment_shading_rate_features.attachmentFragmentShadingRate = true;
    add_extension(fragment_shading_rate_features);
  }

  if (m_is_dxr_supported)
  {
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR ray_tracing_properties = {};
    VkPhysicalDeviceProperties2 device_props2 = {};
    device_props2.pNext = &ray_tracing_properties;
    vkGetPhysicalDeviceProperties2(m_physical_device, &device_props2);

    m_shader_group_handle_size = ray_tracing_properties.shaderGroupHandleSize;
    m_shader_record_alignment = ray_tracing_properties.shaderGroupHandleSize;
    m_shader_table_alignment = ray_tracing_properties.shaderGroupBaseAlignment;
  }

  const float queue_priority = 1.0f;
  std::vector<VkDeviceQueueCreateInfo> queues_create_info;
  for (const auto& queue_info : m_queues_info)
  {
    VkDeviceQueueCreateInfo& queue_create_info = queues_create_info.emplace_back();
    queue_create_info.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_info.second.queue_family_index;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
  }

  VkPhysicalDeviceFeatures device_features = {};
  device_features.textureCompressionBC = true;
  device_features.vertexPipelineStoresAndAtomics = true;
  device_features.samplerAnisotropy = true;
  device_features.fragmentStoresAndAtomics = true;
  device_features.sampleRateShading = true;
  device_features.geometryShader = true;
  device_features.imageCubeArray = true;
  device_features.shaderImageGatherExtended = true;

  VkPhysicalDeviceVulkan12Features device_vulkan12_features = {};
  device_vulkan12_features.drawIndirectCount = true;
  device_vulkan12_features.bufferDeviceAddress = true;
  device_vulkan12_features.timelineSemaphore = true;
  device_vulkan12_features.runtimeDescriptorArray = true;
  device_vulkan12_features.descriptorBindingVariableDescriptorCount = true;
  add_extension(device_vulkan12_features);

  VkPhysicalDeviceMeshShaderFeaturesNV mesh_shader_feature = {};
  mesh_shader_feature.taskShader = true;
  mesh_shader_feature.meshShader = true;
  if (m_is_mesh_shading_supported)
  {
    add_extension(mesh_shader_feature);
  }

  VkPhysicalDeviceRayTracingPipelineFeaturesKHR raytracing_pipeline_feature = {};
  raytracing_pipeline_feature.rayTracingPipeline = true;

  VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_feature = {};
  acceleration_structure_feature.accelerationStructure = true;

  VkPhysicalDeviceRayQueryFeaturesKHR rayquery_pipeline_feature = {};
  rayquery_pipeline_feature.rayQuery = true;

  if (m_is_dxr_supported)
  {
    add_extension(raytracing_pipeline_feature);
    add_extension(acceleration_structure_feature);

    if (m_is_ray_query_supported)
    {
      raytracing_pipeline_feature.rayTraversalPrimitiveCulling = true;
      add_extension(rayquery_pipeline_feature);
    }
  }

  VkDeviceCreateInfo device_create_info = {};
  device_create_info.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pNext = device_create_info_next;
  device_create_info.queueCreateInfoCount = (ezUInt32)queues_create_info.size();
  device_create_info.pQueueCreateInfos = queues_create_info.data();
  device_create_info.pEnabledFeatures = &device_features;
  device_create_info.enabledExtensionCount = (ezUInt32)found_extension.size();
  device_create_info.ppEnabledExtensionNames = found_extension.data();

  vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device);

  for (auto& queue_info : m_queues_info)
  {
    VkCommandPoolCreateInfo cmd_pool_create_info = {};
    cmd_pool_create_info.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_create_info.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmd_pool_create_info.queueFamilyIndex = queue_info.second.queue_family_index;

    vkCreateCommandPool(m_device, &cmd_pool_create_info, nullptr, &m_cmd_pools[queue_info.first]);

    m_command_queues[queue_info.first] = std::make_shared<VKCommandQueue>(*this, queue_info.first, queue_info.second.queue_family_index);
  }
}

std::shared_ptr<Memory> VKDevice::AllocateMemory(uint64_t size, MemoryType memory_type, uint32_t memory_type_bits)
{
  return std::make_shared<VKMemory>(*this, size, memory_type, memory_type_bits, nullptr);
}

std::shared_ptr<CommandQueue> VKDevice::GetCommandQueue(CommandListType type)
{
  return m_command_queues.at(GetAvailableCommandListType(type));
}

uint32_t VKDevice::GetTextureDataPitchAlignment() const
{
  return 1;
}

std::shared_ptr<Swapchain> VKDevice::CreateSwapchain(Window window, uint32_t width, uint32_t height, uint32_t frame_count, bool vsync)
{
  return std::make_shared<VKSwapchain>(*m_command_queues.at(CommandListType::kGraphics), window, width, height, frame_count, vsync);
}

std::shared_ptr<CommandList> VKDevice::CreateCommandList(CommandListType type)
{
  return std::make_shared<VKCommandList>(*this, type);
}

std::shared_ptr<Fence> VKDevice::CreateFence(uint64_t initial_value)
{
  return std::make_shared<VKTimelineSemaphore>(*this, initial_value);
}

std::shared_ptr<Resource> VKDevice::CreateTexture(TextureType type, uint32_t bind_flag, ezRHIResourceFormat::Enum format, uint32_t sample_count, int width, int height, int depth, int mip_levels)
{
  std::shared_ptr<VKResource> res = std::make_shared<VKResource>(*this);
  res->format = format;
  res->resource_type = ResourceType::kTexture;
  res->image.size.height = height;
  res->image.size.width = width;
  res->image.format = VKUtils::ToVkFormat(format);
  res->image.level_count = mip_levels;
  res->image.sample_count = sample_count;
  res->image.array_layers = depth;

  VkImageUsageFlags usage = {};
  if (bind_flag & BindFlag::kDepthStencil)
    usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  if (bind_flag & BindFlag::kShaderResource)
    usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
  if (bind_flag & BindFlag::kRenderTarget)
    usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  if (bind_flag & BindFlag::kUnorderedAccess)
    usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;
  if (bind_flag & BindFlag::kCopyDest)
    usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  if (bind_flag & BindFlag::kCopySource)
    usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  if (bind_flag & BindFlag::kShadingRateSource)
    usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

  VkImageCreateInfo image_info = {};
  image_info.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  switch (type)
  {
    case TextureType::k1D:
      image_info.imageType = VkImageType::VK_IMAGE_TYPE_1D;
      break;
    case TextureType::k2D:
      image_info.imageType = VkImageType::VK_IMAGE_TYPE_2D;
      break;
    case TextureType::k3D:
      image_info.imageType = VkImageType::VK_IMAGE_TYPE_3D;
      break;
  }
  image_info.extent.width = width;
  image_info.extent.height = height;
  if (type == TextureType::k3D)
    image_info.extent.depth = depth;
  else
    image_info.extent.depth = 1;
  image_info.mipLevels = mip_levels;
  if (type == TextureType::k3D)
    image_info.arrayLayers = 1;
  else
    image_info.arrayLayers = depth;
  image_info.format = res->image.format;
  image_info.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
  image_info.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.samples = static_cast<VkSampleCountFlagBits>(sample_count);
  image_info.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;

  if (image_info.arrayLayers % 6 == 0)
    image_info.flags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

  vkCreateImage(m_device, &image_info, nullptr, &res->image.res_owner);
  res->image.res = res->image.res_owner;

  res->SetInitialState(ResourceState::kUndefined);

  return res;
}

std::shared_ptr<Resource> VKDevice::CreateBuffer(uint32_t bind_flag, uint32_t buffer_size)
{
  if (buffer_size == 0)
    return {};

  std::shared_ptr<VKResource> res = std::make_shared<VKResource>(*this);
  res->resource_type = ResourceType::kBuffer;
  res->buffer.size = buffer_size;

  VkBufferCreateInfo buffer_info = {};
  buffer_info.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = buffer_size;
  buffer_info.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

  if (bind_flag & BindFlag::kVertexBuffer)
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  if (bind_flag & BindFlag::kIndexBuffer)
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  if (bind_flag & BindFlag::kConstantBuffer)
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  if (bind_flag & BindFlag::kUnorderedAccess)
  {
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
  }
  if (bind_flag & BindFlag::kShaderResource)
  {
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
  }
  if (bind_flag & BindFlag::kAccelerationStructure)
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
  if (bind_flag & BindFlag::kCopySource)
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  if (bind_flag & BindFlag::kCopyDest)
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  if (bind_flag & BindFlag::kShaderTable)
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;
  if (bind_flag & BindFlag::kIndirectBuffer)
    buffer_info.usage |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

  vkCreateBuffer(m_device, &buffer_info, nullptr, &res->buffer.res);

  res->SetInitialState(ResourceState::kCommon);

  return res;
}

std::shared_ptr<Resource> VKDevice::CreateSampler(const SamplerDesc& desc)
{
  std::shared_ptr<VKResource> res = std::make_shared<VKResource>(*this);

  VkSamplerCreateInfo samplerInfo = {};
  samplerInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VkFilter::VK_FILTER_LINEAR;
  samplerInfo.minFilter = VkFilter::VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = true;
  samplerInfo.maxAnisotropy = 16;
  samplerInfo.borderColor = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = ezMath::MaxValue<float>();

  /*switch (desc.filter)
    {
    case SamplerFilter::kAnisotropic:
        sampler_desc.Filter = D3D12_FILTER_ANISOTROPIC;
        break;
    case SamplerFilter::kMinMagMipLinear:
        sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        break;
    case SamplerFilter::kComparisonMinMagMipLinear:
        sampler_desc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        break;
    }*/

  switch (desc.mode)
  {
    case SamplerTextureAddressMode::kWrap:
      samplerInfo.addressModeU = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
      break;
    case SamplerTextureAddressMode::kClamp:
      samplerInfo.addressModeU = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerInfo.addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      samplerInfo.addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      break;
  }

  switch (desc.func)
  {
    case SamplerComparisonFunc::kNever:
      samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_NEVER;
      break;
    case SamplerComparisonFunc::kAlways:
      samplerInfo.compareEnable = true;
      samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
      break;
    case SamplerComparisonFunc::kLess:
      samplerInfo.compareEnable = true;
      samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_LESS;
      break;
  }

  vkCreateSampler(m_device, &samplerInfo, nullptr, &res->sampler.res);

  res->resource_type = ResourceType::kSampler;
  return res;
}

std::shared_ptr<View> VKDevice::CreateView(const std::shared_ptr<Resource>& resource, const ViewDesc& view_desc)
{
  return std::make_shared<VKView>(*this, std::static_pointer_cast<VKResource>(resource), view_desc);
}

std::shared_ptr<BindingSetLayout> VKDevice::CreateBindingSetLayout(const std::vector<BindKey>& descs)
{
  return std::make_shared<VKBindingSetLayout>(*this, descs);
}

std::shared_ptr<BindingSet> VKDevice::CreateBindingSet(const std::shared_ptr<BindingSetLayout>& layout)
{
  return std::make_shared<VKBindingSet>(*this, std::static_pointer_cast<VKBindingSetLayout>(layout));
}

std::shared_ptr<RenderPass> VKDevice::CreateRenderPass(const RenderPassDesc& desc)
{
  return std::make_shared<VKRenderPass>(*this, desc);
}

std::shared_ptr<Framebuffer> VKDevice::CreateFramebuffer(const FramebufferDesc& desc)
{
  return std::make_shared<VKFramebuffer>(*this, desc);
}

std::shared_ptr<Shader> VKDevice::CreateShader(const ShaderDesc& desc, std::vector<uint8_t> byteCode, std::shared_ptr<ShaderReflection> reflection)
{
  return std::make_shared<ShaderBase>(desc, byteCode, reflection, ShaderBlobType::kSPIRV);
}

std::shared_ptr<Program> VKDevice::CreateProgram(const std::vector<std::shared_ptr<Shader>>& shaders)
{
  return std::make_shared<VKProgram>(*this, shaders);
}

std::shared_ptr<Pipeline> VKDevice::CreateGraphicsPipeline(const GraphicsPipelineDesc& desc)
{
  return std::make_shared<VKGraphicsPipeline>(*this, desc);
}

std::shared_ptr<Pipeline> VKDevice::CreateComputePipeline(const ComputePipelineDesc& desc)
{
  return std::make_shared<VKComputePipeline>(*this, desc);
}

std::shared_ptr<Pipeline> VKDevice::CreateRayTracingPipeline(const RayTracingPipelineDesc& desc)
{
  return std::make_shared<VKRayTracingPipeline>(*this, desc);
}

VkAccelerationStructureGeometryKHR VKDevice::FillRaytracingGeometryTriangles(const BufferDesc& vertex, const BufferDesc& index, RaytracingGeometryFlags flags) const
{
  VkAccelerationStructureGeometryKHR geometry_desc = {};
  geometry_desc.geometryType = VkGeometryTypeNV::VK_GEOMETRY_TYPE_TRIANGLES_KHR;
  switch (flags)
  {
    case RaytracingGeometryFlags::kOpaque:
      geometry_desc.flags = VkGeometryFlagBitsKHR::VK_GEOMETRY_OPAQUE_BIT_KHR;
      break;
    case RaytracingGeometryFlags::kNoDuplicateAnyHitInvocation:
      geometry_desc.flags = VkGeometryFlagBitsKHR::VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;
      break;
  }

  auto vk_vertex_res = std::static_pointer_cast<VKResource>(vertex.res);
  auto vk_index_res = std::static_pointer_cast<VKResource>(index.res);

  auto vertex_stride = ezRHIResourceFormat::GetFormatStride(vertex.format);

  VkBufferDeviceAddressInfo vertexBufferInfo;
  vertexBufferInfo.buffer = vk_vertex_res->buffer.res;

  geometry_desc.geometry.triangles.vertexData.deviceAddress = vkGetBufferDeviceAddress(m_device, &vertexBufferInfo) + vertex.offset * vertex_stride;
  geometry_desc.geometry.triangles.vertexStride = vertex_stride;
  geometry_desc.geometry.triangles.vertexFormat = VKUtils::ToVkFormat(vertex.format);
  geometry_desc.geometry.triangles.maxVertex = vertex.count;
  if (vk_index_res)
  {
    VkBufferDeviceAddressInfo indexBufferInfo;
    indexBufferInfo.buffer = vk_index_res->buffer.res;

    auto index_stride = ezRHIResourceFormat::GetFormatStride(index.format);
    geometry_desc.geometry.triangles.indexData.deviceAddress = vkGetBufferDeviceAddress(m_device, &indexBufferInfo) + index.offset * index_stride;
    geometry_desc.geometry.triangles.indexType = GetVkIndexType(index.format);
  }
  else
  {
    geometry_desc.geometry.triangles.indexType = VkIndexType::VK_INDEX_TYPE_NONE_KHR;
  }

  return geometry_desc;
}

RaytracingASPrebuildInfo VKDevice::GetAccelerationStructurePrebuildInfo(const VkAccelerationStructureBuildGeometryInfoKHR& acceleration_structure_info, const std::vector<uint32_t>& max_primitive_counts) const
{
  VkAccelerationStructureBuildSizesInfoKHR size_info = {};
  vkGetAccelerationStructureBuildSizesKHR(m_device, VkAccelerationStructureBuildTypeKHR::VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &acceleration_structure_info, max_primitive_counts.data(), &size_info);
  RaytracingASPrebuildInfo prebuild_info = {};
  prebuild_info.acceleration_structure_size = size_info.accelerationStructureSize;
  prebuild_info.build_scratch_data_size = size_info.buildScratchSize;
  prebuild_info.update_scratch_data_size = size_info.updateScratchSize;
  return prebuild_info;
}

VkBuildAccelerationStructureFlagsKHR Convert(BuildAccelerationStructureFlags flags)
{
  VkBuildAccelerationStructureFlagsKHR vk_flags = {};
  if (flags & BuildAccelerationStructureFlags::kAllowUpdate)
    vk_flags |= VkBuildAccelerationStructureFlagBitsKHR::VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
  if (flags & BuildAccelerationStructureFlags::kAllowCompaction)
    vk_flags |= VkBuildAccelerationStructureFlagBitsKHR::VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;
  if (flags & BuildAccelerationStructureFlags::kPreferFastTrace)
    vk_flags |= VkBuildAccelerationStructureFlagBitsKHR::VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  if (flags & BuildAccelerationStructureFlags::kPreferFastBuild)
    vk_flags |= VkBuildAccelerationStructureFlagBitsKHR::VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
  if (flags & BuildAccelerationStructureFlags::kMinimizeMemory)
    vk_flags |= VkBuildAccelerationStructureFlagBitsKHR::VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR;
  return vk_flags;
}

VkAccelerationStructureTypeKHR Convert(AccelerationStructureType type)
{
  switch (type)
  {
    case AccelerationStructureType::kTopLevel:
      return VkAccelerationStructureTypeKHR::VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    case AccelerationStructureType::kBottomLevel:
      return VkAccelerationStructureTypeKHR::VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
  }
  assert(false);
  return {};
}

std::shared_ptr<Resource> VKDevice::CreateAccelerationStructure(AccelerationStructureType type, const std::shared_ptr<Resource>& resource, uint64_t offset)
{
  std::shared_ptr<VKResource> res = std::make_shared<VKResource>(*this);
  res->resource_type = ResourceType::kAccelerationStructure;
  res->acceleration_structures_memory = resource;

  VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info = {};
  acceleration_structure_create_info.sType = VkStructureType::VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
  acceleration_structure_create_info.buffer = resource->As<VKResource>().buffer.res;
  acceleration_structure_create_info.offset = offset;
  acceleration_structure_create_info.size = 0;
  acceleration_structure_create_info.type = Convert(type);
  vkCreateAccelerationStructureKHR(m_device, &acceleration_structure_create_info, nullptr, &res->acceleration_structure_handle);

  return res;
}

std::shared_ptr<QueryHeap> VKDevice::CreateQueryHeap(QueryHeapType type, uint32_t count)
{
  return std::make_shared<VKQueryHeap>(*this, type, count);
}

bool VKDevice::IsDxrSupported() const
{
  return m_is_dxr_supported;
}

bool VKDevice::IsRayQuerySupported() const
{
  return m_is_ray_query_supported;
}

bool VKDevice::IsVariableRateShadingSupported() const
{
  return m_is_variable_rate_shading_supported;
}

bool VKDevice::IsMeshShadingSupported() const
{
  return m_is_mesh_shading_supported;
}

uint32_t VKDevice::GetShadingRateImageTileSize() const
{
  return m_shading_rate_image_tile_size;
}

MemoryBudget VKDevice::GetMemoryBudget() const
{
  VkPhysicalDeviceMemoryBudgetPropertiesEXT memory_budget = {};
  VkPhysicalDeviceMemoryProperties2 mem_properties = {};
  mem_properties.pNext = &memory_budget;
  vkGetPhysicalDeviceMemoryProperties2(m_adapter.GetPhysicalDevice(), & mem_properties);
  MemoryBudget res = {};
  for (size_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i)
  {
    res.budget += memory_budget.heapBudget[i];
    res.usage += memory_budget.heapUsage[i];
  }
  return res;
}

uint32_t VKDevice::GetShaderGroupHandleSize() const
{
  return m_shader_group_handle_size;
}

uint32_t VKDevice::GetShaderRecordAlignment() const
{
  return m_shader_record_alignment;
}

uint32_t VKDevice::GetShaderTableAlignment() const
{
  return m_shader_table_alignment;
}

RaytracingASPrebuildInfo VKDevice::GetBLASPrebuildInfo(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) const
{
  std::vector<VkAccelerationStructureGeometryKHR> geometry_descs;
  std::vector<uint32_t> max_primitive_counts;
  for (const auto& desc : descs)
  {
    geometry_descs.emplace_back(FillRaytracingGeometryTriangles(desc.vertex, desc.index, desc.flags));
    if (desc.index.res)
      max_primitive_counts.emplace_back(desc.index.count / 3);
    else
      max_primitive_counts.emplace_back(desc.vertex.count / 3);
  }
  VkAccelerationStructureBuildGeometryInfoKHR acceleration_structure_info = {};
  acceleration_structure_info.type = VkAccelerationStructureTypeKHR::VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
  acceleration_structure_info.geometryCount = (ezUInt32)geometry_descs.size();
  acceleration_structure_info.pGeometries = geometry_descs.data();
  acceleration_structure_info.flags = Convert(flags);
  return GetAccelerationStructurePrebuildInfo(acceleration_structure_info, max_primitive_counts);
}

RaytracingASPrebuildInfo VKDevice::GetTLASPrebuildInfo(uint32_t instance_count, BuildAccelerationStructureFlags flags) const
{
  VkAccelerationStructureGeometryKHR geometry_info{};
  geometry_info.geometryType = VkGeometryTypeKHR::VK_GEOMETRY_TYPE_INSTANCES_KHR;


  geometry_info.geometry.instances = {};

  VkAccelerationStructureBuildGeometryInfoKHR acceleration_structure_info = {};
  acceleration_structure_info.type = VkAccelerationStructureTypeKHR::VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
  acceleration_structure_info.pGeometries = &geometry_info;
  acceleration_structure_info.geometryCount = 1;
  acceleration_structure_info.flags = Convert(flags);
  return GetAccelerationStructurePrebuildInfo(acceleration_structure_info, {instance_count});
}

VKAdapter& VKDevice::GetAdapter()
{
  return m_adapter;
}

VkDevice VKDevice::GetDevice()
{
  return m_device;
}

CommandListType VKDevice::GetAvailableCommandListType(CommandListType type)
{
  if (m_queues_info.count(type))
  {
    return type;
  }
  return CommandListType::kGraphics;
}

VkCommandPool VKDevice::GetCmdPool(CommandListType type)
{
  return m_cmd_pools.at(GetAvailableCommandListType(type));
}

VkImageAspectFlags VKDevice::GetAspectFlags(VkFormat format) const
{
  switch (format)
  {
    case VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT:
    case VkFormat::VK_FORMAT_D24_UNORM_S8_UINT:
    case VkFormat::VK_FORMAT_D16_UNORM_S8_UINT:
      return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
    case VkFormat::VK_FORMAT_D16_UNORM:
    case VkFormat::VK_FORMAT_D32_SFLOAT:
    case VkFormat::VK_FORMAT_X8_D24_UNORM_PACK32:
      return VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
    case VkFormat::VK_FORMAT_S8_UINT:
      return VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
    default:
      return VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
  }
}

uint32_t VKDevice::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physical_device, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
  {
    if ((type_filter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
      return i;
  }
  throw std::runtime_error("failed to find suitable memory type!");
}

VKGPUBindlessDescriptorPoolTyped& VKDevice::GetGPUBindlessDescriptorPool(VkDescriptorType type)
{
  auto it = m_gpu_bindless_descriptor_pool.find(type);
  if (it == m_gpu_bindless_descriptor_pool.end())
    it = m_gpu_bindless_descriptor_pool.emplace(std::piecewise_construct, std::forward_as_tuple(type), std::forward_as_tuple(*this, type)).first;
  return it->second;
}

VKGPUDescriptorPool& VKDevice::GetGPUDescriptorPool()
{
  return m_gpu_descriptor_pool;
}
