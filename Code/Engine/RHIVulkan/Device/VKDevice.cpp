#include <RHIVulkanPCH.h>

#include <RHI/Shader/ShaderBase.h>
#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/BindingSet/VKBindingSet.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/CommandList/VKCommandList.h>
#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>
#include <RHIVulkan/Framebuffer/VKFramebuffer.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUBindlessDescriptorPoolTyped.h>
#include <RHIVulkan/Memory/VKMemory.h>
#include <RHIVulkan/Pipeline/VKComputePipeline.h>
#include <RHIVulkan/Pipeline/VKGraphicsPipeline.h>
#include <RHIVulkan/Pipeline/VKRayTracingPipeline.h>
#include <RHIVulkan/Program/VKProgram.h>
#include <RHIVulkan/QueryHeap/VKQueryHeap.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/SwapChain/VKSwapChain.h>
#include <RHIVulkan/View/VKView.h>

static vk::IndexType GetVkIndexType(ezRHIResourceFormat::Enum format)
{
  auto vkFormat = VKUtilities::ToVKFormat(format);
  switch (vkFormat)
  {
    case vk::Format::eR16Uint:
      return vk::IndexType::eUint16;
    case vk::Format::eR32Uint:
      return vk::IndexType::eUint32;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      return {};
  }
}

vk::BuildAccelerationStructureFlagsKHR Convert(ezBitflags<ezRHIBuildAccelerationStructureFlags> flags)
{
  vk::BuildAccelerationStructureFlagsKHR vk_flags = {};
  if (flags.IsSet(ezRHIBuildAccelerationStructureFlags::AllowUpdate))
    vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::eAllowUpdate;
  if (flags.IsSet(ezRHIBuildAccelerationStructureFlags::AllowCompaction))
    vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction;
  if (flags.IsSet(ezRHIBuildAccelerationStructureFlags::PreferFastTrace))
    vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace;
  if (flags.IsSet(ezRHIBuildAccelerationStructureFlags::PreferFastBuild))
    vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastBuild;
  if (flags.IsSet(ezRHIBuildAccelerationStructureFlags::MinimizeMemory))
    vk_flags |= vk::BuildAccelerationStructureFlagBitsKHR::eLowMemory;
  return vk_flags;
}

vk::AccelerationStructureTypeKHR Convert(ezRHIAccelerationStructureType type)
{
  switch (type)
  {
    case ezRHIAccelerationStructureType::TopLevel:
      return vk::AccelerationStructureTypeKHR::eTopLevel;
    case ezRHIAccelerationStructureType::BottomLevel:
      return vk::AccelerationStructureTypeKHR::eBottomLevel;
  }
  EZ_ASSERT_NOT_IMPLEMENTED;
  return {};
}

vk::ImageLayout ConvertState(ezBitflags<ezRHIResourceState> state)
{
  static ezMap<ezRHIResourceState::Enum, vk::ImageLayout> mapping;
  if (mapping.IsEmpty())
  {
    mapping.Insert(ezRHIResourceState::Common, vk::ImageLayout::eGeneral);
    mapping.Insert(ezRHIResourceState::RenderTarget, vk::ImageLayout::eColorAttachmentOptimal);
    mapping.Insert(ezRHIResourceState::UnorderedAccess, vk::ImageLayout::eGeneral);
    mapping.Insert(ezRHIResourceState::DepthStencilWrite, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    mapping.Insert(ezRHIResourceState::DepthStencilRead, vk::ImageLayout::eDepthStencilReadOnlyOptimal);
    mapping.Insert(ezRHIResourceState::NonPixelShaderResource, vk::ImageLayout::eShaderReadOnlyOptimal);
    mapping.Insert(ezRHIResourceState::PixelShaderResource, vk::ImageLayout::eShaderReadOnlyOptimal);
    mapping.Insert(ezRHIResourceState::CopyDest, vk::ImageLayout::eTransferDstOptimal);
    mapping.Insert(ezRHIResourceState::CopySource, vk::ImageLayout::eTransferSrcOptimal);
    mapping.Insert(ezRHIResourceState::ShadingRateSource, vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR);
    mapping.Insert(ezRHIResourceState::Present, vk::ImageLayout::ePresentSrcKHR);
    mapping.Insert(ezRHIResourceState::kUndefined, vk::ImageLayout::eUndefined);
  }
  for (const auto& m : mapping)
  {
    if (state.IsSet(m.Key()))
    {
      EZ_ASSERT_ALWAYS(state == m.Key(), "");
      return m.Value();
    }
  }
  EZ_ASSERT_ALWAYS(false, "");
  return vk::ImageLayout::eGeneral;
}

ezRHIVKDevice::ezRHIVKDevice(ezRHIVKAdapter& adapter)
  : m_Adapter{adapter}
  , m_PhysicalDevice{adapter.GetPhysicalDevice()}
  , m_GpuDescriptorPool{*this}
{
  m_Capabilities.m_TextureDataPitchAlignment = 1;

  auto queueFamilies = m_PhysicalDevice.getQueueFamilyProperties();
  auto hasAllBits = [](auto flags, auto bits) {
    return (flags & bits) == bits;
  };
  auto hasAnyBits = [](auto flags, auto bits) {
    return flags & bits;
  };
  for (ezUInt32 i = 0; i < (ezUInt32)queueFamilies.size(); ++i)
  {
    const vk::QueueFamilyProperties& queue = queueFamilies[i];
    if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer))
    {
      m_QueuesInfo[ezRHICommandListType::Graphics].m_QueueFamilyIndex = i;
      m_QueuesInfo[ezRHICommandListType::Graphics].m_QueueCount = queue.queueCount;
    }
    else if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer) && !hasAnyBits(queue.queueFlags, vk::QueueFlagBits::eGraphics))
    {
      m_QueuesInfo[ezRHICommandListType::Compute].m_QueueFamilyIndex = i;
      m_QueuesInfo[ezRHICommandListType::Compute].m_QueueCount = queue.queueCount;
    }
    else if (queue.queueCount > 0 && hasAllBits(queue.queueFlags, vk::QueueFlagBits::eTransfer) && !hasAnyBits(queue.queueFlags, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute))
    {
      m_QueuesInfo[ezRHICommandListType::Copy].m_QueueFamilyIndex = i;
      m_QueuesInfo[ezRHICommandListType::Copy].m_QueueCount = queue.queueCount;
    }
  }

  auto extensions = m_PhysicalDevice.enumerateDeviceExtensionProperties();
  ezSet<ezString> reqExtension;

  reqExtension.Insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_RAY_QUERY_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
  reqExtension.Insert(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
  reqExtension.Insert(VK_NV_MESH_SHADER_EXTENSION_NAME);
  reqExtension.Insert(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);


  ezDynamicArray<const char*> foundExtension;
  for (const auto& extension : extensions)
  {
    if (reqExtension.Contains(extension.extensionName.data()))
      foundExtension.PushBack(extension.extensionName);

    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME))
    {
      m_Capabilities.m_IsVariableRateShadingSupported = true;
    }

    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
    {
      m_Capabilities.m_IsDxrSupported = true;
    }

    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_NV_MESH_SHADER_EXTENSION_NAME))
    {
      m_Capabilities.m_IsMeshShadingSupported = true;
    }

    if (ezStringUtils::IsEqual(extension.extensionName.data(), VK_KHR_RAY_QUERY_EXTENSION_NAME))
    {
      m_Capabilities.m_IsRayQuerySupported = true;
    }
  }

  void* deviceCreateInfoNext = nullptr;
  auto addExtension = [&](auto& extension) {
    extension.pNext = deviceCreateInfoNext;
    deviceCreateInfoNext = &extension;
  };

  if (m_Capabilities.m_IsVariableRateShadingSupported)
  {
    ezMap<ezRHIShadingRate, vk::Extent2D> shadingRatePalette;

    shadingRatePalette.Insert(ezRHIShadingRate::ShadingRate1x1, vk::Extent2D{1, 1});
    shadingRatePalette.Insert(ezRHIShadingRate::ShadingRate1x2, vk::Extent2D{1, 2});
    shadingRatePalette.Insert(ezRHIShadingRate::ShadingRate2x1, vk::Extent2D{2, 1});
    shadingRatePalette.Insert(ezRHIShadingRate::ShadingRate2x2, vk::Extent2D{2, 2});
    shadingRatePalette.Insert(ezRHIShadingRate::ShadingRate2x4, vk::Extent2D{2, 4});
    shadingRatePalette.Insert(ezRHIShadingRate::ShadingRate4x2, vk::Extent2D{4, 2});
    shadingRatePalette.Insert(ezRHIShadingRate::ShadingRate4x4, vk::Extent2D{4, 4});

    decltype(auto) fragmentShadingRates = m_Adapter.GetPhysicalDevice().getFragmentShadingRatesKHR();
    for (const auto& fragmentShadingRate : fragmentShadingRates)
    {
      vk::Extent2D size = fragmentShadingRate.fragmentSize;
      ezUInt8 shadingRate = ((size.width >> 1) << 2) | (size.height >> 1);

      EZ_ASSERT_ALWAYS((1 << ((shadingRate >> 2) & 3)) == size.width, "");
      EZ_ASSERT_ALWAYS((1 << (shadingRate & 3)) == size.height, "");
      EZ_ASSERT_ALWAYS(*shadingRatePalette.GetValue((ezRHIShadingRate)shadingRate) == size, "");

      shadingRatePalette.Remove((ezRHIShadingRate)shadingRate);
    }
    EZ_ASSERT_ALWAYS(shadingRatePalette.IsEmpty(), "");

    vk::PhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateImageProperties = {};
    vk::PhysicalDeviceProperties2 deviceProps2 = {};
    deviceProps2.pNext = &shadingRateImageProperties;
    m_Adapter.GetPhysicalDevice().getProperties2(&deviceProps2);
    EZ_ASSERT_ALWAYS(shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize == shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize, "");
    EZ_ASSERT_ALWAYS(shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize.width == shadingRateImageProperties.minFragmentShadingRateAttachmentTexelSize.height, "");
    EZ_ASSERT_ALWAYS(shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.width == shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.height, "");
    m_Capabilities.m_ShadingRateImageTileSize = shadingRateImageProperties.maxFragmentShadingRateAttachmentTexelSize.width;

    vk::PhysicalDeviceFragmentShadingRateFeaturesKHR fragmentShadingRateFeatures = {};
    fragmentShadingRateFeatures.attachmentFragmentShadingRate = true;
    addExtension(fragmentShadingRateFeatures);
  }

  if (m_Capabilities.m_IsDxrSupported)
  {
    vk::PhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingProperties = {};
    vk::PhysicalDeviceProperties2 deviceProps2 = {};
    deviceProps2.pNext = &rayTracingProperties;
    m_PhysicalDevice.getProperties2(&deviceProps2);
    m_Capabilities.m_ShaderGroupHandleSize = rayTracingProperties.shaderGroupHandleSize;
    m_Capabilities.m_ShaderRecordAlignment = rayTracingProperties.shaderGroupHandleSize;
    m_Capabilities.m_ShaderTableAlignment = rayTracingProperties.shaderGroupBaseAlignment;
  }

  const float queuePriority = 1.0f;
  ezDynamicArray<vk::DeviceQueueCreateInfo> queuesCreateInfo;
  for (const auto& queueInfo : m_QueuesInfo)
  {
    vk::DeviceQueueCreateInfo& queueCreateInfo = queuesCreateInfo.ExpandAndGetRef();
    queueCreateInfo.queueFamilyIndex = queueInfo.Value().m_QueueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
  }

  vk::PhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.textureCompressionBC = true;
  deviceFeatures.vertexPipelineStoresAndAtomics = true;
  deviceFeatures.samplerAnisotropy = true;
  deviceFeatures.fragmentStoresAndAtomics = true;
  deviceFeatures.sampleRateShading = true;
  deviceFeatures.geometryShader = true;
  deviceFeatures.imageCubeArray = true;
  deviceFeatures.shaderImageGatherExtended = true;

  vk::PhysicalDeviceVulkan12Features deviceVulkan12Features = {};
  deviceVulkan12Features.drawIndirectCount = true;
  deviceVulkan12Features.bufferDeviceAddress = true;
  deviceVulkan12Features.timelineSemaphore = true;
  deviceVulkan12Features.runtimeDescriptorArray = true;
  deviceVulkan12Features.descriptorBindingVariableDescriptorCount = true;
  addExtension(deviceVulkan12Features);

  vk::PhysicalDeviceMeshShaderFeaturesNV meshShaderFeature = {};
  meshShaderFeature.taskShader = true;
  meshShaderFeature.meshShader = true;
  if (m_Capabilities.m_IsMeshShadingSupported)
  {
    addExtension(meshShaderFeature);
  }

  vk::PhysicalDeviceRayTracingPipelineFeaturesKHR raytracingPipelineFeature = {};
  raytracingPipelineFeature.rayTracingPipeline = true;

  vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeature = {};
  accelerationStructureFeature.accelerationStructure = true;

  vk::PhysicalDeviceRayQueryFeaturesKHR rayqueryPipelineFeature = {};
  rayqueryPipelineFeature.rayQuery = true;

  if (m_Capabilities.m_IsDxrSupported)
  {
    addExtension(raytracingPipelineFeature);
    addExtension(accelerationStructureFeature);

    if (m_Capabilities.m_IsRayQuerySupported)
    {
      raytracingPipelineFeature.rayTraversalPrimitiveCulling = true;
      addExtension(rayqueryPipelineFeature);
    }
  }

  vk::DeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.pNext = deviceCreateInfoNext;
  deviceCreateInfo.queueCreateInfoCount = queuesCreateInfo.GetCount();
  deviceCreateInfo.pQueueCreateInfos = queuesCreateInfo.GetData();
  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
  deviceCreateInfo.enabledExtensionCount = foundExtension.GetCount();
  deviceCreateInfo.ppEnabledExtensionNames = foundExtension.GetData();

  m_Device = m_PhysicalDevice.createDeviceUnique(deviceCreateInfo);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device.get());

  for (auto& queueInfo : m_QueuesInfo)
  {
    vk::CommandPoolCreateInfo cmdPoolCreateInfo = {};
    cmdPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    cmdPoolCreateInfo.queueFamilyIndex = queueInfo.Value().m_QueueFamilyIndex;
    m_CmdPools.Insert(queueInfo.Key(), m_Device->createCommandPoolUnique(cmdPoolCreateInfo));
    m_CommandQueues[queueInfo.Key()] = EZ_DEFAULT_NEW(ezRHIVKCommandQueue, *this, queueInfo.Key(), queueInfo.Value().m_QueueFamilyIndex);
  }
}

ezRHIRaytracingASPrebuildInfo ezRHIVKDevice::GetBLASPrebuildInfo(const ezDynamicArray<ezRHIRaytracingGeometryDescription>& descs, ezBitflags<ezRHIBuildAccelerationStructureFlags> flags) const
{
  ezDynamicArray<vk::AccelerationStructureGeometryKHR> geometryDescs;
  ezDynamicArray<ezUInt32> maxPrimitiveCounts;
  for (const auto& desc : descs)
  {
    geometryDescs.PushBack(FillRaytracingGeometryTriangles(desc.m_Vertex, desc.m_Index, desc.m_Flags));
    if (desc.m_Index.m_Resource)
      maxPrimitiveCounts.PushBack(desc.m_Index.m_Count / 3);
    else
      maxPrimitiveCounts.PushBack(desc.m_Vertex.m_Count / 3);
  }
  vk::AccelerationStructureBuildGeometryInfoKHR accelerationStructureInfo = {};
  accelerationStructureInfo.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
  accelerationStructureInfo.geometryCount = geometryDescs.GetCount();
  accelerationStructureInfo.pGeometries = geometryDescs.GetData();
  accelerationStructureInfo.flags = Convert(flags);
  return GetAccelerationStructurePrebuildInfo(accelerationStructureInfo, maxPrimitiveCounts);
}

ezRHIRaytracingASPrebuildInfo ezRHIVKDevice::GetTLASPrebuildInfo(ezUInt32 instanceCount, ezBitflags<ezRHIBuildAccelerationStructureFlags> flags) const
{
  vk::AccelerationStructureGeometryKHR geometry_info{};
  geometry_info.geometryType = vk::GeometryTypeKHR::eInstances;
  geometry_info.geometry.setInstances({});

  vk::AccelerationStructureBuildGeometryInfoKHR acceleration_structure_info = {};
  acceleration_structure_info.type = vk::AccelerationStructureTypeKHR::eTopLevel;
  acceleration_structure_info.pGeometries = &geometry_info;
  acceleration_structure_info.geometryCount = 1;
  acceleration_structure_info.flags = Convert(flags);

  ezDynamicArray<ezUInt32> maxPrimitiveCounts;
  maxPrimitiveCounts.PushBack(instanceCount);
  return GetAccelerationStructurePrebuildInfo(acceleration_structure_info, maxPrimitiveCounts);
}

ezRHIVKAdapter& ezRHIVKDevice::GetAdapter()
{
  return m_Adapter;
}

vk::Device ezRHIVKDevice::GetDevice()
{
  return m_Device.get();
}

ezRHICommandListType ezRHIVKDevice::GetAvailableCommandListType(ezRHICommandListType type)
{
  if (m_QueuesInfo.Contains(type))
  {
    return type;
  }
  return ezRHICommandListType::Graphics;
}

vk::CommandPool ezRHIVKDevice::GetCmdPool(ezRHICommandListType type)
{
  return (*m_CmdPools.GetValue(GetAvailableCommandListType(type))).get();
}

vk::ImageAspectFlags ezRHIVKDevice::GetAspectFlags(vk::Format format) const
{
  switch (format)
  {
    case vk::Format::eD32SfloatS8Uint:
    case vk::Format::eD24UnormS8Uint:
    case vk::Format::eD16UnormS8Uint:
      return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    case vk::Format::eD16Unorm:
    case vk::Format::eD32Sfloat:
    case vk::Format::eX8D24UnormPack32:
      return vk::ImageAspectFlagBits::eDepth;
    case vk::Format::eS8Uint:
      return vk::ImageAspectFlagBits::eStencil;
    default:
      return vk::ImageAspectFlagBits::eColor;
  }
}

ezRHIVKGPUBindlessDescriptorPoolTyped& ezRHIVKDevice::GetGPUBindlessDescriptorPool(vk::DescriptorType type)
{
  //auto it = m_GpuBindlessDescriptorPool.Find(type);
  //if (it == end(m_GpuBindlessDescriptorPool))
  //{
  //  auto x = ezRHIVKGPUBindlessDescriptorPoolTyped(*this, type);

  //  it = m_GpuBindlessDescriptorPool.Insert(std::forward_as_tuple(type), std::forward_as_tuple(*this, type)); // todo: verify
  //}
  //return it.Value();

  auto it = m_GpuBindlessDescriptorPool.find(type);
  if (it == m_GpuBindlessDescriptorPool.end())
  {
    it = m_GpuBindlessDescriptorPool.emplace(std::piecewise_construct, std::forward_as_tuple(type), std::forward_as_tuple(*this, type)).first;
  }
  return it->second;
}

ezRHIVKGPUDescriptorPool& ezRHIVKDevice::GetGPUDescriptorPool()
{
  return m_GpuDescriptorPool;
}

ezUInt32 ezRHIVKDevice::FindMemoryType(ezUInt32 type_filter, vk::MemoryPropertyFlags properties)
{
  vk::PhysicalDeviceMemoryProperties memProperties;
  m_PhysicalDevice.getMemoryProperties(&memProperties);

  for (ezUInt32 i = 0; i < memProperties.memoryTypeCount; ++i)
  {
    if ((type_filter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
      return i;
  }
  EZ_REPORT_FAILURE("failed to find suitable memory type!");
  return {};
}

vk::AccelerationStructureGeometryKHR ezRHIVKDevice::FillRaytracingGeometryTriangles(const ezRHIBufferDescription& vertex, const ezRHIBufferDescription& index, ezRHIRaytracingGeometryFlags flags) const
{
  vk::AccelerationStructureGeometryKHR geometryDesc = {};
  geometryDesc.geometryType = vk::GeometryTypeNV::eTriangles;
  switch (flags)
  {
    case ezRHIRaytracingGeometryFlags::Opaque:
      geometryDesc.flags = vk::GeometryFlagBitsKHR::eOpaque;
      break;
    case ezRHIRaytracingGeometryFlags::NoDuplicateAnyHitInvocation:
      geometryDesc.flags = vk::GeometryFlagBitsKHR::eNoDuplicateAnyHitInvocation;
      break;
  }

  const auto vkVertexResource = static_cast<ezRHIVKResource*>(vertex.m_Resource);
  const auto vkIndexResource = static_cast<ezRHIVKResource*>(index.m_Resource);

  auto vertexStride = ezRHIResourceFormat::GetFormatStride(vertex.m_Format);
  geometryDesc.geometry.triangles.vertexData = m_Device->getBufferAddress({vkVertexResource->m_Buffer.m_Buffer.get()}) + vertex.m_Offset * vertexStride;
  geometryDesc.geometry.triangles.vertexStride = vertexStride;
  geometryDesc.geometry.triangles.vertexFormat = VKUtilities::ToVKFormat(vertex.m_Format);
  geometryDesc.geometry.triangles.maxVertex = vertex.m_Count;
  if (vkIndexResource)
  {
    auto indexStride = ezRHIResourceFormat::GetFormatStride(index.m_Format);
    geometryDesc.geometry.triangles.indexData = m_Device->getBufferAddress({vkIndexResource->m_Buffer.m_Buffer.get()}) + index.m_Offset * indexStride;
    geometryDesc.geometry.triangles.indexType = GetVkIndexType(index.m_Format);
  }
  else
  {
    geometryDesc.geometry.triangles.indexType = vk::IndexType::eNoneNV;
  }

  return geometryDesc;
}

ezResult ezRHIVKDevice::InitializePlatform()
{
  return EZ_SUCCESS;
}

ezResult ezRHIVKDevice::DestroyPlatform()
{
  return EZ_SUCCESS;
}

ezRHIMemory* ezRHIVKDevice::AllocateMemoryPlatform(ezUInt32 size, ezRHIMemoryType memoryType, ezUInt32 memoryTypeBits)
{
  return EZ_DEFAULT_NEW(ezRHIVKMemory, *this, size, memoryType, memoryTypeBits, nullptr);
}

ezRHICommandQueue* ezRHIVKDevice::GetCommandQueuePlatform(ezRHICommandListType type)
{
  return *m_CommandQueues.GetValue(GetAvailableCommandListType(type));
}

ezRHISwapChain* ezRHIVKDevice::CreateSwapchainPlatform(const ezRHISwapChainCreationDescription& desc)
{
  return EZ_DEFAULT_NEW(ezRHIVKSwapchain, **m_CommandQueues.GetValue(ezRHICommandListType::Graphics), desc);
}

ezRHICommandList* ezRHIVKDevice::CreateCommandListPlatform(ezRHICommandListType type)
{
  return EZ_DEFAULT_NEW(ezRHIVKCommandList, *this, type);
}

ezRHIFence* ezRHIVKDevice::CreateFencePlatform(ezUInt64 initialValue)
{
  return EZ_DEFAULT_NEW(ezRHIVKTimelineSemaphore, *this, initialValue);
}

ezRHIResource* ezRHIVKDevice::CreateTexturePlatform(const ezRHITextureCreationDescription& desc)
{
  ezRHIVKResource* res = EZ_DEFAULT_NEW(ezRHIVKResource, *this);
  res->m_Format = desc.m_Format;
  res->m_ResourceType = ezRHIResourceType::Texture;
  res->m_Image.m_Size.height = desc.m_Height;
  res->m_Image.m_Size.width = desc.m_Width;
  res->m_Image.m_Format = VKUtilities::ToVKFormat(desc.m_Format);
  res->m_Image.m_LevelCount = desc.m_MipLevels;
  res->m_Image.m_SampleCount = desc.m_SampleCount;
  res->m_Image.m_ArrayLayers = desc.m_Depth;

  vk::ImageUsageFlags usage = {};
  if (desc.m_BindFlag & ezRHIBindFlag::DepthStencil)
    usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferDst;
  if (desc.m_BindFlag & ezRHIBindFlag::ShaderResource)
    usage |= vk::ImageUsageFlagBits::eSampled;
  if (desc.m_BindFlag & ezRHIBindFlag::RenderTarget)
    usage |= vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
  if (desc.m_BindFlag & ezRHIBindFlag::UnorderedAccess)
    usage |= vk::ImageUsageFlagBits::eStorage;
  if (desc.m_BindFlag & ezRHIBindFlag::CopyDest)
    usage |= vk::ImageUsageFlagBits::eTransferDst;
  if (desc.m_BindFlag & ezRHIBindFlag::CopySource)
    usage |= vk::ImageUsageFlagBits::eTransferSrc;
  if (desc.m_BindFlag & ezRHIBindFlag::ShadingRateSource)
    usage |= vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR;

  vk::ImageCreateInfo imageInfo = {};
  switch (desc.m_Type)
  {
    case ezRHITextureType::Texture1D:
      imageInfo.imageType = vk::ImageType::e1D;
      break;
    case ezRHITextureType::Texture2D:
      imageInfo.imageType = vk::ImageType::e2D;
      break;
    case ezRHITextureType::Texture3D:
      imageInfo.imageType = vk::ImageType::e3D;
      break;
  }
  imageInfo.extent.width = desc.m_Width;
  imageInfo.extent.height = desc.m_Height;
  if (desc.m_Type == ezRHITextureType::Texture3D)
    imageInfo.extent.depth = desc.m_Depth;
  else
    imageInfo.extent.depth = 1;
  imageInfo.mipLevels = desc.m_MipLevels;
  if (desc.m_Type == ezRHITextureType::Texture3D)
    imageInfo.arrayLayers = 1;
  else
    imageInfo.arrayLayers = desc.m_Depth;
  imageInfo.format = res->m_Image.m_Format;
  imageInfo.tiling = vk::ImageTiling::eOptimal;
  imageInfo.initialLayout = vk::ImageLayout::eUndefined;
  imageInfo.usage = usage;
  imageInfo.samples = static_cast<vk::SampleCountFlagBits>(desc.m_SampleCount);
  imageInfo.sharingMode = vk::SharingMode::eExclusive;

  if (imageInfo.arrayLayers % 6 == 0)
    imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;

  res->m_Image.m_ImageOwner = m_Device->createImageUnique(imageInfo);
  res->m_Image.m_Image = res->m_Image.m_ImageOwner.get();

  res->SetInitialState(ezRHIResourceState::kUndefined);

  return res;
}

ezRHIResource* ezRHIVKDevice::CreateBufferPlatform(const ezRHIBufferCreationDescription& desc)
{
  if (desc.m_BufferSize == 0)
    return {};

  ezRHIVKResource* res = EZ_DEFAULT_NEW(ezRHIVKResource, *this);
  res->m_ResourceType = ezRHIResourceType::Buffer;
  res->m_Buffer.m_Size = desc.m_BufferSize;

  vk::BufferCreateInfo bufferInfo = {};
  bufferInfo.size = desc.m_BufferSize;
  bufferInfo.usage = vk::BufferUsageFlagBits::eShaderDeviceAddress;

  if (desc.m_BindFlag.IsSet(ezRHIBindFlag::VertexBuffer))
    bufferInfo.usage |= vk::BufferUsageFlagBits::eVertexBuffer;
  if (desc.m_BindFlag.IsSet(ezRHIBindFlag::IndexBuffer))
    bufferInfo.usage |= vk::BufferUsageFlagBits::eIndexBuffer;
  if (desc.m_BindFlag.IsSet( ezRHIBindFlag::ConstantBuffer))
    bufferInfo.usage |= vk::BufferUsageFlagBits::eUniformBuffer;
  if (desc.m_BindFlag.IsSet( ezRHIBindFlag::UnorderedAccess))
  {
    bufferInfo.usage |= vk::BufferUsageFlagBits::eStorageBuffer;
    bufferInfo.usage |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
  }
  if (desc.m_BindFlag.IsSet( ezRHIBindFlag::ShaderResource))
  {
    bufferInfo.usage |= vk::BufferUsageFlagBits::eStorageBuffer;
    bufferInfo.usage |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
  }
  if (desc.m_BindFlag.IsSet( ezRHIBindFlag::AccelerationStructure))
    bufferInfo.usage |= vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR;
  if (desc.m_BindFlag.IsSet( ezRHIBindFlag::CopySource))
    bufferInfo.usage |= vk::BufferUsageFlagBits::eTransferSrc;
  if (desc.m_BindFlag.IsSet( ezRHIBindFlag::CopyDest))
    bufferInfo.usage |= vk::BufferUsageFlagBits::eTransferDst;
  if (desc.m_BindFlag.IsSet( ezRHIBindFlag::ShaderTable))
    bufferInfo.usage |= vk::BufferUsageFlagBits::eShaderBindingTableKHR;
  if (desc.m_BindFlag.IsSet( ezRHIBindFlag::IndirectBuffer))
    bufferInfo.usage |= vk::BufferUsageFlagBits::eIndirectBuffer;

  res->m_Buffer.m_Buffer = m_Device->createBufferUnique(bufferInfo);
  res->SetInitialState(ezRHIResourceState::Common);

  return res;
}

ezRHIResource* ezRHIVKDevice::CreateSamplerPlatform(const ezRHISamplerCreationDescription& desc)
{
  ezRHIVKResource* res = EZ_DEFAULT_NEW(ezRHIVKResource, *this);

  vk::SamplerCreateInfo samplerInfo = {};
  samplerInfo.magFilter = vk::Filter::eLinear;
  samplerInfo.minFilter = vk::Filter::eLinear;
  samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
  samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
  samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
  samplerInfo.anisotropyEnable = true;
  samplerInfo.maxAnisotropy = 16;
  samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = vk::CompareOp::eAlways;
  samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
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

  switch (desc.m_Mode)
  {
    case ezRHISamplerTextureAddressMode::Wrap:
      samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
      samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
      samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
      break;
    case ezRHISamplerTextureAddressMode::Clamp:
      samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
      samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
      samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
      break;
  }

  switch (desc.m_ComparisonFunc)
  {
    case ezRHISamplerComparisonFunc::Never:
      samplerInfo.compareOp = vk::CompareOp::eNever;
      break;
    case ezRHISamplerComparisonFunc::Always:
      samplerInfo.compareEnable = true;
      samplerInfo.compareOp = vk::CompareOp::eAlways;
      break;
    case ezRHISamplerComparisonFunc::Less:
      samplerInfo.compareEnable = true;
      samplerInfo.compareOp = vk::CompareOp::eLess;
      break;
  }

  res->m_Sampler.m_Sampler = m_Device->createSamplerUnique(samplerInfo);

  res->m_ResourceType = ezRHIResourceType::Sampler;
  return res;
}

ezRHIView* ezRHIVKDevice::CreateViewPlatform(ezRHIResource* pResource, const ezRHIViewDesc& desc)
{
  return EZ_DEFAULT_NEW(ezRHIVKView, *this, static_cast<ezRHIVKResource*>(pResource), desc);
}

ezRHIBindingSetLayout* ezRHIVKDevice::CreateBindingSetLayoutPlatform(const ezDynamicArray<ezRHIBindKeyDescription>& descs)
{
  return EZ_DEFAULT_NEW(ezRHIVKBindingSetLayout, *this, descs);
}

ezRHIBindingSet* ezRHIVKDevice::CreateBindingSetPlatform(ezRHIBindingSetLayout* pLayout)
{
  return EZ_DEFAULT_NEW(ezRHIVKBindingSet, *this, static_cast<ezRHIVKBindingSetLayout*>(pLayout));
}

ezRHIRenderPass* ezRHIVKDevice::CreateRenderPassPlatform(const ezRHIRenderPassCreationDescription& desc)
{
  return EZ_DEFAULT_NEW(ezRHIVKRenderPass, *this, desc);
}

ezRHIFramebuffer* ezRHIVKDevice::CreateFramebufferPlatform(const ezRHIFramebufferCreationDescription& desc)
{
  return EZ_DEFAULT_NEW(ezRHIVKFramebuffer, *this, desc);
}

ezRHIShader* ezRHIVKDevice::CreateShaderPlatform(const ezRHIShaderCreationDescription& desc)
{
  return EZ_DEFAULT_NEW(ezRHIShaderBase, desc, ezRHIShaderBlobType::SPIRV);
}

ezRHIProgram* ezRHIVKDevice::CreateProgramPlatform(const ezDynamicArray<ezRHIShader*>& shaders)
{
  return EZ_DEFAULT_NEW(ezRHIVKProgram, *this, shaders);
}

ezRHIPipeline* ezRHIVKDevice::CreateGraphicsPipelinePlatform(const ezRHIGraphicsPipelineCreationDescription& desc)
{
  return EZ_DEFAULT_NEW(ezRHIVKGraphicsPipeline, *this, desc);
}

ezRHIPipeline* ezRHIVKDevice::CreateComputePipelinePlatform(const ezRHIComputePipelineCreationDescription& desc)
{
  return EZ_DEFAULT_NEW(ezRHIVKComputePipeline, *this, desc);
}

ezRHIPipeline* ezRHIVKDevice::CreateRayTracingPipelinePlatform(const ezRHIRayTracingPipelineCreationDescription& desc)
{
  return EZ_DEFAULT_NEW(ezRHIVKRayTracingPipeline, *this, desc);
}

ezRHIResource* ezRHIVKDevice::CreateAccelerationStructurePlatform(ezRHIAccelerationStructureType type, ezRHIResource* pResource, ezUInt64 offset)
{
  ezRHIVKResource* res = EZ_DEFAULT_NEW(ezRHIVKResource, *this);
  res->m_ResourceType = ezRHIResourceType::AccelerationStructure;
  res->m_AccelerationStructuresMemory = pResource;

  vk::AccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {};
  accelerationStructureCreateInfo.buffer = pResource->As<ezRHIVKResource>()->m_Buffer.m_Buffer.get();
  accelerationStructureCreateInfo.offset = offset;
  accelerationStructureCreateInfo.size = 0;
  accelerationStructureCreateInfo.type = Convert(type);
  res->m_AccelerationStructureHandle = m_Device->createAccelerationStructureKHRUnique(accelerationStructureCreateInfo);

  return res;
}

ezRHIQueryHeap* ezRHIVKDevice::CreateQueryHeapPlatform(ezRHIQueryHeapType type, ezUInt32 count)
{
  return EZ_DEFAULT_NEW(ezRHIVKQueryHeap, *this, type, count);
}

ezRHIRaytracingASPrebuildInfo ezRHIVKDevice::GetAccelerationStructurePrebuildInfo(const vk::AccelerationStructureBuildGeometryInfoKHR& accelerationStructureInfo, const ezDynamicArray<ezUInt32>& maxPrimitiveCounts) const
{
  vk::AccelerationStructureBuildSizesInfoKHR sizeInfo = {};
  m_Device->getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice, &accelerationStructureInfo, maxPrimitiveCounts.GetData(), &sizeInfo);
  ezRHIRaytracingASPrebuildInfo prebuildInfo = {};
  prebuildInfo.m_AccelerationStructureSize = sizeInfo.accelerationStructureSize;
  prebuildInfo.m_BuildScratchDataSize = sizeInfo.buildScratchSize;
  prebuildInfo.m_UpdateScratchDataSize = sizeInfo.updateScratchSize;
  return prebuildInfo;
}
