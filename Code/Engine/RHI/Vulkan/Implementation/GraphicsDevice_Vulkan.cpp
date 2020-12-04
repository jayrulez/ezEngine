#include <RHI/Vulkan/GraphicsDevice_Vulkan.h>

#ifdef EZ_RHI_VULKAN_SUPPORTED

#  pragma comment(lib, "vulkan-1.lib")

#  include <RHI/Vulkan/ThirdParty/spirv-reflect/spirv_reflect.h>

#  include <Foundation/Logging/Log.h>

#  define VMA_IMPLEMENTATION
#  include <RHI/Vulkan/ThirdParty/vk_mem_alloc.h>

#  include <algorithm>
#  include <cstring>
#  include <iostream>
#  include <set>
#  include <sstream>

#  include <RHI/FormatHelpers.h>

// Enabling ray tracing might crash RenderDoc:
#  define ENABLE_RAYTRACING_EXTENSION

// These shifts are made so that Vulkan resource bindings slots don't interfere with each other across shader stages:
//	These are also defined in compile_shaders_spirv.py as shift numbers, and it needs to be synced!
#  define VULKAN_BINDING_SHIFT_B 0
#  define VULKAN_BINDING_SHIFT_T 1000
#  define VULKAN_BINDING_SHIFT_U 2000
#  define VULKAN_BINDING_SHIFT_S 3000



using namespace Vulkan_Internal;

// Allocators:

void GraphicsDevice_Vulkan::FrameResources::ResourceFrameAllocator::init(GraphicsDevice_Vulkan* device, size_t size)
{
  this->device = device;
  auto internal_state = EZ_DEFAULT_NEW(Buffer_Vulkan);
  internal_state->allocationhandler = device->allocationhandler;
  buffer.internal_state = internal_state;

  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  bufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  bufferInfo.flags = 0;

  VkResult res;

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

  res = vmaCreateBuffer(device->allocationhandler->allocator, &bufferInfo, &allocInfo, &internal_state->resource, &internal_state->allocation, nullptr);
  assert(res == VK_SUCCESS);

  void* pData = internal_state->allocation->GetMappedData();
  dataCur = dataBegin = reinterpret_cast<ezUInt8*>(pData);
  dataEnd = dataBegin + size;

  // Because the "buffer" is created by hand in this, fill the desc to indicate how it can be used:
  this->buffer.type = GPUResource::GPU_RESOURCE_TYPE::BUFFER;
  this->buffer.desc.ByteWidth = (ezUInt32)((size_t)dataEnd - (size_t)dataBegin);
  this->buffer.desc.Usage = ezRHIUsage::Dynamic;
  this->buffer.desc.BindFlags = BIND_VERTEX_BUFFER | BIND_INDEX_BUFFER | BIND_SHADER_RESOURCE;
  this->buffer.desc.MiscFlags = RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
}
ezUInt8* GraphicsDevice_Vulkan::FrameResources::ResourceFrameAllocator::allocate(size_t dataSize, size_t alignment)
{
  dataCur = reinterpret_cast<ezUInt8*>(Align(reinterpret_cast<size_t>(dataCur), alignment));

  if (dataCur + dataSize > dataEnd)
  {
    init(device, ((size_t)dataEnd + dataSize - (size_t)dataBegin) * 2);
  }

  ezUInt8* retVal = dataCur;

  dataCur += dataSize;

  return retVal;
}
void GraphicsDevice_Vulkan::FrameResources::ResourceFrameAllocator::clear()
{
  dataCur = dataBegin;
}
ezUInt64 GraphicsDevice_Vulkan::FrameResources::ResourceFrameAllocator::calculateOffset(ezUInt8* address)
{
  assert(address >= dataBegin && address < dataEnd);
  return static_cast<ezUInt64>(address - dataBegin);
}

void GraphicsDevice_Vulkan::FrameResources::DescriptorTableFrameAllocator::init(GraphicsDevice_Vulkan* device)
{
  this->device = device;

  // Important that these don't reallocate themselves during writing dexcriptors!
  descriptorWrites.Reserve(128);
  bufferInfos.Reserve(128);
  imageInfos.Reserve(128);
  texelBufferViews.Reserve(128);
  accelerationStructureViews.Reserve(128);

  VkResult res;

  // Create descriptor pool:
  VkDescriptorPoolSize poolSizes[9] = {};

  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = GPU_RESOURCE_HEAP_CBV_COUNT * poolSize;

  poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
  poolSizes[1].descriptorCount = GPU_RESOURCE_HEAP_SRV_COUNT * poolSize;

  poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
  poolSizes[2].descriptorCount = GPU_RESOURCE_HEAP_SRV_COUNT * poolSize;

  poolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  poolSizes[3].descriptorCount = GPU_RESOURCE_HEAP_SRV_COUNT * poolSize;

  poolSizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  poolSizes[4].descriptorCount = GPU_RESOURCE_HEAP_UAV_COUNT * poolSize;

  poolSizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
  poolSizes[5].descriptorCount = GPU_RESOURCE_HEAP_UAV_COUNT * poolSize;

  poolSizes[6].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  poolSizes[6].descriptorCount = GPU_RESOURCE_HEAP_UAV_COUNT * poolSize;

  poolSizes[7].type = VK_DESCRIPTOR_TYPE_SAMPLER;
  poolSizes[7].descriptorCount = GPU_SAMPLER_HEAP_COUNT * poolSize;

  poolSizes[8].type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
  poolSizes[8].descriptorCount = GPU_RESOURCE_HEAP_SRV_COUNT * poolSize;

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = EZ_ARRAY_SIZE(poolSizes);
  poolInfo.pPoolSizes = poolSizes;
  poolInfo.maxSets = poolSize;
  //poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

  res = vkCreateDescriptorPool(device->device, &poolInfo, nullptr, &descriptorPool);
  assert(res == VK_SUCCESS);

  reset();
}
void GraphicsDevice_Vulkan::FrameResources::DescriptorTableFrameAllocator::destroy()
{
  if (descriptorPool != VK_NULL_HANDLE)
  {
    device->allocationhandler->destroyer_descriptorPools.PushBack(std::make_pair(descriptorPool, device->FRAMECOUNT));
    descriptorPool = VK_NULL_HANDLE;
  }
}
void GraphicsDevice_Vulkan::FrameResources::DescriptorTableFrameAllocator::reset()
{
  dirty = true;

  if (descriptorPool != VK_NULL_HANDLE)
  {
    VkResult res = vkResetDescriptorPool(device->device, descriptorPool, 0);
    assert(res == VK_SUCCESS);
  }

  memset(CBV, 0, sizeof(CBV));
  memset(SRV, 0, sizeof(SRV));
  memset(SRV_index, -1, sizeof(SRV_index));
  memset(UAV, 0, sizeof(UAV));
  memset(UAV_index, -1, sizeof(UAV_index));
  memset(SAM, 0, sizeof(SAM));
}
void GraphicsDevice_Vulkan::FrameResources::DescriptorTableFrameAllocator::validate(bool graphics, CommandList cmd, bool raytracing)
{
  if (!dirty)
    return;
  dirty = true;

  auto pso_internal = graphics ? to_internal(device->active_pso[cmd]) : nullptr;
  auto cs_internal = graphics ? nullptr : to_internal(device->active_cs[cmd]);

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
  if (graphics)
  {
    pipelineLayout = pso_internal->pipelineLayout;
    descriptorSetLayout = pso_internal->descriptorSetLayout;
  }
  else
  {
    pipelineLayout = cs_internal->pipelineLayout_cs;
    descriptorSetLayout = cs_internal->descriptorSetLayout;
  }

  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &descriptorSetLayout;

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
  VkResult res = vkAllocateDescriptorSets(device->device, &allocInfo, &descriptorSet);
  while (res == VK_ERROR_OUT_OF_POOL_MEMORY)
  {
    poolSize *= 2;
    destroy();
    init(device);
    allocInfo.descriptorPool = descriptorPool;
    res = vkAllocateDescriptorSets(device->device, &allocInfo, &descriptorSet);
  }
  assert(res == VK_SUCCESS);

  descriptorWrites.Clear();
  bufferInfos.Clear();
  imageInfos.Clear();
  texelBufferViews.Clear();
  accelerationStructureViews.Clear();

  const auto& layoutBindings = graphics ? pso_internal->layoutBindings : cs_internal->layoutBindings;
  const auto& imageViewTypes = graphics ? pso_internal->imageViewTypes : cs_internal->imageViewTypes;


  int i = 0;
  for (auto& x : layoutBindings)
  {
    auto& write = descriptorWrites.ExpandAndGetRef();
    write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSet;
    write.dstArrayElement = 0;
    write.descriptorType = x.descriptorType;
    write.dstBinding = x.binding;
    write.descriptorCount = 1;

    VkImageViewType viewtype = imageViewTypes[i++];

    switch (x.descriptorType)
    {
      case VK_DESCRIPTOR_TYPE_SAMPLER:
      {
        write.pImageInfo = &imageInfos.ExpandAndGetRef();
        imageInfos.PeekBack() = {};

        const ezUInt32 original_binding = x.binding - VULKAN_BINDING_SHIFT_S;
        const Sampler* sampler = SAM[original_binding];
        if (sampler == nullptr || !sampler->IsValid())
        {
          imageInfos.PeekBack().sampler = device->nullSampler;
        }
        else
        {
          imageInfos.PeekBack().sampler = to_internal(sampler)->resource;
        }
      }
      break;

      case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
      {
        write.pImageInfo = &imageInfos.ExpandAndGetRef();
        imageInfos.PeekBack() = {};
        imageInfos.PeekBack().imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        const ezUInt32 original_binding = x.binding - VULKAN_BINDING_SHIFT_T;
        const GPUResource* resource = SRV[original_binding];
        if (resource == nullptr || !resource->IsValid() || !resource->IsTexture())
        {
          switch (viewtype)
          {
            case VK_IMAGE_VIEW_TYPE_1D:
              imageInfos.PeekBack().imageView = device->nullImageView1D;
              break;
            case VK_IMAGE_VIEW_TYPE_2D:
              imageInfos.PeekBack().imageView = device->nullImageView2D;
              break;
            case VK_IMAGE_VIEW_TYPE_3D:
              imageInfos.PeekBack().imageView = device->nullImageView3D;
              break;
            case VK_IMAGE_VIEW_TYPE_CUBE:
              imageInfos.PeekBack().imageView = device->nullImageViewCube;
              break;
            case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
              imageInfos.PeekBack().imageView = device->nullImageView1DArray;
              break;
            case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
              imageInfos.PeekBack().imageView = device->nullImageView2DArray;
              break;
            case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
              imageInfos.PeekBack().imageView = device->nullImageViewCubeArray;
              break;
            case VK_IMAGE_VIEW_TYPE_MAX_ENUM:
              break;
            default:
              break;
          }
        }
        else
        {
          int subresource = SRV_index[original_binding];
          const Texture* texture = (const Texture*)resource;
          if (subresource >= 0)
          {
            imageInfos.PeekBack().imageView = to_internal(texture)->subresources_srv[subresource];
          }
          else
          {
            imageInfos.PeekBack().imageView = to_internal(texture)->srv;
          }

          VkImageLayout layout = _ConvertImageLayout(texture->desc.layout);
          if (layout != VK_IMAGE_LAYOUT_GENERAL && layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
          {
            // Means texture initial layout is not compatible, so it must have been transitioned
            layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
          }
          imageInfos.PeekBack().imageLayout = layout;
        }
      }
      break;

      case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
      {
        write.pImageInfo = &imageInfos.ExpandAndGetRef();
        imageInfos.PeekBack() = {};
        imageInfos.PeekBack().imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        const ezUInt32 original_binding = x.binding - VULKAN_BINDING_SHIFT_U;
        const GPUResource* resource = UAV[original_binding];
        if (resource == nullptr || !resource->IsValid() || !resource->IsTexture())
        {
          switch (viewtype)
          {
            case VK_IMAGE_VIEW_TYPE_1D:
              imageInfos.PeekBack().imageView = device->nullImageView1D;
              break;
            case VK_IMAGE_VIEW_TYPE_2D:
              imageInfos.PeekBack().imageView = device->nullImageView2D;
              break;
            case VK_IMAGE_VIEW_TYPE_3D:
              imageInfos.PeekBack().imageView = device->nullImageView3D;
              break;
            case VK_IMAGE_VIEW_TYPE_CUBE:
              imageInfos.PeekBack().imageView = device->nullImageViewCube;
              break;
            case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
              imageInfos.PeekBack().imageView = device->nullImageView1DArray;
              break;
            case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
              imageInfos.PeekBack().imageView = device->nullImageView2DArray;
              break;
            case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
              imageInfos.PeekBack().imageView = device->nullImageViewCubeArray;
              break;
            case VK_IMAGE_VIEW_TYPE_MAX_ENUM:
              break;
            default:
              break;
          }
        }
        else
        {
          int subresource = UAV_index[original_binding];
          const Texture* texture = (const Texture*)resource;
          if (subresource >= 0)
          {
            imageInfos.PeekBack().imageView = to_internal(texture)->subresources_uav[subresource];
          }
          else
          {
            imageInfos.PeekBack().imageView = to_internal(texture)->uav;
          }
        }
      }
      break;

      case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      {
        write.pBufferInfo = &bufferInfos.ExpandAndGetRef();
        bufferInfos.PeekBack() = {};

        const ezUInt32 original_binding = x.binding - VULKAN_BINDING_SHIFT_B;
        const GPUBuffer* buffer = CBV[original_binding];
        if (buffer == nullptr || !buffer->IsValid())
        {
          bufferInfos.PeekBack().buffer = device->nullBuffer;
          bufferInfos.PeekBack().range = VK_WHOLE_SIZE;
        }
        else
        {
          auto internal_state = to_internal(buffer);
          if (buffer->desc.Usage == ezRHIUsage::Dynamic)
          {
            const GPUAllocation& allocation = internal_state->dynamic[cmd];
            bufferInfos.PeekBack().buffer = to_internal(allocation.buffer)->resource;
            bufferInfos.PeekBack().offset = allocation.offset;
            bufferInfos.PeekBack().range = buffer->desc.ByteWidth;
          }
          else
          {
            bufferInfos.PeekBack().buffer = internal_state->resource;
            bufferInfos.PeekBack().offset = 0;
            bufferInfos.PeekBack().range = buffer->desc.ByteWidth;
          }
        }
      }
      break;

      case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
      {
        write.pTexelBufferView = &texelBufferViews.ExpandAndGetRef();
        texelBufferViews.PeekBack() = {};

        const ezUInt32 original_binding = x.binding - VULKAN_BINDING_SHIFT_T;
        const GPUResource* resource = SRV[original_binding];
        if (resource == nullptr || !resource->IsValid() || !resource->IsBuffer())
        {
          texelBufferViews.PeekBack() = device->nullBufferView;
        }
        else
        {
          int subresource = SRV_index[original_binding];
          const GPUBuffer* buffer = (const GPUBuffer*)resource;
          if (subresource >= 0)
          {
            texelBufferViews.PeekBack() = to_internal(buffer)->subresources_srv[subresource];
          }
          else
          {
            texelBufferViews.PeekBack() = to_internal(buffer)->srv;
          }
        }
      }
      break;

      case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
      {
        write.pTexelBufferView = &texelBufferViews.ExpandAndGetRef();
        texelBufferViews.PeekBack() = {};

        const ezUInt32 original_binding = x.binding - VULKAN_BINDING_SHIFT_U;
        const GPUResource* resource = UAV[original_binding];
        if (resource == nullptr || !resource->IsValid() || !resource->IsBuffer())
        {
          texelBufferViews.PeekBack() = device->nullBufferView;
        }
        else
        {
          int subresource = UAV_index[original_binding];
          const GPUBuffer* buffer = (const GPUBuffer*)resource;
          if (subresource >= 0)
          {
            texelBufferViews.PeekBack() = to_internal(buffer)->subresources_uav[subresource];
          }
          else
          {
            texelBufferViews.PeekBack() = to_internal(buffer)->uav;
          }
        }
      }
      break;

      case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
      {
        write.pBufferInfo = &bufferInfos.ExpandAndGetRef();
        bufferInfos.PeekBack() = {};

        if (x.binding < VULKAN_BINDING_SHIFT_U)
        {
          // SRV
          const ezUInt32 original_binding = x.binding - VULKAN_BINDING_SHIFT_T;
          const GPUResource* resource = SRV[original_binding];
          if (resource == nullptr || !resource->IsValid() || !resource->IsBuffer())
          {
            bufferInfos.PeekBack().buffer = device->nullBuffer;
            bufferInfos.PeekBack().range = VK_WHOLE_SIZE;
          }
          else
          {
            int subresource = SRV_index[original_binding];
            const GPUBuffer* buffer = (const GPUBuffer*)resource;
            bufferInfos.PeekBack().buffer = to_internal(buffer)->resource;
            bufferInfos.PeekBack().range = buffer->desc.ByteWidth;
          }
        }
        else
        {
          // UAV
          const ezUInt32 original_binding = x.binding - VULKAN_BINDING_SHIFT_U;
          const GPUResource* resource = UAV[original_binding];
          if (resource == nullptr || !resource->IsValid() || !resource->IsBuffer())
          {
            bufferInfos.PeekBack().buffer = device->nullBuffer;
            bufferInfos.PeekBack().range = VK_WHOLE_SIZE;
          }
          else
          {
            int subresource = UAV_index[original_binding];
            const GPUBuffer* buffer = (const GPUBuffer*)resource;
            bufferInfos.PeekBack().buffer = to_internal(buffer)->resource;
            bufferInfos.PeekBack().range = buffer->desc.ByteWidth;
          }
        }
      }
      break;

      case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
      {
        write.pNext = &accelerationStructureViews.ExpandAndGetRef();
        accelerationStructureViews.PeekBack() = {};
        accelerationStructureViews.PeekBack().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
        accelerationStructureViews.PeekBack().accelerationStructureCount = 1;

        const ezUInt32 original_binding = x.binding - VULKAN_BINDING_SHIFT_T;
        const GPUResource* resource = SRV[original_binding];
        if (resource == nullptr || !resource->IsValid() || !resource->IsAccelerationStructure())
        {
          assert(0); // invalid acceleration structure!
        }
        else
        {
          const RaytracingAccelerationStructure* as = (const RaytracingAccelerationStructure*)resource;
          accelerationStructureViews.PeekBack().pAccelerationStructures = &to_internal(as)->resource;
        }
      }
      break;
    }
  }

  vkUpdateDescriptorSets(device->device, descriptorWrites.GetCount(), descriptorWrites.GetData(), 0, nullptr);

  vkCmdBindDescriptorSets(device->GetDirectCommandList(cmd),
    graphics ? VK_PIPELINE_BIND_POINT_GRAPHICS : (raytracing ? VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR : VK_PIPELINE_BIND_POINT_COMPUTE),
    pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}
VkDescriptorSet GraphicsDevice_Vulkan::FrameResources::DescriptorTableFrameAllocator::commit(const DescriptorTable* table)
{
  auto internal_state = to_internal(table);

  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &internal_state->layout;

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
  VkResult res = vkAllocateDescriptorSets(device->device, &allocInfo, &descriptorSet);
  while (res == VK_ERROR_OUT_OF_POOL_MEMORY)
  {
    poolSize *= 2;
    destroy();
    init(device);
    allocInfo.descriptorPool = descriptorPool;
    res = vkAllocateDescriptorSets(device->device, &allocInfo, &descriptorSet);
  }
  assert(res == VK_SUCCESS);

  vkUpdateDescriptorSetWithTemplate(
    device->device,
    descriptorSet,
    internal_state->updatetemplate,
    internal_state->descriptors.GetData());

  return descriptorSet;
}

void GraphicsDevice_Vulkan::pso_validate(CommandList cmd)
{
  if (!dirty_pso[cmd])
    return;

  const PipelineState* pso = active_pso[cmd];
  size_t pipeline_hash = prev_pipeline_hash[cmd];

  VkPipeline pipeline = VK_NULL_HANDLE;
  auto it = pipelines_global.find(pipeline_hash);
  if (it == pipelines_global.end())
  {
    for (auto& x : pipelines_worker[cmd])
    {
      if (pipeline_hash == x.first)
      {
        pipeline = x.second;
        break;
      }
    }

    if (pipeline == VK_NULL_HANDLE)
    {
      VkGraphicsPipelineCreateInfo pipelineInfo = {};
      //pipelineInfo.flags = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
      pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      if (pso->desc.rootSignature == nullptr)
      {
        pipelineInfo.layout = to_internal(pso)->pipelineLayout;
      }
      else
      {
        pipelineInfo.layout = to_internal(pso->desc.rootSignature)->pipelineLayout;
      }
      pipelineInfo.renderPass = active_renderpass[cmd] == nullptr ? defaultRenderPass : to_internal(active_renderpass[cmd])->renderpass;
      pipelineInfo.subpass = 0;
      pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

      // Shaders:

      ezUInt32 shaderStageCount = 0;
      VkPipelineShaderStageCreateInfo shaderStages[ezRHIShaderStage::ENUM_COUNT - 1];
      if (pso->desc.ms != nullptr && pso->desc.ms->IsValid())
      {
        shaderStages[shaderStageCount++] = to_internal(pso->desc.ms)->stageInfo;
      }
      if (pso->desc.as != nullptr && pso->desc.as->IsValid())
      {
        shaderStages[shaderStageCount++] = to_internal(pso->desc.as)->stageInfo;
      }
      if (pso->desc.vs != nullptr && pso->desc.vs->IsValid())
      {
        shaderStages[shaderStageCount++] = to_internal(pso->desc.vs)->stageInfo;
      }
      if (pso->desc.hs != nullptr && pso->desc.hs->IsValid())
      {
        shaderStages[shaderStageCount++] = to_internal(pso->desc.hs)->stageInfo;
      }
      if (pso->desc.ds != nullptr && pso->desc.ds->IsValid())
      {
        shaderStages[shaderStageCount++] = to_internal(pso->desc.ds)->stageInfo;
      }
      if (pso->desc.gs != nullptr && pso->desc.gs->IsValid())
      {
        shaderStages[shaderStageCount++] = to_internal(pso->desc.gs)->stageInfo;
      }
      if (pso->desc.ps != nullptr && pso->desc.ps->IsValid())
      {
        shaderStages[shaderStageCount++] = to_internal(pso->desc.ps)->stageInfo;
      }
      pipelineInfo.stageCount = shaderStageCount;
      pipelineInfo.pStages = shaderStages;


      // Fixed function states:

      // Input layout:
      VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
      vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      ezDynamicArray<VkVertexInputBindingDescription> bindings;
      ezDynamicArray<VkVertexInputAttributeDescription> attributes;
      if (pso->desc.il != nullptr)
      {
        ezUInt32 lastBinding = 0xFFFFFFFF;
        for (auto& x : pso->desc.il->desc)
        {
          VkVertexInputBindingDescription bind = {};
          bind.binding = x.InputSlot;
          bind.inputRate = x.InputSlotClass == ezRHIInputClassification::PerVertexData ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
          bind.stride = x.AlignedByteOffset;
          if (bind.stride == InputLayoutDesc::APPEND_ALIGNED_ELEMENT)
          {
            // need to manually resolve this from the format spec.
            bind.stride = FormatHelpers::GetFormatStride(x.Format);
          }

          if (lastBinding != bind.binding)
          {
            bindings.PushBack(bind);
            lastBinding = bind.binding;
          }
          else
          {
            bindings.PeekBack().stride += bind.stride;
          }
        }

        ezUInt32 offset = 0;
        ezUInt32 i = 0;
        lastBinding = 0xFFFFFFFF;
        for (auto& x : pso->desc.il->desc)
        {
          VkVertexInputAttributeDescription attr = {};
          attr.binding = x.InputSlot;
          if (attr.binding != lastBinding)
          {
            lastBinding = attr.binding;
            offset = 0;
          }
          attr.format = _ConvertFormat(x.Format);
          attr.location = i;
          attr.offset = x.AlignedByteOffset;
          if (attr.offset == InputLayoutDesc::APPEND_ALIGNED_ELEMENT)
          {
            // need to manually resolve this from the format spec.
            attr.offset = offset;
            offset += FormatHelpers::GetFormatStride(x.Format);
          }

          attributes.PushBack(attr);

          i++;
        }

        vertexInputInfo.vertexBindingDescriptionCount = static_cast<ezUInt32>(bindings.GetCount());
        vertexInputInfo.pVertexBindingDescriptions = bindings.GetData();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<ezUInt32>(attributes.GetCount());
        vertexInputInfo.pVertexAttributeDescriptions = attributes.GetData();
      }
      pipelineInfo.pVertexInputState = &vertexInputInfo;

      // Primitive type:
      VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
      inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      switch (pso->desc.pt)
      {
        case ezRHIPrimitiveTopology::PointList:
          inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
          break;
        case ezRHIPrimitiveTopology::LineList:
          inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
          break;
        case ezRHIPrimitiveTopology::LineStrip:
          inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
          break;
        case ezRHIPrimitiveTopology::TriangleStrip:
          inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
          break;
        case ezRHIPrimitiveTopology::TriangleList:
          inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
          break;
        case ezRHIPrimitiveTopology::PatchList:
          inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
          break;
        default:
          break;
      }
      inputAssembly.primitiveRestartEnable = VK_FALSE;

      pipelineInfo.pInputAssemblyState = &inputAssembly;


      // Rasterizer:
      VkPipelineRasterizationStateCreateInfo rasterizer = {};
      rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizer.depthClampEnable = VK_TRUE;
      rasterizer.rasterizerDiscardEnable = VK_FALSE;
      rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
      rasterizer.lineWidth = 1.0f;
      rasterizer.cullMode = VK_CULL_MODE_NONE;
      rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
      rasterizer.depthBiasEnable = VK_FALSE;
      rasterizer.depthBiasConstantFactor = 0.0f;
      rasterizer.depthBiasClamp = 0.0f;
      rasterizer.depthBiasSlopeFactor = 0.0f;

      // depth clip will be enabled via Vulkan 1.1 extension VK_EXT_depth_clip_enable:
      VkPipelineRasterizationDepthClipStateCreateInfoEXT depthclip = {};
      depthclip.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT;
      depthclip.depthClipEnable = VK_TRUE;
      rasterizer.pNext = &depthclip;

      if (pso->desc.rs != nullptr)
      {
        const RasterizerStateDesc& desc = pso->desc.rs->desc;

        switch (desc.FillMode)
        {
          case ezRHIFillMode::Wireframe:
            rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
            break;
          case ezRHIFillMode::Solid:
          default:
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            break;
        }

        switch (desc.CullMode)
        {
          case ezRHICullMode::Back:
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            break;
          case ezRHICullMode::Front:
            rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
            break;
          case ezRHICullMode::None:
          default:
            rasterizer.cullMode = VK_CULL_MODE_NONE;
            break;
        }

        rasterizer.frontFace = desc.FrontCounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = desc.DepthBias != 0 || desc.SlopeScaledDepthBias != 0;
        rasterizer.depthBiasConstantFactor = static_cast<float>(desc.DepthBias);
        rasterizer.depthBiasClamp = desc.DepthBiasClamp;
        rasterizer.depthBiasSlopeFactor = desc.SlopeScaledDepthBias;

        // depth clip is extension in Vulkan 1.1:
        depthclip.depthClipEnable = desc.DepthClipEnable ? VK_TRUE : VK_FALSE;
      }

      pipelineInfo.pRasterizationState = &rasterizer;


      // Viewport, Scissor:
      VkViewport viewport = {};
      viewport.x = 0;
      viewport.y = 0;
      viewport.width = 65535;
      viewport.height = 65535;
      viewport.minDepth = 0;
      viewport.maxDepth = 1;

      VkRect2D scissor = {};
      scissor.extent.width = 65535;
      scissor.extent.height = 65535;

      VkPipelineViewportStateCreateInfo viewportState = {};
      viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportState.viewportCount = 1;
      viewportState.pViewports = &viewport;
      viewportState.scissorCount = 1;
      viewportState.pScissors = &scissor;

      pipelineInfo.pViewportState = &viewportState;


      // Depth-Stencil:
      VkPipelineDepthStencilStateCreateInfo depthstencil = {};
      depthstencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      if (pso->desc.dss != nullptr)
      {
        depthstencil.depthTestEnable = pso->desc.dss->desc.DepthEnable ? VK_TRUE : VK_FALSE;
        depthstencil.depthWriteEnable = pso->desc.dss->desc.DepthWriteMask == ezRHIDepthWriteMask::Zero ? VK_FALSE : VK_TRUE;
        depthstencil.depthCompareOp = _ConvertComparisonFunc(pso->desc.dss->desc.DepthFunc);

        depthstencil.stencilTestEnable = pso->desc.dss->desc.StencilEnable ? VK_TRUE : VK_FALSE;

        depthstencil.front.compareMask = pso->desc.dss->desc.StencilReadMask;
        depthstencil.front.writeMask = pso->desc.dss->desc.StencilWriteMask;
        depthstencil.front.reference = 0; // runtime supplied
        depthstencil.front.compareOp = _ConvertComparisonFunc(pso->desc.dss->desc.FrontFace.StencilFunc);
        depthstencil.front.passOp = _ConvertStencilOp(pso->desc.dss->desc.FrontFace.StencilPassOp);
        depthstencil.front.failOp = _ConvertStencilOp(pso->desc.dss->desc.FrontFace.StencilFailOp);
        depthstencil.front.depthFailOp = _ConvertStencilOp(pso->desc.dss->desc.FrontFace.StencilDepthFailOp);

        depthstencil.back.compareMask = pso->desc.dss->desc.StencilReadMask;
        depthstencil.back.writeMask = pso->desc.dss->desc.StencilWriteMask;
        depthstencil.back.reference = 0; // runtime supplied
        depthstencil.back.compareOp = _ConvertComparisonFunc(pso->desc.dss->desc.BackFace.StencilFunc);
        depthstencil.back.passOp = _ConvertStencilOp(pso->desc.dss->desc.BackFace.StencilPassOp);
        depthstencil.back.failOp = _ConvertStencilOp(pso->desc.dss->desc.BackFace.StencilFailOp);
        depthstencil.back.depthFailOp = _ConvertStencilOp(pso->desc.dss->desc.BackFace.StencilDepthFailOp);

        depthstencil.depthBoundsTestEnable = VK_FALSE;
      }

      pipelineInfo.pDepthStencilState = &depthstencil;


      // MSAA:
      VkPipelineMultisampleStateCreateInfo multisampling = {};
      multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multisampling.sampleShadingEnable = VK_FALSE;
      multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      if (active_renderpass[cmd] != nullptr && active_renderpass[cmd]->desc.attachments.GetCount() > 0)
      {
        multisampling.rasterizationSamples = (VkSampleCountFlagBits)active_renderpass[cmd]->desc.attachments[0].texture->desc.SampleCount;
      }
      if (pso->desc.rs != nullptr)
      {
        const RasterizerStateDesc& desc = pso->desc.rs->desc;
        if (desc.ForcedSampleCount > 1)
        {
          multisampling.rasterizationSamples = (VkSampleCountFlagBits)desc.ForcedSampleCount;
        }
      }
      multisampling.minSampleShading = 1.0f;
      VkSampleMask samplemask = pso->desc.sampleMask;
      multisampling.pSampleMask = &samplemask;
      multisampling.alphaToCoverageEnable = VK_FALSE;
      multisampling.alphaToOneEnable = VK_FALSE;

      pipelineInfo.pMultisampleState = &multisampling;


      // Blending:

      // ez new
      BlendStateDesc psoBlendStateDesc = pso->desc.bs != nullptr ? pso->desc.bs->GetDesc() : BlendStateDesc();

      ezUInt32 numBlendAttachments = 0;
      VkPipelineColorBlendAttachmentState colorBlendAttachments[8];
      const size_t blend_loopCount = active_renderpass[cmd] == nullptr ? 1 : active_renderpass[cmd]->desc.attachments.GetCount();
      for (size_t i = 0; i < blend_loopCount; ++i)
      {
        if (active_renderpass[cmd] != nullptr && active_renderpass[cmd]->desc.attachments[i].type != RenderPassAttachment::RENDERTARGET)
        {
          continue;
        }

        // ez new
        RenderTargetBlendStateDesc desc = psoBlendStateDesc.RenderTarget[numBlendAttachments];

        VkPipelineColorBlendAttachmentState& attachment = colorBlendAttachments[numBlendAttachments];
        numBlendAttachments++;

        attachment.blendEnable = desc.BlendEnable ? VK_TRUE : VK_FALSE;

        attachment.colorWriteMask = 0;
        if (desc.RenderTargetWriteMask.IsSet(ezRHIColorWriteMask::Red))
        {
          attachment.colorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
        }
        if (desc.RenderTargetWriteMask.IsSet(ezRHIColorWriteMask::Green))
        {
          attachment.colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
        }
        if (desc.RenderTargetWriteMask.IsSet(ezRHIColorWriteMask::Blue))
        {
          attachment.colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
        }
        if (desc.RenderTargetWriteMask.IsSet(ezRHIColorWriteMask::Alpha))
        {
          attachment.colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
        }

        attachment.srcColorBlendFactor = _ConvertBlend(desc.SrcBlend);
        attachment.dstColorBlendFactor = _ConvertBlend(desc.DestBlend);
        attachment.colorBlendOp = _ConvertBlendOp(desc.BlendOp);
        attachment.srcAlphaBlendFactor = _ConvertBlend(desc.SrcBlendAlpha);
        attachment.dstAlphaBlendFactor = _ConvertBlend(desc.DestBlendAlpha);
        attachment.alphaBlendOp = _ConvertBlendOp(desc.BlendOpAlpha);
      }

      VkPipelineColorBlendStateCreateInfo colorBlending = {};
      colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      colorBlending.logicOpEnable = VK_FALSE;
      colorBlending.logicOp = VK_LOGIC_OP_COPY;
      colorBlending.attachmentCount = numBlendAttachments;
      colorBlending.pAttachments = colorBlendAttachments;
      colorBlending.blendConstants[0] = 1.0f;
      colorBlending.blendConstants[1] = 1.0f;
      colorBlending.blendConstants[2] = 1.0f;
      colorBlending.blendConstants[3] = 1.0f;

      pipelineInfo.pColorBlendState = &colorBlending;


      // Tessellation:
      VkPipelineTessellationStateCreateInfo tessellationInfo = {};
      tessellationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
      tessellationInfo.patchControlPoints = 3;

      pipelineInfo.pTessellationState = &tessellationInfo;



      // Dynamic state will be specified at runtime:
      VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_STENCIL_REFERENCE,
        VK_DYNAMIC_STATE_BLEND_CONSTANTS};

      VkPipelineDynamicStateCreateInfo dynamicState = {};
      dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicState.dynamicStateCount = EZ_ARRAY_SIZE(dynamicStates);
      dynamicState.pDynamicStates = dynamicStates;

      pipelineInfo.pDynamicState = &dynamicState;

      VkResult res = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
      assert(res == VK_SUCCESS);

      pipelines_worker[cmd].PushBack(std::make_pair(pipeline_hash, pipeline));
    }
  }
  else
  {
    pipeline = it->second;
  }
  assert(pipeline != VK_NULL_HANDLE);

  vkCmdBindPipeline(GetDirectCommandList(cmd), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void GraphicsDevice_Vulkan::predraw(CommandList cmd)
{
  pso_validate(cmd);

  if (active_pso[cmd]->desc.rootSignature == nullptr)
  {
    GetFrameResources().descriptors[cmd].validate(true, cmd);
  }
  else
  {
    auto rootsig_internal = to_internal(active_pso[cmd]->desc.rootSignature);
    if (rootsig_internal->dirty[cmd])
    {
      rootsig_internal->dirty[cmd] = false;
      vkCmdBindDescriptorSets(
        GetDirectCommandList(cmd),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        rootsig_internal->pipelineLayout,
        0,
        (ezUInt32)rootsig_internal->last_descriptorsets[cmd].GetCount(),
        rootsig_internal->last_descriptorsets[cmd].GetData(),
        (ezUInt32)rootsig_internal->root_offsets[cmd].GetCount(),
        rootsig_internal->root_offsets[cmd].GetData());
    }
  }
}
void GraphicsDevice_Vulkan::predispatch(CommandList cmd)
{
  if (active_cs[cmd]->rootSignature == nullptr)
  {
    GetFrameResources().descriptors[cmd].validate(false, cmd);
  }
  else
  {
    auto rootsig_internal = to_internal(active_cs[cmd]->rootSignature);
    if (rootsig_internal->dirty[cmd])
    {
      rootsig_internal->dirty[cmd] = false;
      vkCmdBindDescriptorSets(
        GetDirectCommandList(cmd),
        VK_PIPELINE_BIND_POINT_COMPUTE,
        rootsig_internal->pipelineLayout,
        0,
        (ezUInt32)rootsig_internal->last_descriptorsets[cmd].GetCount(),
        rootsig_internal->last_descriptorsets[cmd].GetData(),
        (ezUInt32)rootsig_internal->root_offsets[cmd].GetCount(),
        rootsig_internal->root_offsets[cmd].GetData());
    }
  }
}
void GraphicsDevice_Vulkan::preraytrace(CommandList cmd)
{
  if (active_rt[cmd]->desc.rootSignature == nullptr)
  {
    GetFrameResources().descriptors[cmd].validate(false, cmd, true);
  }
  else
  {
    auto rootsig_internal = to_internal(active_rt[cmd]->desc.rootSignature);
    if (rootsig_internal->dirty[cmd])
    {
      rootsig_internal->dirty[cmd] = false;
      vkCmdBindDescriptorSets(
        GetDirectCommandList(cmd),
        VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
        rootsig_internal->pipelineLayout,
        0,
        (ezUInt32)rootsig_internal->last_descriptorsets[cmd].GetCount(),
        rootsig_internal->last_descriptorsets[cmd].GetData(),
        (ezUInt32)rootsig_internal->root_offsets[cmd].GetCount(),
        rootsig_internal->root_offsets[cmd].GetData());
    }
  }
}

// Engine functions
GraphicsDevice_Vulkan::GraphicsDevice_Vulkan(RHIWindowType window, bool fullscreen, bool debuglayer)
{
  capabilities |= GRAPHICSDEVICE_CAPABILITY_DESCRIPTOR_MANAGEMENT;
  TOPLEVEL_ACCELERATION_STRUCTURE_INSTANCE_SIZE = sizeof(VkAccelerationStructureInstanceKHR);

  DEBUGDEVICE = debuglayer;

  FULLSCREEN = fullscreen;

#  ifdef _WIN32
  RECT rect;
  GetClientRect(window, &rect);
  RESOLUTIONWIDTH = rect.right - rect.left;
  RESOLUTIONHEIGHT = rect.bottom - rect.top;
#  elif SDL2
  int width, height;
  SDL_GetWindowSize(window, &width, &height);
  RESOLUTIONWIDTH = width;
  RESOLUTIONHEIGHT = height;
#  endif // _WIN32

  VkResult res;


  // Fill out application info:
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "ezEngine Application";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "eEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  // Enumerate available extensions:
  ezUInt32 extensionCount = 0;
  res = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  assert(res == VK_SUCCESS);
  ezDynamicArray<VkExtensionProperties> availableInstanceExtensions;
  availableInstanceExtensions.SetCount(extensionCount);
  res = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableInstanceExtensions.GetData());
  assert(res == VK_SUCCESS);

  ezDynamicArray<const char*> extensionNames;

  // Check if VK_EXT_debug_utils is supported, which supersedes VK_EXT_Debug_Report
  bool debugUtils = false;
  for (auto& available_extension : availableInstanceExtensions)
  {
    if (strcmp(available_extension.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
    {
      debugUtils = true;
      extensionNames.PushBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      break;
    }
  }

  if (!debugUtils)
  {
    extensionNames.PushBack(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  }


  extensionNames.PushBack(VK_KHR_SURFACE_EXTENSION_NAME);
#  ifdef _WIN32
  extensionNames.PushBack(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#  elif SDL2
  {
    ezUInt32 extensionCount;
    SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
    ezDynamicArray<const char*> extensionNames_sdl;
    extensionNames_sdl.SetCount(extensionCount);
    SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames_sdl.GetData());
    extensionNames.Reserve(extensionNames.GetCount() + extensionNames_sdl.GetCount());
    extensionNames.PushBackRange(extensionNames_sdl);
  }
#  endif // _WIN32

  bool enableValidationLayers = debuglayer;

  if (enableValidationLayers && !checkValidationLayerSupport())
  {
    EZ_REPORT_FAILURE("Vulkan validation layer requested but not available!");
    enableValidationLayers = false;
  }

  // Create instance:
  {
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensionNames.GetCount();
    createInfo.ppEnabledExtensionNames = extensionNames.GetData();
    createInfo.enabledLayerCount = 0;
    const ezDynamicArray<const char*> validationLayers = getValidationLayers();
    if (enableValidationLayers)
    {
      createInfo.enabledLayerCount = validationLayers.GetCount();
      createInfo.ppEnabledLayerNames = validationLayers.GetData();
    }
    res = vkCreateInstance(&createInfo, nullptr, &instance);
    assert(res == VK_SUCCESS);
  }

  // Register validation layer callback:
  if (enableValidationLayers)
  {
    if (debugUtils)
    {
      VkDebugUtilsMessengerCreateInfoEXT createInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
      createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
      createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
      createInfo.pfnUserCallback = debugUtilsMessengerCallback;
      res = CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugUtilsMessenger);
      assert(res == VK_SUCCESS);
    }
    else
    {
      VkDebugReportCallbackCreateInfoEXT createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
      createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
      createInfo.pfnCallback = debugCallback;
      res = CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &debugReportCallback);
      assert(res == VK_SUCCESS);
    }
  }


  // Surface creation:
  {
#  ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = window;
    createInfo.hinstance = GetModuleHandle(nullptr);

    auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");

    if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS)
    {
      assert(0);
    }
#  elif SDL2
    if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
    {
      throw sdl2::SDLError("Error creating a vulkan surface");
    }
#  else
#    error WICKEDENGINE VULKAN DEVICE ERROR: PLATFORM NOT SUPPORTED
#  endif // _WIN32
  }


  // Enumerating and creating devices:
  {
    ezUInt32 deviceCount = 0;
    VkResult res = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    assert(res == VK_SUCCESS);

    if (deviceCount == 0)
    {
      EZ_REPORT_FAILURE("failed to find GPUs with Vulkan support!");
      assert(0);
    }

    ezDynamicArray<VkPhysicalDevice> devices;
    devices.SetCount(deviceCount);
    res = vkEnumeratePhysicalDevices(instance, &deviceCount, devices.GetData());
    assert(res == VK_SUCCESS);

    device_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    device_properties_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    device_properties_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
    raytracing_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_KHR;
    mesh_shader_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV;

    device_properties.pNext = &device_properties_1_1;
    device_properties_1_1.pNext = &device_properties_1_2;
    device_properties_1_2.pNext = &raytracing_properties;
    raytracing_properties.pNext = &mesh_shader_properties;

    for (const auto& device : devices)
    {
      if (isDeviceSuitable(device, surface))
      {
        vkGetPhysicalDeviceProperties2(device, &device_properties);
        bool discrete = device_properties.properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        if (discrete || physicalDevice == VK_NULL_HANDLE)
        {
          physicalDevice = device;
          if (discrete)
          {
            break; // if this is discrete GPU, look no further (prioritize discrete GPU)
          }
        }
      }
    }

    if (physicalDevice == VK_NULL_HANDLE)
    {
      EZ_REPORT_FAILURE("failed to find a suitable GPU!");
      assert(0);
    }

    queueIndices = findQueueFamilies(physicalDevice, surface);

    ezDynamicArray<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = {queueIndices.graphicsFamily, queueIndices.presentFamily, queueIndices.copyFamily};

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies)
    {
      VkDeviceQueueCreateInfo queueCreateInfo = {};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.PushBack(queueCreateInfo);
    }

    assert(device_properties.properties.limits.timestampComputeAndGraphics == VK_TRUE);


    ezDynamicArray<const char*> enabled_deviceExtensions = getRequiredDeviceExtensions();
    res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    assert(res == VK_SUCCESS);
    ezDynamicArray<VkExtensionProperties> available_deviceExtensions;
    available_deviceExtensions.SetCount(extensionCount);
    res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, available_deviceExtensions.GetData());
    assert(res == VK_SUCCESS);

    if (checkDeviceExtensionSupport(VK_KHR_SPIRV_1_4_EXTENSION_NAME, available_deviceExtensions))
    {
      enabled_deviceExtensions.PushBack(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
    }

    device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    features_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

    device_features2.pNext = &features_1_1;
    features_1_1.pNext = &features_1_2;

#  ifdef ENABLE_RAYTRACING_EXTENSION
    raytracing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_FEATURES_KHR;
    if (checkDeviceExtensionSupport(VK_KHR_RAY_TRACING_EXTENSION_NAME, available_deviceExtensions))
    {
      SHADER_IDENTIFIER_SIZE = raytracing_properties.shaderGroupHandleSize;
      capabilities |= GRAPHICSDEVICE_CAPABILITY_RAYTRACING;
      enabled_deviceExtensions.PushBack(VK_KHR_RAY_TRACING_EXTENSION_NAME);
      enabled_deviceExtensions.PushBack(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
      enabled_deviceExtensions.PushBack(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
      enabled_deviceExtensions.PushBack(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
      features_1_2.pNext = &raytracing_features;
    }
#  endif // ENABLE_RAYTRACING_EXTENSION

    mesh_shader_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV;
    if (checkDeviceExtensionSupport(VK_NV_MESH_SHADER_EXTENSION_NAME, available_deviceExtensions))
    {
      enabled_deviceExtensions.PushBack(VK_NV_MESH_SHADER_EXTENSION_NAME);
      if (RAYTRACING)
      {
        raytracing_features.pNext = &mesh_shader_features;
      }
      else
      {
        features_1_2.pNext = &mesh_shader_features;
      }
    }

    vkGetPhysicalDeviceFeatures2(physicalDevice, &device_features2);

    assert(device_features2.features.imageCubeArray == VK_TRUE);
    assert(device_features2.features.independentBlend == VK_TRUE);
    assert(device_features2.features.geometryShader == VK_TRUE);
    assert(device_features2.features.samplerAnisotropy == VK_TRUE);
    assert(device_features2.features.shaderClipDistance == VK_TRUE);
    assert(device_features2.features.textureCompressionBC == VK_TRUE);
    assert(device_features2.features.occlusionQueryPrecise == VK_TRUE);
    if (device_features2.features.tessellationShader == VK_TRUE)
    {
      capabilities |= GRAPHICSDEVICE_CAPABILITY_TESSELLATION;
    }
    if (device_features2.features.shaderStorageImageExtendedFormats == VK_TRUE)
    {
      capabilities |= GRAPHICSDEVICE_CAPABILITY_UAV_LOAD_FORMAT_COMMON;
    }
    capabilities |= GRAPHICSDEVICE_CAPABILITY_RENDERTARGET_AND_VIEWPORT_ARRAYINDEX_WITHOUT_GS; // let's hope for the best...

    if (RAYTRACING)
    {
      assert(features_1_2.bufferDeviceAddress == VK_TRUE);
    }

    if (mesh_shader_features.meshShader == VK_TRUE && mesh_shader_features.taskShader == VK_TRUE)
    {
      // Enable mesh shader here (problematic with certain driver versions, disabled by default):
      //MESH_SHADER = true;
    }

    VkFormatProperties formatProperties = {};
    vkGetPhysicalDeviceFormatProperties(physicalDevice, _ConvertFormat(ezRHIFormat::R11G11B10_FLOAT), &formatProperties);
    if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)
    {
      capabilities |= GRAPHICSDEVICE_CAPABILITY_UAV_LOAD_FORMAT_R11G11B10_FLOAT;
    }

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = queueCreateInfos.GetCount();
    createInfo.pQueueCreateInfos = queueCreateInfos.GetData();

    createInfo.pEnabledFeatures = nullptr;
    createInfo.pNext = &device_features2;

    createInfo.enabledExtensionCount = enabled_deviceExtensions.GetCount();
    createInfo.ppEnabledExtensionNames = enabled_deviceExtensions.GetData();

    const ezDynamicArray<const char*> validationLayers = getValidationLayers();
    if (enableValidationLayers)
    {
      createInfo.enabledLayerCount = validationLayers.GetCount();
      createInfo.ppEnabledLayerNames = validationLayers.GetData();
    }
    else
    {
      createInfo.enabledLayerCount = 0;
    }

    res = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
    assert(res == VK_SUCCESS);

    vkGetDeviceQueue(device, queueIndices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, queueIndices.presentFamily, 0, &presentQueue);
  }

  allocationhandler = EZ_DEFAULT_NEW(VulkanAllocationHandler);
  allocationhandler->device = device;
  allocationhandler->instance = instance;

  // Initialize Vulkan Memory Allocator helper:
  VmaAllocatorCreateInfo allocatorInfo = {};
  allocatorInfo.physicalDevice = physicalDevice;
  allocatorInfo.device = device;
  allocatorInfo.instance = instance;
  if (features_1_2.bufferDeviceAddress)
  {
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  }
  res = vmaCreateAllocator(&allocatorInfo, &allocationhandler->allocator);
  assert(res == VK_SUCCESS);

  // Extension functions:
  setDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
  cmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetDeviceProcAddr(device, "vkCmdBeginDebugUtilsLabelEXT");
  cmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetDeviceProcAddr(device, "vkCmdEndDebugUtilsLabelEXT");
  cmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetDeviceProcAddr(device, "vkCmdInsertDebugUtilsLabelEXT");

  if (RAYTRACING)
  {
    createRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR)vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR");
    createAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR");
    bindAccelerationStructureMemoryKHR = (PFN_vkBindAccelerationStructureMemoryKHR)vkGetDeviceProcAddr(device, "vkBindAccelerationStructureMemoryKHR");
    destroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR)vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR");
    getAccelerationStructureMemoryRequirementsKHR = (PFN_vkGetAccelerationStructureMemoryRequirementsKHR)vkGetDeviceProcAddr(device, "vkGetAccelerationStructureMemoryRequirementsKHR");
    getAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR");
    getRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR");
    cmdBuildAccelerationStructureKHR = (PFN_vkCmdBuildAccelerationStructureKHR)vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructureKHR");
    cmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR)vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR");
  }

  if (CheckCapability(GRAPHICSDEVICE_CAPABILITY_MESH_SHADER))
  {
    cmdDrawMeshTasksNV = (PFN_vkCmdDrawMeshTasksNV)vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksNV");
    cmdDrawMeshTasksIndirectNV = (PFN_vkCmdDrawMeshTasksIndirectNV)vkGetDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectNV");
  }

  CreateBackBufferResources();

  QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

  // Create frame resources:
  {
    for (ezUInt32 fr = 0; fr < BACKBUFFER_COUNT; ++fr)
    {
      // Fence:
      {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        //fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkResult res = vkCreateFence(device, &fenceInfo, nullptr, &frames[fr].frameFence);
        assert(res == VK_SUCCESS);
      }

      // Create resources for transition command buffer:
      {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        poolInfo.flags = 0; // Optional

        res = vkCreateCommandPool(device, &poolInfo, nullptr, &frames[fr].transitionCommandPool);
        assert(res == VK_SUCCESS);

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandBufferCount = 1;
        commandBufferInfo.commandPool = frames[fr].transitionCommandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        res = vkAllocateCommandBuffers(device, &commandBufferInfo, &frames[fr].transitionCommandBuffer);
        assert(res == VK_SUCCESS);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        res = vkBeginCommandBuffer(frames[fr].transitionCommandBuffer, &beginInfo);
        assert(res == VK_SUCCESS);
      }


      // Create resources for copy (transfer) queue:
      {
        vkGetDeviceQueue(device, queueIndices.copyFamily, 0, &frames[fr].copyQueue);

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.copyFamily;
        poolInfo.flags = 0; // Optional

        res = vkCreateCommandPool(device, &poolInfo, nullptr, &frames[fr].copyCommandPool);
        assert(res == VK_SUCCESS);

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandBufferCount = 1;
        commandBufferInfo.commandPool = frames[fr].copyCommandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        res = vkAllocateCommandBuffers(device, &commandBufferInfo, &frames[fr].copyCommandBuffer);
        assert(res == VK_SUCCESS);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        res = vkBeginCommandBuffer(frames[fr].copyCommandBuffer, &beginInfo);
        assert(res == VK_SUCCESS);
      }
    }
  }

  // Create semaphores:
  {
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &copySemaphore);
    assert(res == VK_SUCCESS);

    for (ezUInt32 i = 0; i < BACKBUFFER_COUNT; i++)
    {
      res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &frames[i].swapchainAcquireSemaphore);
      assert(res == VK_SUCCESS);
      res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &frames[i].swapchainReleaseSemaphore);
      assert(res == VK_SUCCESS);
    }
  }


  // Create default null descriptors:
  {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = 4;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.flags = 0;


    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    res = vmaCreateBuffer(allocationhandler->allocator, &bufferInfo, &allocInfo, &nullBuffer, &nullBufferAllocation, nullptr);
    assert(res == VK_SUCCESS);

    VkBufferViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    viewInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    viewInfo.range = VK_WHOLE_SIZE;
    viewInfo.buffer = nullBuffer;
    res = vkCreateBufferView(device, &viewInfo, nullptr, &nullBufferView);
    assert(res == VK_SUCCESS);
  }
  {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.extent.width = 1;
    imageInfo.extent.height = 1;
    imageInfo.extent.depth = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.arrayLayers = 1;
    imageInfo.mipLevels = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
    imageInfo.flags = 0;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    imageInfo.imageType = VK_IMAGE_TYPE_1D;
    res = vmaCreateImage(allocationhandler->allocator, &imageInfo, &allocInfo, &nullImage1D, &nullImageAllocation1D, nullptr);
    assert(res == VK_SUCCESS);

    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    imageInfo.arrayLayers = 6;
    res = vmaCreateImage(allocationhandler->allocator, &imageInfo, &allocInfo, &nullImage2D, &nullImageAllocation2D, nullptr);
    assert(res == VK_SUCCESS);

    imageInfo.imageType = VK_IMAGE_TYPE_3D;
    imageInfo.flags = 0;
    imageInfo.arrayLayers = 1;
    res = vmaCreateImage(allocationhandler->allocator, &imageInfo, &allocInfo, &nullImage3D, &nullImageAllocation3D, nullptr);
    assert(res == VK_SUCCESS);

    // Transitions:
    ezLock lock(copyQueueMutex);
    {
      auto& frame = GetFrameResources();
      if (!copyQueueUse)
      {
        copyQueueUse = true;

        res = vkResetCommandPool(device, frame.copyCommandPool, 0);
        assert(res == VK_SUCCESS);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        res = vkBeginCommandBuffer(frame.copyCommandBuffer, &beginInfo);
        assert(res == VK_SUCCESS);
      }

      VkImageMemoryBarrier barrier = {};
      barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier.oldLayout = imageInfo.initialLayout;
      barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = nullImage1D;
      barrier.subresourceRange.layerCount = 1;
      frame.loadedimagetransitions.PushBack(barrier);
      barrier.image = nullImage2D;
      barrier.subresourceRange.layerCount = 6;
      frame.loadedimagetransitions.PushBack(barrier);
      barrier.image = nullImage3D;
      barrier.subresourceRange.layerCount = 1;
      frame.loadedimagetransitions.PushBack(barrier);
    }
    copyQueueMutex.Unlock();

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;

    viewInfo.image = nullImage1D;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
    res = vkCreateImageView(device, &viewInfo, nullptr, &nullImageView1D);
    assert(res == VK_SUCCESS);

    viewInfo.image = nullImage1D;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    res = vkCreateImageView(device, &viewInfo, nullptr, &nullImageView1DArray);
    assert(res == VK_SUCCESS);

    viewInfo.image = nullImage2D;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    res = vkCreateImageView(device, &viewInfo, nullptr, &nullImageView2D);
    assert(res == VK_SUCCESS);

    viewInfo.image = nullImage2D;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    res = vkCreateImageView(device, &viewInfo, nullptr, &nullImageView2DArray);
    assert(res == VK_SUCCESS);

    viewInfo.image = nullImage2D;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    viewInfo.subresourceRange.layerCount = 6;
    res = vkCreateImageView(device, &viewInfo, nullptr, &nullImageViewCube);
    assert(res == VK_SUCCESS);

    viewInfo.image = nullImage2D;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    viewInfo.subresourceRange.layerCount = 6;
    res = vkCreateImageView(device, &viewInfo, nullptr, &nullImageViewCubeArray);
    assert(res == VK_SUCCESS);

    viewInfo.image = nullImage3D;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
    res = vkCreateImageView(device, &viewInfo, nullptr, &nullImageView3D);
    assert(res == VK_SUCCESS);
  }
  {
    VkSamplerCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    res = vkCreateSampler(device, &createInfo, nullptr, &nullSampler);
    assert(res == VK_SUCCESS);
  }

  // GPU Queries:
  {
    timestamp_frequency = ezUInt64(1.0 / double(device_properties.properties.limits.timestampPeriod) * 1000 * 1000 * 1000);

    VkQueryPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;

    for (ezUInt32 i = 0; i < timestamp_query_count; ++i)
    {
      allocationhandler->free_timestampqueries.push_back(i);
    }
    poolInfo.queryCount = timestamp_query_count;
    poolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
    res = vkCreateQueryPool(device, &poolInfo, nullptr, &querypool_timestamp);
    assert(res == VK_SUCCESS);
    timestamps_to_reset.Reserve(timestamp_query_count);

    for (ezUInt32 i = 0; i < occlusion_query_count; ++i)
    {
      allocationhandler->free_occlusionqueries.push_back(i);
    }
    poolInfo.queryCount = occlusion_query_count;
    poolInfo.queryType = VK_QUERY_TYPE_OCCLUSION;
    res = vkCreateQueryPool(device, &poolInfo, nullptr, &querypool_occlusion);
    assert(res == VK_SUCCESS);
    occlusions_to_reset.Reserve(occlusion_query_count);
  }

  ezLog::Info("Created GraphicsDevice_Vulkan");
}
GraphicsDevice_Vulkan::~GraphicsDevice_Vulkan()
{
  VkResult res = vkQueueWaitIdle(graphicsQueue);
  assert(res == VK_SUCCESS);
  res = vkQueueWaitIdle(presentQueue);
  assert(res == VK_SUCCESS);

  for (auto& frame : frames)
  {
    vkDestroyFence(device, frame.frameFence, nullptr);
    for (auto& commandPool : frame.commandPools)
    {
      vkDestroyCommandPool(device, commandPool, nullptr);
    }
    vkDestroyCommandPool(device, frame.transitionCommandPool, nullptr);
    vkDestroyCommandPool(device, frame.copyCommandPool, nullptr);

    for (auto& descriptormanager : frame.descriptors)
    {
      descriptormanager.destroy();
    }

    vkDestroySemaphore(device, frame.swapchainAcquireSemaphore, nullptr);
    vkDestroySemaphore(device, frame.swapchainReleaseSemaphore, nullptr);
  }

  vkDestroySemaphore(device, copySemaphore, nullptr);

  for (auto& x : pipelines_worker)
  {
    for (auto& y : x)
    {
      vkDestroyPipeline(device, y.second, nullptr);
    }
  }
  for (auto& x : pipelines_global)
  {
    vkDestroyPipeline(device, x.second, nullptr);
  }

  vkDestroyQueryPool(device, querypool_timestamp, nullptr);
  vkDestroyQueryPool(device, querypool_occlusion, nullptr);

  vmaDestroyBuffer(allocationhandler->allocator, nullBuffer, nullBufferAllocation);
  vkDestroyBufferView(device, nullBufferView, nullptr);
  vmaDestroyImage(allocationhandler->allocator, nullImage1D, nullImageAllocation1D);
  vmaDestroyImage(allocationhandler->allocator, nullImage2D, nullImageAllocation2D);
  vmaDestroyImage(allocationhandler->allocator, nullImage3D, nullImageAllocation3D);
  vkDestroyImageView(device, nullImageView1D, nullptr);
  vkDestroyImageView(device, nullImageView1DArray, nullptr);
  vkDestroyImageView(device, nullImageView2D, nullptr);
  vkDestroyImageView(device, nullImageView2DArray, nullptr);
  vkDestroyImageView(device, nullImageViewCube, nullptr);
  vkDestroyImageView(device, nullImageViewCubeArray, nullptr);
  vkDestroyImageView(device, nullImageView3D, nullptr);
  vkDestroySampler(device, nullSampler, nullptr);

  vkDestroyRenderPass(device, defaultRenderPass, nullptr);
  for (size_t i = 0; i < swapChainImages.GetCount(); ++i)
  {
    vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
    vkDestroyImageView(device, swapChainImageViews[i], nullptr);
  }
  vkDestroySwapchainKHR(device, swapChain, nullptr);

  if (debugUtilsMessenger != VK_NULL_HANDLE)
  {
    DestroyDebugUtilsMessengerEXT(instance, debugUtilsMessenger, nullptr);
  }

  if (debugReportCallback != VK_NULL_HANDLE)
  {
    DestroyDebugReportCallbackEXT(instance, debugReportCallback, nullptr);
  }

  vkDestroySurfaceKHR(instance, surface, nullptr);
}

void GraphicsDevice_Vulkan::CreateBackBufferResources()
{
  SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

  VkSurfaceFormatKHR surfaceFormat = {};
  surfaceFormat.format = _ConvertFormat(BACKBUFFER_FORMAT);
  bool valid = false;

  for (const auto& format : swapChainSupport.formats)
  {
    if (format.format == surfaceFormat.format)
    {
      surfaceFormat = format;
      valid = true;
      break;
    }
  }
  if (!valid)
  {
    surfaceFormat = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    BACKBUFFER_FORMAT = ezRHIFormat::B8G8R8A8_UNORM;
  }

  swapChainExtent = {static_cast<ezUInt32>(RESOLUTIONWIDTH), static_cast<ezUInt32>(RESOLUTIONHEIGHT)};
  swapChainExtent.width = ezMath::Max(swapChainSupport.capabilities.minImageExtent.width, ezMath::Min(swapChainSupport.capabilities.maxImageExtent.width, swapChainExtent.width));
  swapChainExtent.height = ezMath::Max(swapChainSupport.capabilities.minImageExtent.height, ezMath::Min(swapChainSupport.capabilities.maxImageExtent.height, swapChainExtent.height));


  ezUInt32 imageCount = BACKBUFFER_COUNT;

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = swapChainExtent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

  ezUInt32 queueFamilyIndices[] = {(ezUInt32)queueIndices.graphicsFamily, (ezUInt32)queueIndices.presentFamily};

  if (queueIndices.graphicsFamily != queueIndices.presentFamily)
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;     // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // The only one that is always supported
  if (!VSYNC)
  {
    // The immediate present mode is not necessarily supported:
    for (auto& presentmode : swapChainSupport.presentModes)
    {
      if (presentmode == VK_PRESENT_MODE_IMMEDIATE_KHR)
      {
        createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        break;
      }
    }
  }
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = swapChain;

  VkResult res = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
  assert(res == VK_SUCCESS);

  if (createInfo.oldSwapchain != VK_NULL_HANDLE)
  {
    vkDestroySwapchainKHR(device, createInfo.oldSwapchain, nullptr);
  }

  res = vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  assert(res == VK_SUCCESS);
  assert(BACKBUFFER_COUNT <= imageCount);
  swapChainImages.SetCount(imageCount);
  res = vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.GetData());
  assert(res == VK_SUCCESS);
  swapChainImageFormat = surfaceFormat.format;

  VkDebugUtilsObjectNameInfoEXT info = {};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
  info.pObjectName = "SWAPCHAIN";
  info.objectType = VK_OBJECT_TYPE_IMAGE;
  for (auto& x : swapChainImages)
  {
    info.objectHandle = (ezUInt64)x;

    res = setDebugUtilsObjectNameEXT(device, &info);
    assert(res == VK_SUCCESS);
  }

  // Create default render pass:
  {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (defaultRenderPass != VK_NULL_HANDLE)
    {
      allocationhandler->destroyer_renderpasses.PushBack(std::make_pair(defaultRenderPass, allocationhandler->framecount));
    }
    res = vkCreateRenderPass(device, &renderPassInfo, nullptr, &defaultRenderPass);
    assert(res == VK_SUCCESS);
  }

  // Create swap chain render targets:
  swapChainImageViews.SetCount(swapChainImages.GetCount());
  swapChainFramebuffers.SetCount(swapChainImages.GetCount());
  for (size_t i = 0; i < swapChainImages.GetCount(); ++i)
  {
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapChainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (swapChainImageViews[i] != VK_NULL_HANDLE)
    {
      allocationhandler->destroyer_imageviews.PushBack(std::make_pair(swapChainImageViews[i], allocationhandler->framecount));
    }
    res = vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]);
    assert(res == VK_SUCCESS);

    VkImageView attachments[] = {
      swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = defaultRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = swapChainExtent.width;
    framebufferInfo.height = swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (swapChainFramebuffers[i] != VK_NULL_HANDLE)
    {
      allocationhandler->destroyer_framebuffers.PushBack(std::make_pair(swapChainFramebuffers[i], allocationhandler->framecount));
    }
    res = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
    assert(res == VK_SUCCESS);
  }
}

void GraphicsDevice_Vulkan::SetResolution(int width, int height)
{
  if (width != RESOLUTIONWIDTH || height != RESOLUTIONHEIGHT)
  {
    RESOLUTIONWIDTH = width;
    RESOLUTIONHEIGHT = height;

    CreateBackBufferResources();
  }
}

Texture GraphicsDevice_Vulkan::GetBackBuffer()
{
  auto internal_state = EZ_DEFAULT_NEW(Texture_Vulkan);
  internal_state->resource = swapChainImages[swapChainImageIndex];

  Texture result;
  result.type = GPUResource::GPU_RESOURCE_TYPE::TEXTURE;
  result.internal_state = internal_state;
  result.desc.type = TextureDesc::TEXTURE_2D;
  result.desc.Width = swapChainExtent.width;
  result.desc.Height = swapChainExtent.height;
  result.desc.Format = BACKBUFFER_FORMAT;
  return result;
}

bool GraphicsDevice_Vulkan::CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer)
{
  auto internal_state = EZ_DEFAULT_NEW(Buffer_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  pBuffer->internal_state = internal_state;
  pBuffer->type = GPUResource::GPU_RESOURCE_TYPE::BUFFER;

  pBuffer->desc = *pDesc;

  if (pDesc->Usage == ezRHIUsage::Dynamic && pDesc->BindFlags & BIND_CONSTANT_BUFFER)
  {
    // this special case will use frame allocator
    return true;
  }

  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = pBuffer->desc.ByteWidth;
  bufferInfo.usage = 0;
  if (pBuffer->desc.BindFlags & BIND_VERTEX_BUFFER)
  {
    bufferInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  }
  if (pBuffer->desc.BindFlags & BIND_INDEX_BUFFER)
  {
    bufferInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  }
  if (pBuffer->desc.BindFlags & BIND_CONSTANT_BUFFER)
  {
    bufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  }
  if (pBuffer->desc.BindFlags & BIND_SHADER_RESOURCE)
  {
    if (pBuffer->desc.Format == ezRHIFormat::Unknown)
    {
      bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    else
    {
      bufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
    }
  }
  if (pBuffer->desc.BindFlags & BIND_UNORDERED_ACCESS)
  {
    if (pBuffer->desc.Format == ezRHIFormat::Unknown)
    {
      bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    else
    {
      bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
    }
  }
  if (pBuffer->desc.MiscFlags & RESOURCE_MISC_INDIRECT_ARGS)
  {
    bufferInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
  }
  if (pBuffer->desc.MiscFlags & RESOURCE_MISC_RAY_TRACING)
  {
    bufferInfo.usage |= VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR;
  }
  if (features_1_2.bufferDeviceAddress == VK_TRUE)
  {
    bufferInfo.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
  }
  bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  bufferInfo.flags = 0;

  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;



  VkResult res;

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  if (pDesc->Usage == ezRHIUsage::Staging)
  {
    if (pDesc->CPUAccessFlags & CPU_ACCESS_READ)
    {
      allocInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
      bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    else
    {
      allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
      allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
      bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
  }

  res = vmaCreateBuffer(allocationhandler->allocator, &bufferInfo, &allocInfo, &internal_state->resource, &internal_state->allocation, nullptr);
  assert(res == VK_SUCCESS);

  // Issue data copy on request:
  if (pInitialData != nullptr)
  {
    GPUBufferDesc uploaddesc;
    uploaddesc.ByteWidth = pDesc->ByteWidth;
    uploaddesc.Usage = ezRHIUsage::Staging;
    GPUBuffer uploadbuffer;
    bool upload_success = CreateBuffer(&uploaddesc, nullptr, &uploadbuffer);
    assert(upload_success);
    VkBuffer upload_resource = to_internal(&uploadbuffer)->resource;
    VmaAllocation upload_allocation = to_internal(&uploadbuffer)->allocation;

    void* pData = upload_allocation->GetMappedData();
    assert(pData != nullptr);

    memcpy(pData, pInitialData->pSysMem, pBuffer->desc.ByteWidth);

    ezLock lock(copyQueueMutex);
    {
      auto& frame = GetFrameResources();
      if (!copyQueueUse)
      {
        copyQueueUse = true;

        res = vkResetCommandPool(device, frame.copyCommandPool, 0);
        assert(res == VK_SUCCESS);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        res = vkBeginCommandBuffer(frame.copyCommandBuffer, &beginInfo);
        assert(res == VK_SUCCESS);
      }

      VkBufferCopy copyRegion = {};
      copyRegion.size = pBuffer->desc.ByteWidth;
      copyRegion.srcOffset = 0;
      copyRegion.dstOffset = 0;

      VkBufferMemoryBarrier barrier = {};
      barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
      barrier.buffer = internal_state->resource;
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.size = VK_WHOLE_SIZE;

      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

      vkCmdPipelineBarrier(
        frame.copyCommandBuffer,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        1, &barrier,
        0, nullptr);


      vkCmdCopyBuffer(frame.copyCommandBuffer, upload_resource, internal_state->resource, 1, &copyRegion);


      VkAccessFlags tmp = barrier.srcAccessMask;
      barrier.srcAccessMask = barrier.dstAccessMask;
      barrier.dstAccessMask = 0;

      if (pBuffer->desc.BindFlags & BIND_CONSTANT_BUFFER)
      {
        barrier.dstAccessMask |= VK_ACCESS_UNIFORM_READ_BIT;
      }
      if (pBuffer->desc.BindFlags & BIND_VERTEX_BUFFER)
      {
        barrier.dstAccessMask |= VK_ACCESS_INDEX_READ_BIT;
      }
      if (pBuffer->desc.BindFlags & BIND_INDEX_BUFFER)
      {
        barrier.dstAccessMask |= VK_ACCESS_INDEX_READ_BIT;
      }
      if (pBuffer->desc.BindFlags & BIND_SHADER_RESOURCE)
      {
        barrier.dstAccessMask |= VK_ACCESS_SHADER_READ_BIT;
      }
      if (pBuffer->desc.BindFlags & BIND_UNORDERED_ACCESS)
      {
        barrier.dstAccessMask |= VK_ACCESS_SHADER_WRITE_BIT;
      }

      // transfer queue-ownership from copy to graphics:
      barrier.srcQueueFamilyIndex = queueIndices.copyFamily;
      barrier.dstQueueFamilyIndex = queueIndices.graphicsFamily;

      vkCmdPipelineBarrier(
        frame.copyCommandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0,
        0, nullptr,
        1, &barrier,
        0, nullptr);
    }
    copyQueueMutex.Unlock();
  }


  // Create resource views if needed
  if (pDesc->BindFlags & BIND_SHADER_RESOURCE)
  {
    CreateSubresource(pBuffer, SRV, 0);
  }
  if (pDesc->BindFlags & BIND_UNORDERED_ACCESS)
  {
    CreateSubresource(pBuffer, UAV, 0);
  }



  return res == VK_SUCCESS;
}
bool GraphicsDevice_Vulkan::CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, Texture* pTexture)
{
  auto internal_state = EZ_DEFAULT_NEW(Texture_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  pTexture->internal_state = internal_state;
  pTexture->type = GPUResource::GPU_RESOURCE_TYPE::TEXTURE;

  pTexture->desc = *pDesc;

  if (pTexture->desc.MipLevels == 0)
  {
    pTexture->desc.MipLevels = (ezUInt32)log2(ezMath::Max(pTexture->desc.Width, pTexture->desc.Height)) + 1;
  }

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkImageCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.extent.width = pTexture->desc.Width;
  imageInfo.extent.height = pTexture->desc.Height;
  imageInfo.extent.depth = 1;
  imageInfo.format = _ConvertFormat(pTexture->desc.Format);
  imageInfo.arrayLayers = pTexture->desc.ArraySize;
  imageInfo.mipLevels = pTexture->desc.MipLevels;
  imageInfo.samples = (VkSampleCountFlagBits)pTexture->desc.SampleCount;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.usage = 0;
  if (pTexture->desc.BindFlags & BIND_SHADER_RESOURCE)
  {
    imageInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
  }
  if (pTexture->desc.BindFlags & BIND_UNORDERED_ACCESS)
  {
    imageInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
  }
  if (pTexture->desc.BindFlags & BIND_RENDER_TARGET)
  {
    imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    allocInfo.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
  }
  if (pTexture->desc.BindFlags & BIND_DEPTH_STENCIL)
  {
    imageInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    allocInfo.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
  }
  imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  imageInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  imageInfo.flags = 0;
  if (pTexture->desc.MiscFlags & RESOURCE_MISC_TEXTURECUBE)
  {
    imageInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
  }

  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  switch (pTexture->desc.type)
  {
    case TextureDesc::TEXTURE_1D:
      imageInfo.imageType = VK_IMAGE_TYPE_1D;
      break;
    case TextureDesc::TEXTURE_2D:
      imageInfo.imageType = VK_IMAGE_TYPE_2D;
      break;
    case TextureDesc::TEXTURE_3D:
      imageInfo.imageType = VK_IMAGE_TYPE_3D;
      imageInfo.extent.depth = pTexture->desc.Depth;
      break;
    default:
      assert(0);
      break;
  }

  VkResult res;

  if (pTexture->desc.Usage == ezRHIUsage::Staging)
  {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageInfo.extent.width * imageInfo.extent.height * imageInfo.extent.depth * imageInfo.arrayLayers *
                      FormatHelpers::GetFormatStride(pTexture->desc.Format);

    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (pDesc->Usage == ezRHIUsage::Staging)
    {
      if (pDesc->CPUAccessFlags & CPU_ACCESS_READ)
      {
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      }
      else
      {
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      }
    }

    res = vmaCreateBuffer(allocationhandler->allocator, &bufferInfo, &allocInfo, &internal_state->staging_resource, &internal_state->allocation, nullptr);
    assert(res == VK_SUCCESS);

    imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
    VkImage image;
    res = vkCreateImage(device, &imageInfo, nullptr, &image);
    assert(res == VK_SUCCESS);

    VkImageSubresource subresource = {};
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vkGetImageSubresourceLayout(device, image, &subresource, &internal_state->subresourcelayout);

    vkDestroyImage(device, image, nullptr);
    return res == VK_SUCCESS;
  }
  else
  {
    res = vmaCreateImage(allocationhandler->allocator, &imageInfo, &allocInfo, &internal_state->resource, &internal_state->allocation, nullptr);
    assert(res == VK_SUCCESS);
  }

  // Issue data copy on request:
  if (pInitialData != nullptr)
  {
    GPUBufferDesc uploaddesc;
    uploaddesc.ByteWidth = (ezUInt32)internal_state->allocation->GetSize();
    uploaddesc.Usage = ezRHIUsage::Staging;
    GPUBuffer uploadbuffer;
    bool upload_success = CreateBuffer(&uploaddesc, nullptr, &uploadbuffer);
    assert(upload_success);
    VkBuffer upload_resource = to_internal(&uploadbuffer)->resource;
    VmaAllocation upload_allocation = to_internal(&uploadbuffer)->allocation;

    void* pData = upload_allocation->GetMappedData();
    assert(pData != nullptr);

    ezDynamicArray<VkBufferImageCopy> copyRegions;

    size_t cpyoffset = 0;
    ezUInt32 initDataIdx = 0;
    for (ezUInt32 slice = 0; slice < pDesc->ArraySize; ++slice)
    {
      ezUInt32 width = pDesc->Width;
      ezUInt32 height = pDesc->Height;
      for (ezUInt32 mip = 0; mip < pDesc->MipLevels; ++mip)
      {
        const SubresourceData& subresourceData = pInitialData[initDataIdx++];
        size_t cpysize = subresourceData.SysMemPitch * height;
        if (FormatHelpers::IsFormatBlockCompressed(pDesc->Format))
        {
          cpysize /= 4;
        }
        ezUInt8* cpyaddr = (ezUInt8*)pData + cpyoffset;
        memcpy(cpyaddr, subresourceData.pSysMem, cpysize);

        VkBufferImageCopy copyRegion = {};
        copyRegion.bufferOffset = cpyoffset;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;

        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = mip;
        copyRegion.imageSubresource.baseArrayLayer = slice;
        copyRegion.imageSubresource.layerCount = 1;

        copyRegion.imageOffset = {0, 0, 0};
        copyRegion.imageExtent = {
          width,
          height,
          1};

        width = ezMath::Max(1u, width / 2);
        height = ezMath::Max(1u, height / 2);

        copyRegions.PushBack(copyRegion);

        cpyoffset += Align(cpysize, FormatHelpers::GetFormatStride(pDesc->Format));
      }
    }

    ezLock lock(copyQueueMutex);
    {
      auto& frame = GetFrameResources();
      if (!copyQueueUse)
      {
        copyQueueUse = true;

        res = vkResetCommandPool(device, frame.copyCommandPool, 0);
        assert(res == VK_SUCCESS);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        res = vkBeginCommandBuffer(frame.copyCommandBuffer, &beginInfo);
        assert(res == VK_SUCCESS);
      }

      VkImageMemoryBarrier barrier = {};
      barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier.image = internal_state->resource;
      barrier.oldLayout = imageInfo.initialLayout;
      barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = imageInfo.arrayLayers;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = imageInfo.mipLevels;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

      vkCmdPipelineBarrier(
        frame.copyCommandBuffer,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

      vkCmdCopyBufferToImage(frame.copyCommandBuffer, upload_resource, internal_state->resource, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copyRegions.GetCount(), copyRegions.GetData());

      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.newLayout = _ConvertImageLayout(pTexture->desc.layout);
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

      frame.loadedimagetransitions.PushBack(barrier);
    }
    copyQueueMutex.Unlock();
  }
  else
  {
    ezLock lock(copyQueueMutex);

    auto& frame = GetFrameResources();
    if (!copyQueueUse)
    {
      copyQueueUse = true;

      res = vkResetCommandPool(device, frame.copyCommandPool, 0);
      assert(res == VK_SUCCESS);

      VkCommandBufferBeginInfo beginInfo = {};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
      beginInfo.pInheritanceInfo = nullptr; // Optional

      res = vkBeginCommandBuffer(frame.copyCommandBuffer, &beginInfo);
      assert(res == VK_SUCCESS);
    }

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = internal_state->resource;
    barrier.oldLayout = imageInfo.initialLayout;
    barrier.newLayout = _ConvertImageLayout(pTexture->desc.layout);
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    if (pTexture->desc.BindFlags & BIND_DEPTH_STENCIL)
    {
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
      if (FormatHelpers::IsFormatStencilSupport(pTexture->desc.Format))
      {
        barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
      }
    }
    else
    {
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = imageInfo.arrayLayers;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = imageInfo.mipLevels;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    frame.loadedimagetransitions.PushBack(barrier);

    copyQueueMutex.Unlock();
  }

  if (pTexture->desc.BindFlags & BIND_RENDER_TARGET)
  {
    CreateSubresource(pTexture, RTV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_DEPTH_STENCIL)
  {
    CreateSubresource(pTexture, DSV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_SHADER_RESOURCE)
  {
    CreateSubresource(pTexture, SRV, 0, -1, 0, -1);
  }
  if (pTexture->desc.BindFlags & BIND_UNORDERED_ACCESS)
  {
    CreateSubresource(pTexture, UAV, 0, -1, 0, -1);
  }

  return res == VK_SUCCESS;
}
bool GraphicsDevice_Vulkan::CreateInputLayout(const InputLayoutDesc* pInputElementDescs, ezUInt32 NumElements, const Shader* shader, InputLayout* pInputLayout)
{
  pInputLayout->internal_state = allocationhandler;

  pInputLayout->desc.Clear();
  pInputLayout->desc.Reserve((size_t)NumElements);
  for (ezUInt32 i = 0; i < NumElements; ++i)
  {
    pInputLayout->desc.PushBack(pInputElementDescs[i]);
  }

  return true;
}
bool GraphicsDevice_Vulkan::CreateShader(ezEnum<ezRHIShaderStage> stage, const void* pShaderBytecode, size_t BytecodeLength, Shader* pShader)
{
  auto internal_state = EZ_DEFAULT_NEW(Shader_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  pShader->internal_state = internal_state;

  pShader->code.SetCount(BytecodeLength);
  //std::memcpy(pShader->code.GetData(), pShaderBytecode, BytecodeLength);
  ezMemoryUtils::Copy(pShader->code.GetData(), const_cast<ezUInt8*>(reinterpret_cast<const ezUInt8*>(pShaderBytecode)), BytecodeLength);
  pShader->stage = stage;

  VkResult res = VK_SUCCESS;

  VkShaderModuleCreateInfo moduleInfo = {};
  moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  moduleInfo.codeSize = pShader->code.GetCount();
  moduleInfo.pCode = (uint32_t*)pShader->code.GetData();
  res = vkCreateShaderModule(device, &moduleInfo, nullptr, &internal_state->shaderModule);
  assert(res == VK_SUCCESS);

  internal_state->stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  internal_state->stageInfo.module = internal_state->shaderModule;
  internal_state->stageInfo.pName = "main";
  switch (stage)
  {
    case ezRHIShaderStage::MeshShader:
      internal_state->stageInfo.stage = VK_SHADER_STAGE_MESH_BIT_NV;
      break;
    case ezRHIShaderStage::AmplificationShader:
      internal_state->stageInfo.stage = VK_SHADER_STAGE_TASK_BIT_NV;
      break;
    case ezRHIShaderStage::VertexShader:
      internal_state->stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
      break;
    case ezRHIShaderStage::HullShader:
      internal_state->stageInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
      break;
    case ezRHIShaderStage::DomainShader:
      internal_state->stageInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
      break;
    case ezRHIShaderStage::GeometryShader:
      internal_state->stageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
      break;
    case ezRHIShaderStage::PixelShader:
      internal_state->stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      break;
    case ezRHIShaderStage::ComputeShader:
      internal_state->stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
      break;
    default:
      internal_state->stageInfo.stage = VK_SHADER_STAGE_ALL;
      // library shader (ray tracing)
      break;
  }

  if (pShader->rootSignature == nullptr)
  {
    // Perform shader reflection for shaders that don't specify a root signature:

    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(moduleInfo.codeSize, moduleInfo.pCode, &module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    uint32_t binding_count = 0;
    result = spvReflectEnumerateEntryPointDescriptorBindings(
      &module, internal_state->stageInfo.pName, &binding_count, nullptr);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    ezDynamicArray<SpvReflectDescriptorBinding*> bindings;
    bindings.SetCount(binding_count);
    result = spvReflectEnumerateEntryPointDescriptorBindings(
      &module, internal_state->stageInfo.pName, &binding_count, bindings.GetData());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    ezDynamicArray<VkDescriptorSetLayoutBinding>& layoutBindings = internal_state->layoutBindings;
    ezDynamicArray<VkImageViewType>& imageViewTypes = internal_state->imageViewTypes;

    for (auto& x : bindings)
    {
      imageViewTypes.PushBack(VK_IMAGE_VIEW_TYPE_MAX_ENUM);
      layoutBindings.ExpandAndGetRef();
      layoutBindings.PeekBack().stageFlags = internal_state->stageInfo.stage;
      layoutBindings.PeekBack().binding = x->binding;
      layoutBindings.PeekBack().descriptorCount = 1;

      switch (x->descriptor_type)
      {
        default:
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
          layoutBindings.PeekBack().descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
          break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
          if (x->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
          {
            layoutBindings.PeekBack().descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
          }
          else
          {
            layoutBindings.PeekBack().descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
          }
          switch (x->image.dim)
          {
            default:
            case SpvDim1D:
              if (x->image.arrayed == 0)
              {
                imageViewTypes.PeekBack() = VK_IMAGE_VIEW_TYPE_1D;
              }
              else
              {
                imageViewTypes.PeekBack() = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
              }
              break;
            case SpvDim2D:
              if (x->image.arrayed == 0)
              {
                imageViewTypes.PeekBack() = VK_IMAGE_VIEW_TYPE_2D;
              }
              else
              {
                imageViewTypes.PeekBack() = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
              }
              break;
            case SpvDim3D:
              imageViewTypes.PeekBack() = VK_IMAGE_VIEW_TYPE_3D;
              break;
            case SpvDimCube:
              if (x->image.arrayed == 0)
              {
                imageViewTypes.PeekBack() = VK_IMAGE_VIEW_TYPE_CUBE;
              }
              else
              {
                imageViewTypes.PeekBack() = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
              }
              break;
          }
          break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
          layoutBindings.PeekBack().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
          break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
          layoutBindings.PeekBack().descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
          break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
          layoutBindings.PeekBack().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
          break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
          layoutBindings.PeekBack().descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
          break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
          layoutBindings.PeekBack().descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
          break;
      }
    }

    spvReflectDestroyShaderModule(&module);

    if (stage == ezRHIShaderStage::ComputeShader || stage == ezRHIShaderStage::ENUM_COUNT)
    {
      VkDescriptorSetLayoutCreateInfo descriptorSetlayoutInfo = {};
      descriptorSetlayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      descriptorSetlayoutInfo.pBindings = layoutBindings.GetData();
      descriptorSetlayoutInfo.bindingCount = layoutBindings.GetCount();
      res = vkCreateDescriptorSetLayout(device, &descriptorSetlayoutInfo, nullptr, &internal_state->descriptorSetLayout);
      assert(res == VK_SUCCESS);

      VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
      pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutInfo.pSetLayouts = &internal_state->descriptorSetLayout;
      pipelineLayoutInfo.setLayoutCount = 1; // cs
      pipelineLayoutInfo.pushConstantRangeCount = 0;

      res = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &internal_state->pipelineLayout_cs);
      assert(res == VK_SUCCESS);
    }
  }

  if (stage == ezRHIShaderStage::ComputeShader)
  {
    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    if (pShader->rootSignature == nullptr)
    {
      // pipeline layout from reflection:
      pipelineInfo.layout = internal_state->pipelineLayout_cs;
    }
    else
    {
      // pipeline layout from root signature:
      pipelineInfo.layout = to_internal(pShader->rootSignature)->pipelineLayout;
    }
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    // Create compute pipeline state in place:
    pipelineInfo.stage = internal_state->stageInfo;


    res = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &internal_state->pipeline_cs);
    assert(res == VK_SUCCESS);
  }

  return res == VK_SUCCESS;
}
bool GraphicsDevice_Vulkan::CreateBlendState(const BlendStateDesc* pBlendStateDesc, BlendState* pBlendState)
{
  pBlendState->internal_state = allocationhandler;

  pBlendState->desc = *pBlendStateDesc;
  return true;
}
bool GraphicsDevice_Vulkan::CreateDepthStencilState(const DepthStencilStateDesc* pDepthStencilStateDesc, DepthStencilState* pDepthStencilState)
{
  pDepthStencilState->internal_state = allocationhandler;

  pDepthStencilState->desc = *pDepthStencilStateDesc;
  return true;
}
bool GraphicsDevice_Vulkan::CreateRasterizerState(const RasterizerStateDesc* pRasterizerStateDesc, RasterizerState* pRasterizerState)
{
  pRasterizerState->internal_state = allocationhandler;

  pRasterizerState->desc = *pRasterizerStateDesc;
  return true;
}
bool GraphicsDevice_Vulkan::CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState)
{
  auto internal_state = EZ_DEFAULT_NEW(Sampler_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  pSamplerState->internal_state = internal_state;

  pSamplerState->desc = *pSamplerDesc;

  VkSamplerCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pNext = nullptr;


  switch (pSamplerDesc->Filter)
  {
    case ezRHIFilter::FILTER_MIN_MAG_MIP_POINT:
      createInfo.minFilter = VK_FILTER_NEAREST;
      createInfo.magFilter = VK_FILTER_NEAREST;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = false;
      break;
    case ezRHIFilter::FILTER_MIN_MAG_POINT_MIP_LINEAR:
      createInfo.minFilter = VK_FILTER_NEAREST;
      createInfo.magFilter = VK_FILTER_NEAREST;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = false;
      break;
    case ezRHIFilter::FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
      createInfo.minFilter = VK_FILTER_NEAREST;
      createInfo.magFilter = VK_FILTER_LINEAR;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = false;
      break;
    case ezRHIFilter::FILTER_MIN_POINT_MAG_MIP_LINEAR:
      createInfo.minFilter = VK_FILTER_NEAREST;
      createInfo.magFilter = VK_FILTER_LINEAR;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = false;
      break;
    case ezRHIFilter::FILTER_MIN_LINEAR_MAG_MIP_POINT:
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.magFilter = VK_FILTER_NEAREST;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = false;
      break;
    case ezRHIFilter::FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.magFilter = VK_FILTER_NEAREST;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = false;
      break;
    case ezRHIFilter::FILTER_MIN_MAG_LINEAR_MIP_POINT:
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.magFilter = VK_FILTER_LINEAR;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = false;
      break;
    case ezRHIFilter::FILTER_MIN_MAG_MIP_LINEAR:
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.magFilter = VK_FILTER_LINEAR;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = false;
      break;
    case ezRHIFilter::FILTER_ANISOTROPIC:
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.magFilter = VK_FILTER_LINEAR;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      createInfo.anisotropyEnable = true;
      createInfo.compareEnable = false;
      break;
    case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_MIP_POINT:
      createInfo.minFilter = VK_FILTER_NEAREST;
      createInfo.magFilter = VK_FILTER_NEAREST;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = true;
      break;
    case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
      createInfo.minFilter = VK_FILTER_NEAREST;
      createInfo.magFilter = VK_FILTER_NEAREST;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = true;
      break;
    case ezRHIFilter::FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
      createInfo.minFilter = VK_FILTER_NEAREST;
      createInfo.magFilter = VK_FILTER_LINEAR;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = true;
      break;
    case ezRHIFilter::FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
      createInfo.minFilter = VK_FILTER_NEAREST;
      createInfo.magFilter = VK_FILTER_NEAREST;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = true;
      break;
    case ezRHIFilter::FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.magFilter = VK_FILTER_NEAREST;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = true;
      break;
    case ezRHIFilter::FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.magFilter = VK_FILTER_NEAREST;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = true;
      break;
    case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.magFilter = VK_FILTER_LINEAR;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = true;
      break;
    case ezRHIFilter::FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.magFilter = VK_FILTER_LINEAR;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = true;
      break;
    case ezRHIFilter::FILTER_COMPARISON_ANISOTROPIC:
      createInfo.minFilter = VK_FILTER_LINEAR;
      createInfo.magFilter = VK_FILTER_LINEAR;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      createInfo.anisotropyEnable = true;
      createInfo.compareEnable = true;
      break;
    case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_MIP_POINT:
    case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
    case ezRHIFilter::FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case ezRHIFilter::FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
    case ezRHIFilter::FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
    case ezRHIFilter::FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
    case ezRHIFilter::FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
    case ezRHIFilter::FILTER_MINIMUM_ANISOTROPIC:
    case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
    case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
    case ezRHIFilter::FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
    case ezRHIFilter::FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
    case ezRHIFilter::FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
    case ezRHIFilter::FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
    case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
    case ezRHIFilter::FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
    case ezRHIFilter::FILTER_MAXIMUM_ANISOTROPIC:
    default:
      createInfo.minFilter = VK_FILTER_NEAREST;
      createInfo.magFilter = VK_FILTER_NEAREST;
      createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
      createInfo.anisotropyEnable = false;
      createInfo.compareEnable = false;
      break;
  }

  createInfo.addressModeU = _ConvertTextureAddressMode(pSamplerDesc->AddressU);
  createInfo.addressModeV = _ConvertTextureAddressMode(pSamplerDesc->AddressV);
  createInfo.addressModeW = _ConvertTextureAddressMode(pSamplerDesc->AddressW);
  createInfo.maxAnisotropy = static_cast<float>(pSamplerDesc->MaxAnisotropy);
  createInfo.compareOp = _ConvertComparisonFunc(pSamplerDesc->ComparisonFunc);
  createInfo.minLod = pSamplerDesc->MinLOD;
  createInfo.maxLod = pSamplerDesc->MaxLOD;
  createInfo.mipLodBias = pSamplerDesc->MipLODBias;
  createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
  createInfo.unnormalizedCoordinates = VK_FALSE;

  VkResult res = vkCreateSampler(device, &createInfo, nullptr, &internal_state->resource);
  assert(res == VK_SUCCESS);

  return res == VK_SUCCESS;
}
bool GraphicsDevice_Vulkan::CreateQuery(const GPUQueryDesc* pDesc, GPUQuery* pQuery)
{
  auto internal_state = EZ_DEFAULT_NEW(Query_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  pQuery->internal_state = internal_state;

  pQuery->desc = *pDesc;
  internal_state->query_type = pQuery->desc.Type;

  switch (pDesc->Type)
  {
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP:
      if (!allocationhandler->free_timestampqueries.pop_front(internal_state->query_index))
      {
        internal_state->query_type = ezRHIGPUQueryType::GPU_QUERY_TYPE_INVALID;
        return false;
      }
      break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP_DISJOINT:
      break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION:
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
      if (!allocationhandler->free_occlusionqueries.pop_front(internal_state->query_index))
      {
        internal_state->query_type = ezRHIGPUQueryType::GPU_QUERY_TYPE_INVALID;
        return false;
      }
      break;
  }

  return true;
}
bool GraphicsDevice_Vulkan::CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso)
{
  auto internal_state = EZ_DEFAULT_NEW(PipelineState_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  pso->internal_state = internal_state;

  pso->desc = *pDesc;

  pso->hash = ezHashHelper<PipelineStateDesc>::Hash(pso->desc);

  if (pDesc->rootSignature == nullptr)
  {
    // Descriptor set layout comes from reflection data when there is no root signature specified:

    auto insert_shader = [&](const Shader* shader) {
      if (shader == nullptr)
        return;
      auto shader_internal = to_internal(shader);

      ezUInt32 i = 0;
      size_t check_max = internal_state->layoutBindings.GetCount(); // dont't check for duplicates within self table
      for (auto& x : shader_internal->layoutBindings)
      {
        bool found = false;
        size_t j = 0;
        for (auto& y : internal_state->layoutBindings)
        {
          if (x.binding == y.binding)
          {
            // If the asserts fire, it means there are overlapping bindings between shader stages
            //	This is not supported now for performance reasons (less binding management)!
            //	(Overlaps between s/b/t bind points are not a problem because those are shifted by the compiler)
            assert(x.descriptorCount == y.descriptorCount);
            assert(x.descriptorType == y.descriptorType);
            found = true;
            y.stageFlags |= x.stageFlags;
            break;
          }
          if (j++ >= check_max)
            break;
        }

        if (!found)
        {
          internal_state->layoutBindings.PushBack(x);
          internal_state->imageViewTypes.PushBack(shader_internal->imageViewTypes[i]);
        }
        i++;
      }
    };

    insert_shader(pDesc->ms);
    insert_shader(pDesc->as);
    insert_shader(pDesc->vs);
    insert_shader(pDesc->hs);
    insert_shader(pDesc->ds);
    insert_shader(pDesc->gs);
    insert_shader(pDesc->ps);

    VkDescriptorSetLayoutCreateInfo descriptorSetlayoutInfo = {};
    descriptorSetlayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetlayoutInfo.pBindings = internal_state->layoutBindings.GetData();
    descriptorSetlayoutInfo.bindingCount = internal_state->layoutBindings.GetCount();
    VkResult res = vkCreateDescriptorSetLayout(device, &descriptorSetlayoutInfo, nullptr, &internal_state->descriptorSetLayout);
    assert(res == VK_SUCCESS);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pSetLayouts = &internal_state->descriptorSetLayout;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    res = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &internal_state->pipelineLayout);
    assert(res == VK_SUCCESS);

    return res == VK_SUCCESS;
  }

  return true;
}
bool GraphicsDevice_Vulkan::CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass)
{
  auto internal_state = EZ_DEFAULT_NEW(RenderPass_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  renderpass->internal_state = internal_state;

  renderpass->desc = *pDesc;

  renderpass->hash = ezHashHelper<RenderPassDesc>::Hash(renderpass->desc);

  VkResult res;

  VkImageView attachments[17] = {};
  VkAttachmentDescription attachmentDescriptions[17] = {};
  VkAttachmentReference colorAttachmentRefs[8] = {};
  VkAttachmentReference resolveAttachmentRefs[8] = {};
  VkAttachmentReference depthAttachmentRef = {};

  int resolvecount = 0;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  const RenderPassDesc& desc = renderpass->desc;

  ezUInt32 validAttachmentCount = 0;
  for (auto& attachment : renderpass->desc.attachments)
  {
    const Texture* texture = attachment.texture;
    const TextureDesc& texdesc = texture->desc;
    int subresource = attachment.subresource;
    auto texture_internal_state = to_internal(texture);

    attachmentDescriptions[validAttachmentCount].format = _ConvertFormat(texdesc.Format);
    attachmentDescriptions[validAttachmentCount].samples = (VkSampleCountFlagBits)texdesc.SampleCount;

    switch (attachment.loadop)
    {
      default:
      case RenderPassAttachment::LOADOP_LOAD:
        attachmentDescriptions[validAttachmentCount].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        break;
      case RenderPassAttachment::LOADOP_CLEAR:
        attachmentDescriptions[validAttachmentCount].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        break;
      case RenderPassAttachment::LOADOP_DONTCARE:
        attachmentDescriptions[validAttachmentCount].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        break;
    }

    switch (attachment.storeop)
    {
      default:
      case RenderPassAttachment::STOREOP_STORE:
        attachmentDescriptions[validAttachmentCount].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        break;
      case RenderPassAttachment::STOREOP_DONTCARE:
        attachmentDescriptions[validAttachmentCount].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        break;
    }

    attachmentDescriptions[validAttachmentCount].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[validAttachmentCount].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    attachmentDescriptions[validAttachmentCount].initialLayout = _ConvertImageLayout(attachment.initial_layout);
    attachmentDescriptions[validAttachmentCount].finalLayout = _ConvertImageLayout(attachment.final_layout);

    if (attachment.type == RenderPassAttachment::RENDERTARGET)
    {
      if (subresource < 0 || texture_internal_state->subresources_rtv.IsEmpty())
      {
        attachments[validAttachmentCount] = texture_internal_state->rtv;
      }
      else
      {
        assert(texture_internal_state->subresources_rtv.GetCount() > size_t(subresource) && "Invalid RTV subresource!");
        attachments[validAttachmentCount] = texture_internal_state->subresources_rtv[subresource];
      }
      if (attachments[validAttachmentCount] == VK_NULL_HANDLE)
      {
        continue;
      }

      colorAttachmentRefs[subpass.colorAttachmentCount].attachment = validAttachmentCount;
      colorAttachmentRefs[subpass.colorAttachmentCount].layout = _ConvertImageLayout(attachment.subpass_layout);
      subpass.colorAttachmentCount++;
      subpass.pColorAttachments = colorAttachmentRefs;
    }
    else if (attachment.type == RenderPassAttachment::DEPTH_STENCIL)
    {
      if (subresource < 0 || texture_internal_state->subresources_dsv.IsEmpty())
      {
        attachments[validAttachmentCount] = texture_internal_state->dsv;
      }
      else
      {
        assert(texture_internal_state->subresources_dsv.GetCount() > size_t(subresource) && "Invalid DSV subresource!");
        attachments[validAttachmentCount] = texture_internal_state->subresources_dsv[subresource];
      }
      if (attachments[validAttachmentCount] == VK_NULL_HANDLE)
      {
        continue;
      }

      if (FormatHelpers::IsFormatStencilSupport(texdesc.Format))
      {
        switch (attachment.loadop)
        {
          default:
          case RenderPassAttachment::LOADOP_LOAD:
            attachmentDescriptions[validAttachmentCount].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            break;
          case RenderPassAttachment::LOADOP_CLEAR:
            attachmentDescriptions[validAttachmentCount].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            break;
          case RenderPassAttachment::LOADOP_DONTCARE:
            attachmentDescriptions[validAttachmentCount].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            break;
        }

        switch (attachment.storeop)
        {
          default:
          case RenderPassAttachment::STOREOP_STORE:
            attachmentDescriptions[validAttachmentCount].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            break;
          case RenderPassAttachment::STOREOP_DONTCARE:
            attachmentDescriptions[validAttachmentCount].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            break;
        }
      }

      depthAttachmentRef.attachment = validAttachmentCount;
      depthAttachmentRef.layout = _ConvertImageLayout(attachment.subpass_layout);
      subpass.pDepthStencilAttachment = &depthAttachmentRef;
    }
    else if (attachment.type == RenderPassAttachment::RESOLVE)
    {
      if (attachment.texture == nullptr)
      {
        resolveAttachmentRefs[resolvecount].attachment = VK_ATTACHMENT_UNUSED;
      }
      else
      {
        if (subresource < 0 || texture_internal_state->subresources_srv.IsEmpty())
        {
          attachments[validAttachmentCount] = texture_internal_state->srv;
        }
        else
        {
          assert(texture_internal_state->subresources_srv.GetCount() > size_t(subresource) && "Invalid SRV subresource!");
          attachments[validAttachmentCount] = texture_internal_state->subresources_srv[subresource];
        }
        if (attachments[validAttachmentCount] == VK_NULL_HANDLE)
        {
          continue;
        }
        resolveAttachmentRefs[resolvecount].attachment = validAttachmentCount;
        resolveAttachmentRefs[resolvecount].layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      }

      resolvecount++;
      subpass.pResolveAttachments = resolveAttachmentRefs;
    }

    validAttachmentCount++;
  }
  assert(renderpass->desc.attachments.GetCount() == validAttachmentCount);

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = validAttachmentCount;
  renderPassInfo.pAttachments = attachmentDescriptions;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  res = vkCreateRenderPass(device, &renderPassInfo, nullptr, &internal_state->renderpass);
  assert(res == VK_SUCCESS);

  VkFramebufferCreateInfo framebufferInfo = {};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = internal_state->renderpass;
  framebufferInfo.attachmentCount = validAttachmentCount;
  framebufferInfo.pAttachments = attachments;

  if (validAttachmentCount > 0)
  {
    const TextureDesc& texdesc = desc.attachments[0].texture->desc;
    framebufferInfo.width = texdesc.Width;
    framebufferInfo.height = texdesc.Height;
    framebufferInfo.layers = texdesc.MiscFlags & RESOURCE_MISC_TEXTURECUBE ? 6 : 1; // todo figure out better! can't use ArraySize here, it will crash!
  }
  else
  {
    framebufferInfo.width = 1;
    framebufferInfo.height = 1;
    framebufferInfo.layers = 1;
  }

  res = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &internal_state->framebuffer);
  assert(res == VK_SUCCESS);


  internal_state->beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  internal_state->beginInfo.renderPass = internal_state->renderpass;
  internal_state->beginInfo.framebuffer = internal_state->framebuffer;

  if (validAttachmentCount > 0)
  {
    const TextureDesc& texdesc = desc.attachments[0].texture->desc;

    internal_state->beginInfo.renderArea.offset = {0, 0};
    internal_state->beginInfo.renderArea.extent.width = texdesc.Width;
    internal_state->beginInfo.renderArea.extent.height = texdesc.Height;
    internal_state->beginInfo.clearValueCount = validAttachmentCount;
    internal_state->beginInfo.pClearValues = internal_state->clearColors;

    int i = 0;
    for (auto& attachment : desc.attachments)
    {
      if (desc.attachments[i].type == RenderPassAttachment::RESOLVE || attachment.texture == nullptr)
        continue;

      const ClearValue& clear = desc.attachments[i].texture->desc.clear;
      if (desc.attachments[i].type == RenderPassAttachment::RENDERTARGET)
      {
        internal_state->clearColors[i].color.float32[0] = clear.color[0];
        internal_state->clearColors[i].color.float32[1] = clear.color[1];
        internal_state->clearColors[i].color.float32[2] = clear.color[2];
        internal_state->clearColors[i].color.float32[3] = clear.color[3];
      }
      else if (desc.attachments[i].type == RenderPassAttachment::DEPTH_STENCIL)
      {
        internal_state->clearColors[i].depthStencil.depth = clear.depthstencil.depth;
        internal_state->clearColors[i].depthStencil.stencil = clear.depthstencil.stencil;
      }
      else
      {
        assert(0);
      }
      i++;
    }
  }

  return res == VK_SUCCESS;
}
bool GraphicsDevice_Vulkan::CreateRaytracingAccelerationStructure(const RaytracingAccelerationStructureDesc* pDesc, RaytracingAccelerationStructure* bvh)
{
  auto internal_state = EZ_DEFAULT_NEW(BVH_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  bvh->internal_state = internal_state;
  bvh->type = GPUResource::GPU_RESOURCE_TYPE::RAYTRACING_ACCELERATION_STRUCTURE;

  bvh->desc = *pDesc;

  VkAccelerationStructureCreateInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;

  if (pDesc->_flags & RaytracingAccelerationStructureDesc::FLAG_ALLOW_UPDATE)
  {
    info.flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
  }
  if (pDesc->_flags & RaytracingAccelerationStructureDesc::FLAG_ALLOW_COMPACTION)
  {
    info.flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;
  }
  if (pDesc->_flags & RaytracingAccelerationStructureDesc::FLAG_PREFER_FAST_TRACE)
  {
    info.flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  }
  if (pDesc->_flags & RaytracingAccelerationStructureDesc::FLAG_PREFER_FAST_BUILD)
  {
    info.flags |= VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR;
  }
  if (pDesc->_flags & RaytracingAccelerationStructureDesc::FLAG_MINIMIZE_MEMORY)
  {
    info.flags |= VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR;
  }

  switch (pDesc->type)
  {
    case RaytracingAccelerationStructureDesc::BOTTOMLEVEL:
    {
      info.type = VkAccelerationStructureTypeKHR::VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

      for (auto& x : pDesc->bottomlevel.geometries)
      {
        auto& geometry = internal_state->geometries.ExpandAndGetRef();
        geometry = {};
        geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;

        if (x._flags & RaytracingAccelerationStructureDesc::BottomLevel::Geometry::FLAG_USE_TRANSFORM)
        {
          geometry.allowsTransforms = VK_TRUE;
        }

        if (x.type == RaytracingAccelerationStructureDesc::BottomLevel::Geometry::TRIANGLES)
        {
          geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
          geometry.maxPrimitiveCount = x.triangles.indexCount / 3;
          geometry.indexType = x.triangles.indexFormat == ezRHIIndexBufferFormat::UInt16 ? VkIndexType::VK_INDEX_TYPE_UINT16 : VkIndexType::VK_INDEX_TYPE_UINT32;
          geometry.maxVertexCount = x.triangles.vertexCount;
          geometry.vertexFormat = _ConvertFormat(x.triangles.vertexFormat);
        }
        else if (x.type == RaytracingAccelerationStructureDesc::BottomLevel::Geometry::PROCEDURAL_AABBS)
        {
          geometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
          geometry.maxPrimitiveCount = x.aabbs.count;
        }
      }
    }
    break;
    case RaytracingAccelerationStructureDesc::TOPLEVEL:
    {
      info.type = VkAccelerationStructureTypeKHR::VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

      auto& geometry = internal_state->geometries.ExpandAndGetRef();
      geometry = {};
      geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR;
      geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
      geometry.allowsTransforms = VK_TRUE;
      geometry.maxPrimitiveCount = pDesc->toplevel.count;
    }
    break;
  }

  info.pGeometryInfos = internal_state->geometries.GetData();
  info.maxGeometryCount = (ezUInt32)internal_state->geometries.GetCount();
  internal_state->info = info;

  VkResult res = createAccelerationStructureKHR(device, &info, nullptr, &internal_state->resource);
  assert(res == VK_SUCCESS);

  VkAccelerationStructureMemoryRequirementsInfoKHR meminfo = {};
  meminfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR;
  meminfo.accelerationStructure = internal_state->resource;

  meminfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_KHR;
  VkMemoryRequirements2 memrequirements = {};
  memrequirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
  getAccelerationStructureMemoryRequirementsKHR(device, &meminfo, &memrequirements);


  meminfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_KHR;
  VkMemoryRequirements2 memrequirements_scratch_build = {};
  memrequirements_scratch_build.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
  getAccelerationStructureMemoryRequirementsKHR(device, &meminfo, &memrequirements_scratch_build);

  meminfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_UPDATE_SCRATCH_KHR;
  VkMemoryRequirements2 memrequirements_scratch_update = {};
  memrequirements_scratch_update.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
  getAccelerationStructureMemoryRequirementsKHR(device, &meminfo, &memrequirements_scratch_update);


  // Main backing memory:
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = memrequirements.memoryRequirements.size +
                    ezMath::Max(memrequirements_scratch_build.memoryRequirements.size, memrequirements_scratch_update.memoryRequirements.size);
  bufferInfo.usage = VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  assert(features_1_2.bufferDeviceAddress == VK_TRUE);
  bufferInfo.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
  bufferInfo.flags = 0;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

  res = vmaCreateBuffer(allocationhandler->allocator, &bufferInfo, &allocInfo, &internal_state->buffer, &internal_state->allocation, nullptr);
  assert(res == VK_SUCCESS);

  VkBindAccelerationStructureMemoryInfoKHR bind_info = {};
  bind_info.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_KHR;
  bind_info.accelerationStructure = internal_state->resource;
  bind_info.memory = internal_state->allocation->GetMemory();
  res = bindAccelerationStructureMemoryKHR(device, 1, &bind_info);
  assert(res == VK_SUCCESS);

  VkAccelerationStructureDeviceAddressInfoKHR addrinfo = {};
  addrinfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
  addrinfo.accelerationStructure = internal_state->resource;
  internal_state->as_address = getAccelerationStructureDeviceAddressKHR(device, &addrinfo);

  internal_state->scratch_offset = memrequirements.memoryRequirements.size;

  return res == VK_SUCCESS;
}
bool GraphicsDevice_Vulkan::CreateRaytracingPipelineState(const RaytracingPipelineStateDesc* pDesc, RaytracingPipelineState* rtpso)
{
  auto internal_state = EZ_DEFAULT_NEW(RTPipelineState_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  rtpso->internal_state = internal_state;

  rtpso->desc = *pDesc;

  VkRayTracingPipelineCreateInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
  info.flags = 0;

  info.libraries.sType = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;

  ezDynamicArray<VkPipelineShaderStageCreateInfo> stages;
  for (auto& x : pDesc->shaderlibraries)
  {
    auto& stage = stages.ExpandAndGetRef();
    stage = {};
    stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.module = to_internal(x.shader)->shaderModule;
    switch (x.type)
    {
      default:
      case ShaderLibrary::RAYGENERATION:
        stage.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        break;
      case ShaderLibrary::MISS:
        stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
        break;
      case ShaderLibrary::CLOSESTHIT:
        stage.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        break;
      case ShaderLibrary::ANYHIT:
        stage.stage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
        break;
      case ShaderLibrary::INTERSECTION:
        stage.stage = VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
        break;
    }
    stage.pName = x.function_name.GetData();
  }
  info.stageCount = stages.GetCount();
  info.pStages = stages.GetData();

  ezDynamicArray<VkRayTracingShaderGroupCreateInfoKHR> groups;
  groups.Reserve(pDesc->hitgroups.GetCount());
  for (auto& x : pDesc->hitgroups)
  {
    auto& group = groups.ExpandAndGetRef();
    group = {};
    group.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    switch (x.type)
    {
      default:
      case ShaderHitGroup::GENERAL:
        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        break;
      case ShaderHitGroup::TRIANGLES:
        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        break;
      case ShaderHitGroup::PROCEDURAL:
        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
        break;
    }
    group.generalShader = x.general_shader;
    group.closestHitShader = x.closesthit_shader;
    group.anyHitShader = x.anyhit_shader;
    group.intersectionShader = x.intersection_shader;
  }
  info.groupCount = groups.GetCount();
  info.pGroups = groups.GetData();

  info.maxRecursionDepth = pDesc->max_trace_recursion_depth;

  if (pDesc->rootSignature == nullptr)
  {
    //info.layout = to_internal(pDesc->shaderlibraries.front().shader)->pipelineLayout_cs; // think better way
    info.layout = to_internal(pDesc->shaderlibraries[0].shader)->pipelineLayout_cs; // ezTODO: do much better
  }
  else
  {
    info.layout = to_internal(pDesc->rootSignature)->pipelineLayout;
  }

  VkRayTracingPipelineInterfaceCreateInfoKHR library_interface = {};
  library_interface.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_INTERFACE_CREATE_INFO_KHR;
  library_interface.maxPayloadSize = pDesc->max_payload_size_in_bytes;
  library_interface.maxAttributeSize = pDesc->max_attribute_size_in_bytes;
  library_interface.maxCallableSize = 0;

  info.basePipelineHandle = VK_NULL_HANDLE;
  info.basePipelineIndex = 0;

  VkResult res = createRayTracingPipelinesKHR(device, VK_NULL_HANDLE, 1, &info, nullptr, &internal_state->pipeline);
  assert(res == VK_SUCCESS);

  return res == VK_SUCCESS;
}
bool GraphicsDevice_Vulkan::CreateDescriptorTable(DescriptorTable* table)
{
  auto internal_state = EZ_DEFAULT_NEW(DescriptorTable_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  table->internal_state = internal_state;

  ezDynamicArray<VkDescriptorSetLayoutBinding> bindings;
  bindings.Reserve(table->samplers.GetCount() + table->resources.GetCount() + table->staticsamplers.GetCount());

  ezDynamicArray<VkDescriptorUpdateTemplateEntry> entries;
  entries.Reserve(table->samplers.GetCount() + table->resources.GetCount());

  internal_state->descriptors.Reserve(table->samplers.GetCount() + table->resources.GetCount());

  size_t offset = 0;
  for (auto& x : table->resources)
  {
    auto& binding = bindings.ExpandAndGetRef();
    binding = {};
    binding.stageFlags = _ConvertStageFlags(table->stage);
    binding.descriptorCount = x.count;

    switch (x.binding)
    {
      case ROOT_CONSTANTBUFFER:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        binding.binding = x.slot + VULKAN_BINDING_SHIFT_B;
        break;
      case ROOT_RAWBUFFER:
      case ROOT_STRUCTUREDBUFFER:
      case ROOT_RWRAWBUFFER:
      case ROOT_RWSTRUCTUREDBUFFER:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        binding.binding = x.slot + VULKAN_BINDING_SHIFT_T;
        break;

      case CONSTANTBUFFER:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.binding = x.slot + VULKAN_BINDING_SHIFT_B;
        break;
      case RAWBUFFER:
      case STRUCTUREDBUFFER:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        binding.binding = x.slot + VULKAN_BINDING_SHIFT_T;
        break;
      case TYPEDBUFFER:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        binding.binding = x.slot + VULKAN_BINDING_SHIFT_T;
        break;
      case TEXTURE1D:
      case TEXTURE1DARRAY:
      case TEXTURE2D:
      case TEXTURE2DARRAY:
      case TEXTURECUBE:
      case TEXTURECUBEARRAY:
      case TEXTURE3D:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        binding.binding = x.slot + VULKAN_BINDING_SHIFT_T;
        break;
      case ACCELERATIONSTRUCTURE:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        binding.binding = x.slot + VULKAN_BINDING_SHIFT_T;
        break;
      case RWRAWBUFFER:
      case RWSTRUCTUREDBUFFER:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        binding.binding = x.slot + VULKAN_BINDING_SHIFT_U;
        break;
      case RWTYPEDBUFFER:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        binding.binding = x.slot + VULKAN_BINDING_SHIFT_U;
        break;
      case RWTEXTURE1D:
      case RWTEXTURE1DARRAY:
      case RWTEXTURE2D:
      case RWTEXTURE2DARRAY:
      case RWTEXTURE3D:
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        binding.binding = x.slot + VULKAN_BINDING_SHIFT_U;
        break;
      default:
        assert(0);
        break;
    }

    // Unroll, because we need the ability to update an array element individually:
    internal_state->resource_write_remap.PushBack(entries.GetCount());
    for (ezUInt32 i = 0; i < binding.descriptorCount; ++i)
    {
      internal_state->descriptors.ExpandAndGetRef();
      auto& entry = entries.ExpandAndGetRef();
      entry = {};
      entry.descriptorCount = 1;
      entry.descriptorType = binding.descriptorType;
      entry.dstArrayElement = i;
      entry.dstBinding = binding.binding;
      entry.offset = offset;
      entry.stride = sizeof(DescriptorTable_Vulkan::Descriptor);

      offset += entry.stride;
    }
  }
  for (auto& x : table->samplers)
  {
    internal_state->descriptors.ExpandAndGetRef();
    auto& binding = bindings.ExpandAndGetRef();
    binding = {};
    binding.stageFlags = _ConvertStageFlags(table->stage);
    binding.descriptorCount = x.count;
    binding.binding = x.slot + VULKAN_BINDING_SHIFT_S;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;

    // Unroll, because we need the ability to update an array element individually:
    internal_state->sampler_write_remap.PushBack(entries.GetCount());
    for (ezUInt32 i = 0; i < binding.descriptorCount; ++i)
    {
      auto& entry = entries.ExpandAndGetRef();
      entry = {};
      entry.descriptorCount = 1;
      entry.descriptorType = binding.descriptorType;
      entry.dstArrayElement = i;
      entry.dstBinding = binding.binding;
      entry.offset = offset;
      entry.stride = sizeof(DescriptorTable_Vulkan::Descriptor);

      offset += entry.stride;
    }
  }

  ezDynamicArray<VkSampler> immutableSamplers;
  immutableSamplers.Reserve(table->staticsamplers.GetCount());

  for (auto& x : table->staticsamplers)
  {
    auto& immutablesampler = immutableSamplers.ExpandAndGetRef();
    immutablesampler = to_internal(&x.sampler)->resource;

    auto& binding = bindings.ExpandAndGetRef();
    binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    binding.stageFlags = VK_SHADER_STAGE_ALL;
    binding.binding = x.slot + VULKAN_BINDING_SHIFT_S;
    binding.descriptorCount = 1;
    binding.pImmutableSamplers = &immutablesampler;
  }

  VkDescriptorSetLayoutCreateInfo layoutinfo = {};
  layoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutinfo.flags = 0;
  layoutinfo.pBindings = bindings.GetData();
  layoutinfo.bindingCount = bindings.GetCount();
  VkResult res = vkCreateDescriptorSetLayout(device, &layoutinfo, nullptr, &internal_state->layout);
  assert(res == VK_SUCCESS);

  VkDescriptorUpdateTemplateCreateInfo updatetemplateinfo = {};
  updatetemplateinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO;
  updatetemplateinfo.templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_DESCRIPTOR_SET;
  updatetemplateinfo.flags = 0;
  updatetemplateinfo.descriptorSetLayout = internal_state->layout;
  updatetemplateinfo.pDescriptorUpdateEntries = entries.GetData();
  updatetemplateinfo.descriptorUpdateEntryCount = (ezUInt32)entries.GetCount();
  res = vkCreateDescriptorUpdateTemplate(device, &updatetemplateinfo, nullptr, &internal_state->updatetemplate);
  assert(res == VK_SUCCESS);

  ezUInt32 slot = 0;
  for (auto& x : table->resources)
  {
    for (ezUInt32 i = 0; i < x.count; ++i)
    {
      WriteDescriptor(table, slot, i, (const GPUResource*)nullptr);
    }
    slot++;
  }
  slot = 0;
  for (auto& x : table->samplers)
  {
    for (ezUInt32 i = 0; i < x.count; ++i)
    {
      WriteDescriptor(table, slot, i, (const Sampler*)nullptr);
    }
    slot++;
  }

  return res == VK_SUCCESS;
}
bool GraphicsDevice_Vulkan::CreateRootSignature(RootSignature* rootsig)
{
  auto internal_state = EZ_DEFAULT_NEW(RootSignature_Vulkan);
  internal_state->allocationhandler = allocationhandler;
  rootsig->internal_state = internal_state;

  ezDynamicArray<VkDescriptorSetLayout> layouts;
  layouts.Reserve(rootsig->tables.GetCount());
  ezUInt32 space = 0;
  for (auto& x : rootsig->tables)
  {
    layouts.PushBack(to_internal(&x)->layout);

    ezUInt32 rangeIndex = 0;
    for (auto& binding : x.resources)
    {
      if (binding.binding < CONSTANTBUFFER)
      {
        assert(binding.count == 1); // descriptor array not allowed in the root
        internal_state->root_remap.ExpandAndGetRef();
        internal_state->root_remap.PeekBack().space = space;
        internal_state->root_remap.PeekBack().binding = binding.slot;
        internal_state->root_remap.PeekBack().rangeIndex = rangeIndex;
      }
      rangeIndex++;
    }
    space++;
  }

  for (CommandList cmd = 0; cmd < COMMANDLIST_COUNT; ++cmd)
  {
    internal_state->last_tables[cmd].SetCount(layouts.GetCount());
    internal_state->last_descriptorsets[cmd].SetCount(layouts.GetCount());

    for (auto& x : rootsig->tables)
    {
      for (auto& binding : x.resources)
      {
        if (binding.binding < CONSTANTBUFFER)
        {
          internal_state->root_descriptors[cmd].ExpandAndGetRef();
          internal_state->root_offsets[cmd].ExpandAndGetRef();
        }
      }
    }
  }

  ezDynamicArray<VkPushConstantRange> pushranges;
  pushranges.Reserve(rootsig->rootconstants.GetCount());
  for (auto& x : rootsig->rootconstants)
  {
    // ez on
    auto& pushrange = pushranges.ExpandAndGetRef();
    pushrange = {};
    pushrange.stageFlags = _ConvertStageFlags(x.stage);
    pushrange.offset = 0;
    pushrange.size = x.size;
    // ez off
  }

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.pSetLayouts = layouts.GetData();
  pipelineLayoutInfo.setLayoutCount = layouts.GetCount();
  pipelineLayoutInfo.pPushConstantRanges = pushranges.GetData();
  pipelineLayoutInfo.pushConstantRangeCount = (ezUInt32)pushranges.GetCount();

  VkResult res = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &internal_state->pipelineLayout);
  assert(res == VK_SUCCESS);

  return res == VK_SUCCESS;
}

int GraphicsDevice_Vulkan::CreateSubresource(Texture* texture, SUBRESOURCE_TYPE type, ezUInt32 firstSlice, ezUInt32 sliceCount, ezUInt32 firstMip, ezUInt32 mipCount)
{
  auto internal_state = to_internal(texture);

  VkImageViewCreateInfo view_desc = {};
  view_desc.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_desc.flags = 0;
  view_desc.image = internal_state->resource;
  view_desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  view_desc.subresourceRange.baseArrayLayer = firstSlice;
  view_desc.subresourceRange.layerCount = sliceCount;
  view_desc.subresourceRange.baseMipLevel = firstMip;
  view_desc.subresourceRange.levelCount = mipCount;
  view_desc.format = _ConvertFormat(texture->desc.Format);

  if (texture->desc.type == TextureDesc::TEXTURE_1D)
  {
    if (texture->desc.ArraySize > 1)
    {
      view_desc.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    }
    else
    {
      view_desc.viewType = VK_IMAGE_VIEW_TYPE_1D;
    }
  }
  else if (texture->desc.type == TextureDesc::TEXTURE_2D)
  {
    if (texture->desc.ArraySize > 1)
    {
      if (texture->desc.MiscFlags & RESOURCE_MISC_TEXTURECUBE)
      {
        if (texture->desc.ArraySize > 6 && sliceCount > 6)
        {
          view_desc.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        }
        else
        {
          view_desc.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        }
      }
      else
      {
        view_desc.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
      }
    }
    else
    {
      view_desc.viewType = VK_IMAGE_VIEW_TYPE_2D;
    }
  }
  else if (texture->desc.type == TextureDesc::TEXTURE_3D)
  {
    view_desc.viewType = VK_IMAGE_VIEW_TYPE_3D;
  }

  switch (type)
  {
    case SRV:
    {
      switch (texture->desc.Format)
      {
        case ezRHIFormat::R16_TYPELESS:
          view_desc.format = VK_FORMAT_D16_UNORM;
          view_desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
          break;
        case ezRHIFormat::R32_TYPELESS:
          view_desc.format = VK_FORMAT_D32_SFLOAT;
          view_desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
          break;
        case ezRHIFormat::R24G8_TYPELESS:
          view_desc.format = VK_FORMAT_D24_UNORM_S8_UINT;
          view_desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
          break;
        case ezRHIFormat::R32G8X24_TYPELESS:
          view_desc.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
          view_desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
          break;
      }

      VkImageView srv;
      VkResult res = vkCreateImageView(device, &view_desc, nullptr, &srv);

      if (res == VK_SUCCESS)
      {
        if (internal_state->srv == VK_NULL_HANDLE)
        {
          internal_state->srv = srv;
          return -1;
        }
        internal_state->subresources_srv.PushBack(srv);
        return int(internal_state->subresources_srv.GetCount() - 1);
      }
      else
      {
        assert(0);
      }
    }
    break;
    case UAV:
    {
      if (view_desc.viewType == VK_IMAGE_VIEW_TYPE_CUBE || view_desc.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY)
      {
        view_desc.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
      }

      VkImageView uav;
      VkResult res = vkCreateImageView(device, &view_desc, nullptr, &uav);

      if (res == VK_SUCCESS)
      {
        if (internal_state->uav == VK_NULL_HANDLE)
        {
          internal_state->uav = uav;
          return -1;
        }
        internal_state->subresources_uav.PushBack(uav);
        return int(internal_state->subresources_uav.GetCount() - 1);
      }
      else
      {
        assert(0);
      }
    }
    break;
    case RTV:
    {
      VkImageView rtv;
      view_desc.subresourceRange.levelCount = 1;
      VkResult res = vkCreateImageView(device, &view_desc, nullptr, &rtv);

      if (res == VK_SUCCESS)
      {
        if (internal_state->rtv == VK_NULL_HANDLE)
        {
          internal_state->rtv = rtv;
          return -1;
        }
        internal_state->subresources_rtv.PushBack(rtv);
        return int(internal_state->subresources_rtv.GetCount() - 1);
      }
      else
      {
        assert(0);
      }
    }
    break;
    case DSV:
    {
      view_desc.subresourceRange.levelCount = 1;
      view_desc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

      switch (texture->desc.Format)
      {
        case ezRHIFormat::R16_TYPELESS:
          view_desc.format = VK_FORMAT_D16_UNORM;
          break;
        case ezRHIFormat::R32_TYPELESS:
          view_desc.format = VK_FORMAT_D32_SFLOAT;
          break;
        case ezRHIFormat::R24G8_TYPELESS:
          view_desc.format = VK_FORMAT_D24_UNORM_S8_UINT;
          view_desc.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
          break;
        case ezRHIFormat::R32G8X24_TYPELESS:
          view_desc.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
          view_desc.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
          break;
      }

      VkImageView dsv;
      VkResult res = vkCreateImageView(device, &view_desc, nullptr, &dsv);

      if (res == VK_SUCCESS)
      {
        if (internal_state->dsv == VK_NULL_HANDLE)
        {
          internal_state->dsv = dsv;
          return -1;
        }
        internal_state->subresources_dsv.PushBack(dsv);
        return int(internal_state->subresources_dsv.GetCount() - 1);
      }
      else
      {
        assert(0);
      }
    }
    break;
    default:
      break;
  }
  return -1;
}
int GraphicsDevice_Vulkan::CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, ezUInt64 offset, ezUInt64 size)
{
  auto internal_state = to_internal(buffer);
  const GPUBufferDesc& desc = buffer->GetDesc();
  VkResult res;

  switch (type)
  {
    case SRV:
    case UAV:
    {
      if (desc.Format == ezRHIFormat::Unknown)
      {
        return -1;
      }

      VkBufferViewCreateInfo srv_desc = {};
      srv_desc.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
      srv_desc.buffer = internal_state->resource;
      srv_desc.flags = 0;
      srv_desc.format = _ConvertFormat(desc.Format);
      srv_desc.offset = Align(offset, device_properties.properties.limits.minTexelBufferOffsetAlignment); // damn, if this needs alignment, that could break a lot of things! (index buffer, index offset?)
      srv_desc.range = ezMath::Min(size, (ezUInt64)desc.ByteWidth - srv_desc.offset);

      VkBufferView view;
      res = vkCreateBufferView(device, &srv_desc, nullptr, &view);

      if (res == VK_SUCCESS)
      {
        if (type == SRV)
        {
          if (internal_state->srv == VK_NULL_HANDLE)
          {
            internal_state->srv = view;
            return -1;
          }
          internal_state->subresources_srv.PushBack(view);
          return int(internal_state->subresources_srv.GetCount() - 1);
        }
        else
        {
          if (internal_state->uav == VK_NULL_HANDLE)
          {
            internal_state->uav = view;
            return -1;
          }
          internal_state->subresources_uav.PushBack(view);
          return int(internal_state->subresources_uav.GetCount() - 1);
        }
      }
      else
      {
        assert(0);
      }
    }
    break;
    default:
      assert(0);
      break;
  }
  return -1;
}

void GraphicsDevice_Vulkan::WriteTopLevelAccelerationStructureInstance(const RaytracingAccelerationStructureDesc::TopLevel::Instance* instance, void* dest)
{
  VkAccelerationStructureInstanceKHR* desc = (VkAccelerationStructureInstanceKHR*)dest;
  memcpy(&desc->transform, &instance->transform, sizeof(desc->transform));
  desc->instanceCustomIndex = instance->InstanceID;
  desc->mask = instance->InstanceMask;
  desc->instanceShaderBindingTableRecordOffset = instance->InstanceContributionToHitGroupIndex;
  desc->flags = instance->Flags;

  assert(instance->bottomlevel.IsAccelerationStructure());
  auto internal_state = to_internal((RaytracingAccelerationStructure*)&instance->bottomlevel);
  desc->accelerationStructureReference = internal_state->as_address;
}
void GraphicsDevice_Vulkan::WriteShaderIdentifier(const RaytracingPipelineState* rtpso, ezUInt32 group_index, void* dest)
{
  VkResult res = getRayTracingShaderGroupHandlesKHR(device, to_internal(rtpso)->pipeline, group_index, 1, SHADER_IDENTIFIER_SIZE, dest);
  assert(res == VK_SUCCESS);
}
void GraphicsDevice_Vulkan::WriteDescriptor(const DescriptorTable* table, ezUInt32 rangeIndex, ezUInt32 arrayIndex, const GPUResource* resource, int subresource, ezUInt64 offset)
{
  auto table_internal = to_internal(table);
  size_t remap = table_internal->resource_write_remap[rangeIndex];
  auto& descriptor = table_internal->descriptors[remap + arrayIndex];

  switch (table->resources[rangeIndex].binding)
  {
    case CONSTANTBUFFER:
    case RAWBUFFER:
    case STRUCTUREDBUFFER:
    case ROOT_CONSTANTBUFFER:
    case ROOT_RAWBUFFER:
    case ROOT_STRUCTUREDBUFFER:
      if (resource == nullptr || !resource->IsValid())
      {
        descriptor.bufferInfo.buffer = nullBuffer;
        descriptor.bufferInfo.offset = 0;
        descriptor.bufferInfo.range = VK_WHOLE_SIZE;
      }
      else if (resource->IsBuffer())
      {
        const GPUBuffer* buffer = (const GPUBuffer*)resource;
        auto internal_state = to_internal(buffer);
        descriptor.bufferInfo.buffer = internal_state->resource;
        descriptor.bufferInfo.offset = offset;
        descriptor.bufferInfo.range = VK_WHOLE_SIZE;
      }
      else
      {
        assert(0);
      }
      break;
    case TYPEDBUFFER:
      if (resource == nullptr || !resource->IsValid())
      {
        descriptor.bufferView = nullBufferView;
      }
      else if (resource->IsBuffer())
      {
        const GPUBuffer* buffer = (const GPUBuffer*)resource;
        auto internal_state = to_internal(buffer);
        descriptor.bufferView = subresource < 0 ? internal_state->srv : internal_state->subresources_srv[subresource];
      }
      else
      {
        assert(0);
      }
      break;
    case TEXTURE1D:
    case TEXTURE1DARRAY:
    case TEXTURE2D:
    case TEXTURE2DARRAY:
    case TEXTURECUBE:
    case TEXTURECUBEARRAY:
    case TEXTURE3D:
      descriptor.imageinfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
      descriptor.imageinfo.sampler = VK_NULL_HANDLE;
      if (resource == nullptr || !resource->IsValid())
      {
        switch (table->resources[rangeIndex].binding)
        {
          case TEXTURE1D:
            descriptor.imageinfo.imageView = nullImageView1D;
            break;
          case TEXTURE1DARRAY:
            descriptor.imageinfo.imageView = nullImageView1DArray;
            break;
          case TEXTURE2D:
            descriptor.imageinfo.imageView = nullImageView2D;
            break;
          case TEXTURE2DARRAY:
            descriptor.imageinfo.imageView = nullImageView2DArray;
            break;
          case TEXTURECUBE:
            descriptor.imageinfo.imageView = nullImageViewCube;
            break;
          case TEXTURECUBEARRAY:
            descriptor.imageinfo.imageView = nullImageViewCubeArray;
            break;
          case TEXTURE3D:
            descriptor.imageinfo.imageView = nullImageView3D;
            break;
        };
      }
      else if (resource->IsTexture())
      {
        const Texture* texture = (const Texture*)resource;
        auto internal_state = to_internal(texture);
        if (subresource < 0)
        {
          descriptor.imageinfo.imageView = internal_state->srv;
        }
        else
        {
          descriptor.imageinfo.imageView = internal_state->subresources_srv[subresource];
        }
        VkImageLayout layout = _ConvertImageLayout(texture->desc.layout);
        if (layout != VK_IMAGE_LAYOUT_GENERAL && layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
          // Means texture initial layout is not compatible, so it must have been transitioned
          layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        descriptor.imageinfo.imageLayout = layout;
      }
      else
      {
        assert(0);
      }
      break;
    case ACCELERATIONSTRUCTURE:
      if (resource == nullptr || !resource->IsValid())
      {
        // nothing
      }
      else if (resource->IsAccelerationStructure())
      {
        auto internal_state = to_internal((const RaytracingAccelerationStructure*)resource);
        descriptor.accelerationStructure = internal_state->resource;
      }
      else
      {
        assert(0);
      }
      break;
    case RWRAWBUFFER:
    case RWSTRUCTUREDBUFFER:
    case ROOT_RWRAWBUFFER:
    case ROOT_RWSTRUCTUREDBUFFER:
      if (resource == nullptr || !resource->IsValid())
      {
        descriptor.bufferInfo.buffer = nullBuffer;
        descriptor.bufferInfo.offset = 0;
        descriptor.bufferInfo.range = VK_WHOLE_SIZE;
      }
      else if (resource->IsBuffer())
      {
        const GPUBuffer* buffer = (const GPUBuffer*)resource;
        auto internal_state = to_internal(buffer);
        descriptor.bufferInfo.buffer = internal_state->resource;
        descriptor.bufferInfo.offset = offset;
        descriptor.bufferInfo.range = VK_WHOLE_SIZE;
      }
      else
      {
        assert(0);
      }
      break;
    case RWTYPEDBUFFER:
      if (resource == nullptr || !resource->IsValid())
      {
        descriptor.bufferView = nullBufferView;
      }
      else if (resource->IsBuffer())
      {
        const GPUBuffer* buffer = (const GPUBuffer*)resource;
        auto internal_state = to_internal(buffer);
        descriptor.bufferView = subresource < 0 ? internal_state->uav : internal_state->subresources_uav[subresource];
      }
      else
      {
        assert(0);
      }
      break;
    case RWTEXTURE1D:
    case RWTEXTURE1DARRAY:
    case RWTEXTURE2D:
    case RWTEXTURE2DARRAY:
    case RWTEXTURE3D:
      descriptor.imageinfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
      descriptor.imageinfo.sampler = VK_NULL_HANDLE;
      if (resource == nullptr || !resource->IsValid())
      {
        switch (table->resources[rangeIndex].binding)
        {
          case TEXTURE1D:
            descriptor.imageinfo.imageView = nullImageView1D;
            break;
          case TEXTURE1DARRAY:
            descriptor.imageinfo.imageView = nullImageView1DArray;
            break;
          case TEXTURE2D:
            descriptor.imageinfo.imageView = nullImageView2D;
            break;
          case TEXTURE2DARRAY:
            descriptor.imageinfo.imageView = nullImageView2DArray;
            break;
          case TEXTURECUBE:
            descriptor.imageinfo.imageView = nullImageViewCube;
            break;
          case TEXTURECUBEARRAY:
            descriptor.imageinfo.imageView = nullImageViewCubeArray;
            break;
          case TEXTURE3D:
            descriptor.imageinfo.imageView = nullImageView3D;
            break;
        };
      }
      else if (resource->IsTexture())
      {
        const Texture* texture = (const Texture*)resource;
        auto internal_state = to_internal(texture);
        if (subresource < 0)
        {
          descriptor.imageinfo.imageView = internal_state->uav;
        }
        else
        {
          descriptor.imageinfo.imageView = internal_state->subresources_uav[subresource];
        }
      }
      else
      {
        assert(0);
      }
      break;
    default:
      break;
  }
}
void GraphicsDevice_Vulkan::WriteDescriptor(const DescriptorTable* table, ezUInt32 rangeIndex, ezUInt32 arrayIndex, const Sampler* sampler)
{
  auto table_internal = to_internal(table);
  size_t sampler_remap = table->resources.GetCount() + (size_t)rangeIndex;
  size_t remap = table_internal->sampler_write_remap[rangeIndex];
  auto& descriptor = table_internal->descriptors[remap];
  descriptor.imageinfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  descriptor.imageinfo.imageView = VK_NULL_HANDLE;

  if (sampler == nullptr || !sampler->IsValid())
  {
    descriptor.imageinfo.sampler = nullSampler;
  }
  else
  {
    auto internal_state = to_internal(sampler);
    descriptor.imageinfo.sampler = internal_state->resource;
  }
}

void GraphicsDevice_Vulkan::Map(const GPUResource* resource, Mapping* mapping)
{
  VkDeviceMemory memory = VK_NULL_HANDLE;

  if (resource->type == GPUResource::GPU_RESOURCE_TYPE::BUFFER)
  {
    const GPUBuffer* buffer = (const GPUBuffer*)resource;
    auto internal_state = to_internal(buffer);
    memory = internal_state->allocation->GetMemory();
    mapping->rowpitch = (ezUInt32)buffer->desc.ByteWidth;
  }
  else if (resource->type == GPUResource::GPU_RESOURCE_TYPE::TEXTURE)
  {
    const Texture* texture = (const Texture*)resource;
    auto internal_state = to_internal(texture);
    memory = internal_state->allocation->GetMemory();
    mapping->rowpitch = (ezUInt32)internal_state->subresourcelayout.rowPitch;
  }
  else
  {
    assert(0);
    return;
  }

  VkDeviceSize offset = mapping->offset;
  VkDeviceSize size = mapping->size;

  VkResult res = vkMapMemory(device, memory, offset, size, 0, &mapping->data);
  if (res != VK_SUCCESS)
  {
    assert(0);
    mapping->data = nullptr;
    mapping->rowpitch = 0;
  }
}
void GraphicsDevice_Vulkan::Unmap(const GPUResource* resource)
{
  if (resource->type == GPUResource::GPU_RESOURCE_TYPE::BUFFER)
  {
    const GPUBuffer* buffer = (const GPUBuffer*)resource;
    auto internal_state = to_internal(buffer);
    vkUnmapMemory(device, internal_state->allocation->GetMemory());
  }
  else if (resource->type == GPUResource::GPU_RESOURCE_TYPE::TEXTURE)
  {
    const Texture* texture = (const Texture*)resource;
    auto internal_state = to_internal(texture);
    vkUnmapMemory(device, internal_state->allocation->GetMemory());
  }
}
bool GraphicsDevice_Vulkan::QueryRead(const GPUQuery* query, GPUQueryResult* result)
{
  auto internal_state = to_internal(query);

  VkResult res = VK_SUCCESS;

  switch (internal_state->query_type)
  {
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_EVENT:
      assert(0); // not implemented yet
      break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP:
      res = vkGetQueryPoolResults(device, querypool_timestamp, (ezUInt32)internal_state->query_index, 1, sizeof(ezUInt64),
        &result->result_timestamp, sizeof(ezUInt64), VK_QUERY_RESULT_64_BIT);
      if (timestamps_to_reset.IsEmpty() || timestamps_to_reset.PeekBack() != internal_state->query_index)
      {
        timestamps_to_reset.PushBack(internal_state->query_index);
      }
      break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP_DISJOINT:
      result->result_timestamp_frequency = timestamp_frequency;
      break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION:
      res = vkGetQueryPoolResults(device, querypool_occlusion, (ezUInt32)internal_state->query_index, 1, sizeof(ezUInt64),
        &result->result_passed_sample_count, sizeof(ezUInt64), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_PARTIAL_BIT);
      if (occlusions_to_reset.IsEmpty() || occlusions_to_reset.PeekBack() != internal_state->query_index)
      {
        occlusions_to_reset.PushBack((ezUInt32)internal_state->query_index);
      }
      break;
    default:
      return false;
      break;
  }

  return res == VK_SUCCESS;
}

void GraphicsDevice_Vulkan::SetName(GPUResource* pResource, const char* name)
{
  VkDebugUtilsObjectNameInfoEXT info = {};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
  info.pObjectName = name;
  if (pResource->IsTexture())
  {
    info.objectType = VK_OBJECT_TYPE_IMAGE;
    info.objectHandle = (ezUInt64)to_internal((const Texture*)pResource)->resource;
  }
  else if (pResource->IsBuffer())
  {
    info.objectType = VK_OBJECT_TYPE_BUFFER;
    info.objectHandle = (ezUInt64)to_internal((const GPUBuffer*)pResource)->resource;
  }
  else if (pResource->IsAccelerationStructure())
  {
    info.objectType = VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR;
    info.objectHandle = (ezUInt64)to_internal((const RaytracingAccelerationStructure*)pResource)->resource;
  }

  if (info.objectHandle == VK_NULL_HANDLE)
  {
    return;
  }

  VkResult res = setDebugUtilsObjectNameEXT(device, &info);
  assert(res == VK_SUCCESS);
}

void GraphicsDevice_Vulkan::PresentBegin(CommandList cmd)
{
  VkSemaphore acquireSemaphore = GetFrameResources().swapchainAcquireSemaphore;

  VkResult res = vkAcquireNextImageKHR(device, swapChain, 0xFFFFFFFFFFFFFFFF, acquireSemaphore, VK_NULL_HANDLE, &swapChainImageIndex);
  if (res != VK_SUCCESS)
  {
    // Handle outdated error in acquire.
    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
    {
      CreateBackBufferResources();
      PresentBegin(cmd);
      return;
    }
  }

  VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
  VkRenderPassBeginInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = defaultRenderPass;
  renderPassInfo.framebuffer = swapChainFramebuffers[swapChainImageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChainExtent;
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;
  vkCmdBeginRenderPass(GetDirectCommandList(cmd), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
void GraphicsDevice_Vulkan::PresentEnd(CommandList cmd)
{
  vkCmdEndRenderPass(GetDirectCommandList(cmd));

  SubmitCommandLists();

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  VkSemaphore signalSemaphores[] = {frames[swapChainImageIndex].swapchainReleaseSemaphore};
  presentInfo.waitSemaphoreCount = EZ_ARRAY_SIZE(signalSemaphores);
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &swapChainImageIndex;
  presentInfo.pResults = nullptr; // Optional

  VkResult res = vkQueuePresentKHR(presentQueue, &presentInfo);
  assert(res == VK_SUCCESS);
}

CommandList GraphicsDevice_Vulkan::BeginCommandList()
{
  VkResult res;

  CommandList cmd = cmd_count.fetch_add(1);
  if (GetDirectCommandList(cmd) == VK_NULL_HANDLE)
  {
    // need to create one more command list:
    assert(cmd < COMMANDLIST_COUNT);

    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

    for (auto& frame : frames)
    {
      VkCommandPoolCreateInfo poolInfo = {};
      poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
      poolInfo.flags = 0; // Optional

      res = vkCreateCommandPool(device, &poolInfo, nullptr, &frame.commandPools[cmd]);
      assert(res == VK_SUCCESS);

      VkCommandBufferAllocateInfo commandBufferInfo = {};
      commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      commandBufferInfo.commandBufferCount = 1;
      commandBufferInfo.commandPool = frame.commandPools[cmd];
      commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

      res = vkAllocateCommandBuffers(device, &commandBufferInfo, &frame.commandBuffers[cmd]);
      assert(res == VK_SUCCESS);

      frame.resourceBuffer[cmd].init(this, 1024 * 1024); // 1 MB starting size
      frame.descriptors[cmd].init(this);
    }
  }
  res = vkResetCommandPool(device, GetFrameResources().commandPools[cmd], 0);
  assert(res == VK_SUCCESS);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  beginInfo.pInheritanceInfo = nullptr; // Optional

  res = vkBeginCommandBuffer(GetFrameResources().commandBuffers[cmd], &beginInfo);
  assert(res == VK_SUCCESS);

  VkViewport viewports[6];
  for (ezUInt32 i = 0; i < EZ_ARRAY_SIZE(viewports); ++i)
  {
    viewports[i].x = 0;
    viewports[i].y = 0;
    viewports[i].width = (float)RESOLUTIONWIDTH;
    viewports[i].height = (float)RESOLUTIONHEIGHT;
    viewports[i].minDepth = 0;
    viewports[i].maxDepth = 1;
  }
  vkCmdSetViewport(GetDirectCommandList(cmd), 0, EZ_ARRAY_SIZE(viewports), viewports);

  VkRect2D scissors[8];
  for (int i = 0; i < EZ_ARRAY_SIZE(scissors); ++i)
  {
    scissors[i].offset.x = 0;
    scissors[i].offset.y = 0;
    scissors[i].extent.width = 65535;
    scissors[i].extent.height = 65535;
  }
  vkCmdSetScissor(GetDirectCommandList(cmd), 0, EZ_ARRAY_SIZE(scissors), scissors);

  float blendConstants[] = {1, 1, 1, 1};
  vkCmdSetBlendConstants(GetDirectCommandList(cmd), blendConstants);

  // reset descriptor allocators:
  GetFrameResources().descriptors[cmd].reset();

  // reset immediate resource allocators:
  GetFrameResources().resourceBuffer[cmd].clear();

  if (!initial_querypool_reset)
  {
    initial_querypool_reset = true;
    vkCmdResetQueryPool(GetDirectCommandList(cmd), querypool_timestamp, 0, timestamp_query_count);
    vkCmdResetQueryPool(GetDirectCommandList(cmd), querypool_occlusion, 0, occlusion_query_count);
  }
  for (auto& x : timestamps_to_reset)
  {
    vkCmdResetQueryPool(GetDirectCommandList(cmd), querypool_timestamp, x, 1);
  }
  timestamps_to_reset.Clear();
  for (auto& x : occlusions_to_reset)
  {
    vkCmdResetQueryPool(GetDirectCommandList(cmd), querypool_occlusion, x, 1);
  }
  occlusions_to_reset.Clear();

  prev_pipeline_hash[cmd] = 0;
  active_pso[cmd] = nullptr;
  active_cs[cmd] = nullptr;
  active_rt[cmd] = nullptr;
  active_renderpass[cmd] = VK_NULL_HANDLE;
  dirty_pso[cmd] = false;

  return cmd;
}
void GraphicsDevice_Vulkan::SubmitCommandLists()
{
  // Sync up copy queue and transitions:
  ezLock lock(copyQueueMutex);
  if (copyQueueUse)
  {
    auto& frame = GetFrameResources();

    // Copies:
    {
      VkResult res = vkEndCommandBuffer(frame.copyCommandBuffer);
      assert(res == VK_SUCCESS);

      VkSemaphore semaphores[] = {copySemaphore};

      VkSubmitInfo submitInfo = {};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &frame.copyCommandBuffer;
      submitInfo.pSignalSemaphores = semaphores;
      submitInfo.signalSemaphoreCount = EZ_ARRAY_SIZE(semaphores);

      res = vkQueueSubmit(frame.copyQueue, 1, &submitInfo, VK_NULL_HANDLE);
      assert(res == VK_SUCCESS);
    }

    // Transitions:
    {
      for (auto& barrier : frame.loadedimagetransitions)
      {
        vkCmdPipelineBarrier(
          frame.transitionCommandBuffer,
          VK_PIPELINE_STAGE_TRANSFER_BIT,
          VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
          0,
          0, nullptr,
          0, nullptr,
          1, &barrier);
      }
      frame.loadedimagetransitions.Clear();

      VkResult res = vkEndCommandBuffer(frame.transitionCommandBuffer);
      assert(res == VK_SUCCESS);

      VkSubmitInfo submitInfo = {};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &frame.transitionCommandBuffer;

      res = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
      assert(res == VK_SUCCESS);
    }
  }

  // Execute deferred command lists:
  {
    auto& frame = GetFrameResources();

    VkCommandBuffer cmdLists[COMMANDLIST_COUNT];
    CommandList cmds[COMMANDLIST_COUNT];
    ezUInt32 counter = 0;

    CommandList cmd_last = cmd_count.load();
    cmd_count.store(0);
    for (CommandList cmd = 0; cmd < cmd_last; ++cmd)
    {
      VkResult res = vkEndCommandBuffer(GetDirectCommandList(cmd));
      assert(res == VK_SUCCESS);

      cmdLists[counter] = GetDirectCommandList(cmd);
      cmds[counter] = cmd;
      counter++;

      for (auto& x : pipelines_worker[cmd])
      {
        if (pipelines_global.count(x.first) == 0)
        {
          pipelines_global[x.first] = x.second;
        }
        else
        {
          ezLock lock(allocationhandler->destroylocker);
          allocationhandler->destroyer_pipelines.PushBack(std::make_pair(x.second, FRAMECOUNT));
          allocationhandler->destroylocker.Unlock();
        }
      }
      pipelines_worker[cmd].Clear();
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {frame.swapchainAcquireSemaphore, copySemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT};
    if (copyQueueUse)
    {
      submitInfo.waitSemaphoreCount = 2;
    }
    else
    {
      submitInfo.waitSemaphoreCount = 1;
    }
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = counter;
    submitInfo.pCommandBuffers = cmdLists;

    VkSemaphore signalSemaphores[] = {frame.swapchainReleaseSemaphore};
    submitInfo.signalSemaphoreCount = EZ_ARRAY_SIZE(signalSemaphores);
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkResult res = vkQueueSubmit(graphicsQueue, 1, &submitInfo, frame.frameFence);
    assert(res == VK_SUCCESS);
  }

  // This acts as a barrier, following this we will be using the next frame's resources when calling GetFrameResources()!
  FRAMECOUNT++;

  // Initiate stalling CPU when GPU is behind by more frames than would fit in the backbuffers:
  if (FRAMECOUNT >= BACKBUFFER_COUNT)
  {
    VkResult res = vkWaitForFences(device, 1, &GetFrameResources().frameFence, true, 0xFFFFFFFFFFFFFFFF);
    assert(res == VK_SUCCESS);

    res = vkResetFences(device, 1, &GetFrameResources().frameFence);
    assert(res == VK_SUCCESS);
  }

  allocationhandler->Update(FRAMECOUNT, BACKBUFFER_COUNT);

  // Restart transition command buffers:
  {
    auto& frame = GetFrameResources();

    VkResult res = vkResetCommandPool(device, frame.transitionCommandPool, 0);
    assert(res == VK_SUCCESS);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr; // Optional

    res = vkBeginCommandBuffer(frame.transitionCommandBuffer, &beginInfo);
    assert(res == VK_SUCCESS);
  }

  copyQueueUse = false;
  copyQueueMutex.Unlock();
}

void GraphicsDevice_Vulkan::WaitForGPU()
{
  VkResult res = vkQueueWaitIdle(graphicsQueue);
  assert(res == VK_SUCCESS);
}
void GraphicsDevice_Vulkan::ClearPipelineStateCache()
{
  ezLock lock(allocationhandler->destroylocker);
  for (auto& x : pipelines_global)
  {
    allocationhandler->destroyer_pipelines.PushBack(std::make_pair(x.second, FRAMECOUNT));
  }
  pipelines_global.clear();

  for (int i = 0; i < EZ_ARRAY_SIZE(pipelines_worker); ++i)
  {
    for (auto& x : pipelines_worker[i])
    {
      allocationhandler->destroyer_pipelines.PushBack(std::make_pair(x.second, FRAMECOUNT));
    }
    pipelines_worker[i].Clear();
  }
  allocationhandler->destroylocker.Unlock();
}


void GraphicsDevice_Vulkan::RenderPassBegin(const RenderPass* renderpass, CommandList cmd)
{
  active_renderpass[cmd] = renderpass;

  auto internal_state = to_internal(renderpass);
  vkCmdBeginRenderPass(GetDirectCommandList(cmd), &internal_state->beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}
void GraphicsDevice_Vulkan::RenderPassEnd(CommandList cmd)
{
  vkCmdEndRenderPass(GetDirectCommandList(cmd));

  active_renderpass[cmd] = VK_NULL_HANDLE;
}
void GraphicsDevice_Vulkan::BindScissorRects(ezUInt32 numRects, const Rect* rects, CommandList cmd)
{
  assert(rects != nullptr);
  assert(numRects <= 8);
  VkRect2D scissors[8];
  for (ezUInt32 i = 0; i < numRects; ++i)
  {
    scissors[i].extent.width = abs(rects[i].right - rects[i].left);
    scissors[i].extent.height = abs(rects[i].top - rects[i].bottom);
    scissors[i].offset.x = ezMath::Max(0, rects[i].left);
    scissors[i].offset.y = ezMath::Max(0, rects[i].top);
  }
  vkCmdSetScissor(GetDirectCommandList(cmd), 0, numRects, scissors);
}
void GraphicsDevice_Vulkan::BindViewports(ezUInt32 NumViewports, const Viewport* pViewports, CommandList cmd)
{
  assert(NumViewports <= 6);
  VkViewport viewports[6];
  for (ezUInt32 i = 0; i < NumViewports; ++i)
  {
    viewports[i].x = pViewports[i].TopLeftX;
    viewports[i].y = pViewports[i].TopLeftY;
    viewports[i].width = pViewports[i].Width;
    viewports[i].height = pViewports[i].Height;
    viewports[i].minDepth = pViewports[i].MinDepth;
    viewports[i].maxDepth = pViewports[i].MaxDepth;
  }
  vkCmdSetViewport(GetDirectCommandList(cmd), 0, NumViewports, viewports);
}
void GraphicsDevice_Vulkan::BindResource(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource)
{
  assert(slot < GPU_RESOURCE_HEAP_SRV_COUNT);
  auto& descriptors = GetFrameResources().descriptors[cmd];
  if (descriptors.SRV[slot] != resource || descriptors.SRV_index[slot] != subresource)
  {
    descriptors.SRV[slot] = resource;
    descriptors.SRV_index[slot] = subresource;
    descriptors.dirty = true;
  }
}
void GraphicsDevice_Vulkan::BindResources(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd)
{
  if (resources != nullptr)
  {
    for (ezUInt32 i = 0; i < count; ++i)
    {
      BindResource(stage, resources[i], slot + i, cmd, -1);
    }
  }
}
void GraphicsDevice_Vulkan::BindUAV(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource)
{
  assert(slot < GPU_RESOURCE_HEAP_UAV_COUNT);
  auto& descriptors = GetFrameResources().descriptors[cmd];
  if (descriptors.UAV[slot] != resource || descriptors.UAV_index[slot] != subresource)
  {
    descriptors.UAV[slot] = resource;
    descriptors.UAV_index[slot] = subresource;
    descriptors.dirty = true;
  }
}
void GraphicsDevice_Vulkan::BindUAVs(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd)
{
  if (resources != nullptr)
  {
    for (ezUInt32 i = 0; i < count; ++i)
    {
      BindUAV(stage, resources[i], slot + i, cmd, -1);
    }
  }
}
void GraphicsDevice_Vulkan::UnbindResources(ezUInt32 slot, ezUInt32 num, CommandList cmd)
{
}
void GraphicsDevice_Vulkan::UnbindUAVs(ezUInt32 slot, ezUInt32 num, CommandList cmd)
{
}
void GraphicsDevice_Vulkan::BindSampler(ezEnum<ezRHIShaderStage> stage, const Sampler* sampler, ezUInt32 slot, CommandList cmd)
{
  assert(slot < GPU_SAMPLER_HEAP_COUNT);
  auto& descriptors = GetFrameResources().descriptors[cmd];
  if (descriptors.SAM[slot] != sampler)
  {
    descriptors.SAM[slot] = sampler;
    descriptors.dirty = true;
  }
}
void GraphicsDevice_Vulkan::BindConstantBuffer(ezEnum<ezRHIShaderStage> stage, const GPUBuffer* buffer, ezUInt32 slot, CommandList cmd)
{
  assert(slot < GPU_RESOURCE_HEAP_CBV_COUNT);
  auto& descriptors = GetFrameResources().descriptors[cmd];
  if (buffer->desc.Usage == ezRHIUsage::Dynamic || descriptors.CBV[slot] != buffer)
  {
    descriptors.CBV[slot] = buffer;
    descriptors.dirty = true;
  }
}
void GraphicsDevice_Vulkan::BindVertexBuffers(const GPUBuffer* const* vertexBuffers, ezUInt32 slot, ezUInt32 count, const ezUInt32* strides, const ezUInt32* offsets, CommandList cmd)
{
  VkDeviceSize voffsets[8] = {};
  VkBuffer vbuffers[8] = {};
  assert(count <= 8);
  for (ezUInt32 i = 0; i < count; ++i)
  {
    if (vertexBuffers[i] == nullptr || !vertexBuffers[i]->IsValid())
    {
      vbuffers[i] = nullBuffer;
    }
    else
    {
      auto internal_state = to_internal(vertexBuffers[i]);
      vbuffers[i] = internal_state->resource;
      if (offsets != nullptr)
      {
        voffsets[i] = (VkDeviceSize)offsets[i];
      }
    }
  }

  vkCmdBindVertexBuffers(GetDirectCommandList(cmd), static_cast<ezUInt32>(slot), static_cast<ezUInt32>(count), vbuffers, voffsets);
}
void GraphicsDevice_Vulkan::BindIndexBuffer(const GPUBuffer* indexBuffer, const ezEnum<ezRHIIndexBufferFormat> format, ezUInt32 offset, CommandList cmd)
{
  if (indexBuffer != nullptr)
  {
    auto internal_state = to_internal(indexBuffer);
    vkCmdBindIndexBuffer(GetDirectCommandList(cmd), internal_state->resource, (VkDeviceSize)offset, format == ezRHIIndexBufferFormat::UInt16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
  }
}
void GraphicsDevice_Vulkan::BindStencilRef(ezUInt32 value, CommandList cmd)
{
  vkCmdSetStencilReference(GetDirectCommandList(cmd), VK_STENCIL_FRONT_AND_BACK, value);
}
void GraphicsDevice_Vulkan::BindBlendFactor(float r, float g, float b, float a, CommandList cmd)
{
  float blendConstants[] = {r, g, b, a};
  vkCmdSetBlendConstants(GetDirectCommandList(cmd), blendConstants);
}
void GraphicsDevice_Vulkan::BindPipelineState(const PipelineState* pso, CommandList cmd)
{
  size_t pipeline_hash = pso->hash;
  if (active_renderpass[cmd] != nullptr)
  {
    pipeline_hash = ezHashingUtils::xxHash32(&active_renderpass[cmd]->hash, sizeof(ezUInt32), pipeline_hash);
  }
  if (prev_pipeline_hash[cmd] == pipeline_hash)
  {
    return;
  }
  prev_pipeline_hash[cmd] = pipeline_hash;

  GetFrameResources().descriptors[cmd].dirty = true;
  active_pso[cmd] = pso;
  dirty_pso[cmd] = true;
}
void GraphicsDevice_Vulkan::BindComputeShader(const Shader* cs, CommandList cmd)
{
  assert(cs->stage == ezRHIShaderStage::ComputeShader);
  if (active_cs[cmd] != cs)
  {
    GetFrameResources().descriptors[cmd].dirty = true;
    active_cs[cmd] = cs;
    auto internal_state = to_internal(cs);
    vkCmdBindPipeline(GetDirectCommandList(cmd), VK_PIPELINE_BIND_POINT_COMPUTE, internal_state->pipeline_cs);
  }
}
void GraphicsDevice_Vulkan::Draw(ezUInt32 vertexCount, ezUInt32 startVertexLocation, CommandList cmd)
{
  predraw(cmd);
  vkCmdDraw(GetDirectCommandList(cmd), static_cast<ezUInt32>(vertexCount), 1, startVertexLocation, 0);
}
void GraphicsDevice_Vulkan::DrawIndexed(ezUInt32 indexCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, CommandList cmd)
{
  predraw(cmd);
  vkCmdDrawIndexed(GetDirectCommandList(cmd), static_cast<ezUInt32>(indexCount), 1, startIndexLocation, baseVertexLocation, 0);
}
void GraphicsDevice_Vulkan::DrawInstanced(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 startVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd)
{
  predraw(cmd);
  vkCmdDraw(GetDirectCommandList(cmd), static_cast<ezUInt32>(vertexCount), static_cast<ezUInt32>(instanceCount), startVertexLocation, startInstanceLocation);
}
void GraphicsDevice_Vulkan::DrawIndexedInstanced(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd)
{
  predraw(cmd);
  vkCmdDrawIndexed(GetDirectCommandList(cmd), static_cast<ezUInt32>(indexCount), static_cast<ezUInt32>(instanceCount), startIndexLocation, baseVertexLocation, startInstanceLocation);
}
void GraphicsDevice_Vulkan::DrawInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  predraw(cmd);
  auto internal_state = to_internal(args);
  vkCmdDrawIndirect(GetDirectCommandList(cmd), internal_state->resource, (VkDeviceSize)args_offset, 1, (ezUInt32)sizeof(IndirectDrawArgsInstanced));
}
void GraphicsDevice_Vulkan::DrawIndexedInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  predraw(cmd);
  auto internal_state = to_internal(args);
  vkCmdDrawIndexedIndirect(GetDirectCommandList(cmd), internal_state->resource, (VkDeviceSize)args_offset, 1, (ezUInt32)sizeof(IndirectDrawArgsIndexedInstanced));
}
void GraphicsDevice_Vulkan::Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ, CommandList cmd)
{
  predispatch(cmd);
  vkCmdDispatch(GetDirectCommandList(cmd), threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}
void GraphicsDevice_Vulkan::DispatchIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  predispatch(cmd);
  auto internal_state = to_internal(args);
  vkCmdDispatchIndirect(GetDirectCommandList(cmd), internal_state->resource, (VkDeviceSize)args_offset);
}
void GraphicsDevice_Vulkan::DispatchMesh(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ, CommandList cmd)
{
  predraw(cmd);
  cmdDrawMeshTasksNV(GetDirectCommandList(cmd), threadGroupCountX * threadGroupCountY * threadGroupCountZ, 0);
}
void GraphicsDevice_Vulkan::DispatchMeshIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd)
{
  predraw(cmd);
  auto internal_state = to_internal(args);
  cmdDrawMeshTasksIndirectNV(GetDirectCommandList(cmd), internal_state->resource, (VkDeviceSize)args_offset, 1, sizeof(IndirectDispatchArgs));
}
void GraphicsDevice_Vulkan::CopyResource(const GPUResource* pDst, const GPUResource* pSrc, CommandList cmd)
{
  if (pDst->type == GPUResource::GPU_RESOURCE_TYPE::TEXTURE && pSrc->type == GPUResource::GPU_RESOURCE_TYPE::TEXTURE)
  {
    auto internal_state_src = to_internal((const Texture*)pSrc);
    auto internal_state_dst = to_internal((const Texture*)pDst);

    const TextureDesc& src_desc = ((const Texture*)pSrc)->GetDesc();
    const TextureDesc& dst_desc = ((const Texture*)pDst)->GetDesc();

    if (src_desc.Usage.IsSet(ezRHIUsage::Staging))
    {
      VkBufferImageCopy copy = {};
      copy.imageExtent.width = dst_desc.Width;
      copy.imageExtent.height = dst_desc.Height;
      copy.imageExtent.depth = 1;
      copy.imageExtent.width = dst_desc.Width;
      copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      copy.imageSubresource.layerCount = 1;
      vkCmdCopyBufferToImage(
        GetDirectCommandList(cmd),
        internal_state_src->staging_resource,
        internal_state_dst->resource,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copy);
    }
    else if (dst_desc.Usage.IsSet(ezRHIUsage::Staging))
    {
      VkBufferImageCopy copy = {};
      copy.imageExtent.width = src_desc.Width;
      copy.imageExtent.height = src_desc.Height;
      copy.imageExtent.depth = 1;
      copy.imageExtent.width = src_desc.Width;
      copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      copy.imageSubresource.layerCount = 1;
      vkCmdCopyImageToBuffer(
        GetDirectCommandList(cmd),
        internal_state_src->resource,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        internal_state_dst->staging_resource,
        1,
        &copy);
    }
    else
    {
      VkImageCopy copy = {};
      copy.extent.width = dst_desc.Width;
      copy.extent.height = dst_desc.Height;
      copy.extent.depth = ezMath::Max(1u, dst_desc.Depth);

      copy.srcOffset.x = 0;
      copy.srcOffset.y = 0;
      copy.srcOffset.z = 0;

      copy.dstOffset.x = 0;
      copy.dstOffset.y = 0;
      copy.dstOffset.z = 0;

      if (src_desc.BindFlags & BIND_DEPTH_STENCIL)
      {
        copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (FormatHelpers::IsFormatStencilSupport(src_desc.Format))
        {
          copy.srcSubresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
      }
      else
      {
        copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      }
      copy.srcSubresource.baseArrayLayer = 0;
      copy.srcSubresource.layerCount = src_desc.ArraySize;
      copy.srcSubresource.mipLevel = 0;

      if (dst_desc.BindFlags & BIND_DEPTH_STENCIL)
      {
        copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (FormatHelpers::IsFormatStencilSupport(dst_desc.Format))
        {
          copy.dstSubresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
      }
      else
      {
        copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      }
      copy.dstSubresource.baseArrayLayer = 0;
      copy.dstSubresource.layerCount = dst_desc.ArraySize;
      copy.dstSubresource.mipLevel = 0;

      vkCmdCopyImage(GetDirectCommandList(cmd),
        internal_state_src->resource, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        internal_state_dst->resource, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &copy);
    }
  }
  else if (pDst->type == GPUResource::GPU_RESOURCE_TYPE::BUFFER && pSrc->type == GPUResource::GPU_RESOURCE_TYPE::BUFFER)
  {
    auto internal_state_src = to_internal((const GPUBuffer*)pSrc);
    auto internal_state_dst = to_internal((const GPUBuffer*)pDst);

    const GPUBufferDesc& src_desc = ((const GPUBuffer*)pSrc)->GetDesc();
    const GPUBufferDesc& dst_desc = ((const GPUBuffer*)pDst)->GetDesc();

    VkBufferCopy copy = {};
    copy.srcOffset = 0;
    copy.dstOffset = 0;
    copy.size = (VkDeviceSize)ezMath::Min(src_desc.ByteWidth, dst_desc.ByteWidth);

    vkCmdCopyBuffer(GetDirectCommandList(cmd),
      internal_state_src->resource,
      internal_state_dst->resource,
      1, &copy);
  }
}
void GraphicsDevice_Vulkan::UpdateBuffer(const GPUBuffer* buffer, const void* data, CommandList cmd, int dataSize)
{
  assert(buffer->desc.Usage != ezRHIUsage::Immutable && "Cannot update IMMUTABLE GPUBuffer!");
  assert((int)buffer->desc.ByteWidth >= dataSize || dataSize < 0 && "Data size is too big!");

  if (dataSize == 0)
  {
    return;
  }
  auto internal_state = to_internal(buffer);

  dataSize = ezMath::Min((int)buffer->desc.ByteWidth, dataSize);
  dataSize = (dataSize >= 0 ? dataSize : buffer->desc.ByteWidth);


  if (buffer->desc.Usage == ezRHIUsage::Dynamic && buffer->desc.BindFlags & BIND_CONSTANT_BUFFER)
  {
    // Dynamic buffer will be used from host memory directly:
    GPUAllocation allocation = AllocateGPU(dataSize, cmd);
    memcpy(allocation.data, data, dataSize);
    internal_state->dynamic[cmd] = allocation;
    GetFrameResources().descriptors[cmd].dirty = true;
  }
  else
  {
    // Contents will be transferred to device memory:

    // barrier to transfer:

    assert(active_renderpass[cmd] == nullptr); // must not be inside render pass

    VkPipelineStageFlags stages = 0;

    VkBufferMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.buffer = internal_state->resource;
    barrier.srcAccessMask = 0;
    if (buffer->desc.BindFlags & BIND_CONSTANT_BUFFER)
    {
      barrier.srcAccessMask |= VK_ACCESS_UNIFORM_READ_BIT;
      stages = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    if (buffer->desc.BindFlags & BIND_VERTEX_BUFFER)
    {
      barrier.srcAccessMask |= VK_ACCESS_INDEX_READ_BIT;
      stages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    }
    if (buffer->desc.BindFlags & BIND_INDEX_BUFFER)
    {
      barrier.srcAccessMask |= VK_ACCESS_INDEX_READ_BIT;
      stages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    }
    if (buffer->desc.BindFlags & BIND_SHADER_RESOURCE)
    {
      barrier.srcAccessMask |= VK_ACCESS_SHADER_READ_BIT;
      stages = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    if (buffer->desc.BindFlags & BIND_UNORDERED_ACCESS)
    {
      barrier.srcAccessMask |= VK_ACCESS_SHADER_WRITE_BIT;
      stages = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    if (buffer->desc.MiscFlags & RESOURCE_MISC_RAY_TRACING)
    {
      barrier.srcAccessMask |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
      stages = VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
    }
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.size = VK_WHOLE_SIZE;

    vkCmdPipelineBarrier(
      GetDirectCommandList(cmd),
      stages,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      0,
      0, nullptr,
      1, &barrier,
      0, nullptr);


    // issue data copy:
    ezUInt8* dest = GetFrameResources().resourceBuffer[cmd].allocate(dataSize, 1);
    memcpy(dest, data, dataSize);

    VkBufferCopy copyRegion = {};
    copyRegion.size = dataSize;
    copyRegion.srcOffset = GetFrameResources().resourceBuffer[cmd].calculateOffset(dest);
    copyRegion.dstOffset = 0;

    //vkCmdCopyBuffer(GetDirectCommandList(cmd), std::static_pointer_cast<Buffer_Vulkan>(GetFrameResources().resourceBuffer[cmd].buffer.internal_state)->resource, internal_state->resource, 1, &copyRegion);
    vkCmdCopyBuffer(GetDirectCommandList(cmd), GetFrameResources().resourceBuffer[cmd].buffer.internal_state.Downcast<Buffer_Vulkan>()->resource, internal_state->resource, 1, &copyRegion);



    // reverse barrier:
    std::swap(barrier.srcAccessMask, barrier.dstAccessMask);

    vkCmdPipelineBarrier(
      GetDirectCommandList(cmd),
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      stages,
      0,
      0, nullptr,
      1, &barrier,
      0, nullptr);
  }
}
void GraphicsDevice_Vulkan::QueryBegin(const GPUQuery* query, CommandList cmd)
{
  auto internal_state = to_internal(query);

  switch (internal_state->query_type)
  {
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
      vkCmdBeginQuery(GetDirectCommandList(cmd), querypool_occlusion, (ezUInt32)internal_state->query_index, 0);
      break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION:
      vkCmdBeginQuery(GetDirectCommandList(cmd), querypool_occlusion, (ezUInt32)internal_state->query_index, VK_QUERY_CONTROL_PRECISE_BIT);
      break;
  }
}
void GraphicsDevice_Vulkan::QueryEnd(const GPUQuery* query, CommandList cmd)
{
  auto internal_state = to_internal(query);

  switch (internal_state->query_type)
  {
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_TIMESTAMP:
      vkCmdWriteTimestamp(GetDirectCommandList(cmd), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, querypool_timestamp, internal_state->query_index);
      break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION_PREDICATE:
      vkCmdEndQuery(GetDirectCommandList(cmd), querypool_occlusion, internal_state->query_index);
      break;
    case ezRHIGPUQueryType::GPU_QUERY_TYPE_OCCLUSION:
      vkCmdEndQuery(GetDirectCommandList(cmd), querypool_occlusion, internal_state->query_index);
      break;
  }
}
void GraphicsDevice_Vulkan::Barrier(const GPUBarrier* barriers, ezUInt32 numBarriers, CommandList cmd)
{
  VkMemoryBarrier memorybarriers[8];
  VkImageMemoryBarrier imagebarriers[8];
  VkBufferMemoryBarrier bufferbarriers[8];
  ezUInt32 memorybarrier_count = 0;
  ezUInt32 imagebarrier_count = 0;
  ezUInt32 bufferbarrier_count = 0;

  for (ezUInt32 i = 0; i < numBarriers; ++i)
  {
    const GPUBarrier& barrier = barriers[i];

    switch (barrier.type)
    {
      default:
      case GPUBarrier::MEMORY_BARRIER:
      {
        VkMemoryBarrier& barrierdesc = memorybarriers[memorybarrier_count++];
        barrierdesc.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        barrierdesc.pNext = nullptr;
        barrierdesc.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        barrierdesc.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
        if (RAYTRACING)
        {
          barrierdesc.srcAccessMask |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
          barrierdesc.dstAccessMask |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
        }
      }
      break;
      case GPUBarrier::IMAGE_BARRIER:
      {
        const TextureDesc& desc = barrier.image.texture->GetDesc();
        auto internal_state = to_internal(barrier.image.texture);

        VkImageMemoryBarrier& barrierdesc = imagebarriers[imagebarrier_count++];
        barrierdesc.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierdesc.pNext = nullptr;
        barrierdesc.image = internal_state->resource;
        barrierdesc.oldLayout = _ConvertImageLayout(barrier.image.layout_before);
        barrierdesc.newLayout = _ConvertImageLayout(barrier.image.layout_after);
        barrierdesc.srcAccessMask = _ParseImageLayout(barrier.image.layout_before);
        barrierdesc.dstAccessMask = _ParseImageLayout(barrier.image.layout_after);
        if (desc.BindFlags & BIND_DEPTH_STENCIL)
        {
          barrierdesc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
          if (FormatHelpers::IsFormatStencilSupport(desc.Format))
          {
            barrierdesc.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
          }
        }
        else
        {
          barrierdesc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
        barrierdesc.subresourceRange.baseArrayLayer = 0;
        barrierdesc.subresourceRange.layerCount = desc.ArraySize;
        barrierdesc.subresourceRange.baseMipLevel = 0;
        barrierdesc.subresourceRange.levelCount = desc.MipLevels;
        barrierdesc.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierdesc.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      }
      break;
      case GPUBarrier::BUFFER_BARRIER:
      {
        auto internal_state = to_internal(barrier.buffer.buffer);

        VkBufferMemoryBarrier& barrierdesc = bufferbarriers[bufferbarrier_count++];
        barrierdesc.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrierdesc.pNext = nullptr;
        barrierdesc.buffer = internal_state->resource;
        barrierdesc.size = barrier.buffer.buffer->GetDesc().ByteWidth;
        barrierdesc.offset = 0;
        barrierdesc.srcAccessMask = _ParseBufferState(barrier.buffer.state_before);
        barrierdesc.dstAccessMask = _ParseBufferState(barrier.buffer.state_after);
        barrierdesc.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierdesc.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      }
      break;
    }
  }

  VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
  VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

  if (RAYTRACING)
  {
    srcStage |= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
    dstStage |= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
  }

  vkCmdPipelineBarrier(GetDirectCommandList(cmd),
    srcStage,
    dstStage,
    0,
    memorybarrier_count, memorybarriers,
    bufferbarrier_count, bufferbarriers,
    imagebarrier_count, imagebarriers);
}
void GraphicsDevice_Vulkan::BuildRaytracingAccelerationStructure(const RaytracingAccelerationStructure* dst, CommandList cmd, const RaytracingAccelerationStructure* src)
{
  auto dst_internal = to_internal(dst);

  VkAccelerationStructureBuildGeometryInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  info.flags = dst_internal->info.flags;
  info.dstAccelerationStructure = dst_internal->resource;
  info.srcAccelerationStructure = VK_NULL_HANDLE;
  info.update = VK_FALSE;
  info.geometryArrayOfPointers = VK_FALSE;

  VkBufferDeviceAddressInfo addressinfo = {};
  addressinfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  addressinfo.buffer = dst_internal->buffer;
  info.scratchData.deviceAddress = vkGetBufferDeviceAddress(device, &addressinfo) + dst_internal->scratch_offset;

  if (src != nullptr)
  {
    info.update = VK_TRUE;

    auto src_internal = to_internal(src);
    info.srcAccelerationStructure = src_internal->resource;
  }

  ezDynamicArray<VkAccelerationStructureGeometryKHR> geometries;
  ezDynamicArray<VkAccelerationStructureBuildOffsetInfoKHR> offsetinfos;

  switch (dst->desc.type)
  {
    case RaytracingAccelerationStructureDesc::BOTTOMLEVEL:
    {
      info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
      info.geometryCount = (ezUInt32)dst->desc.bottomlevel.geometries.GetCount();
      geometries.Reserve(info.geometryCount);
      offsetinfos.Reserve(info.geometryCount);

      size_t i = 0;
      for (auto& x : dst->desc.bottomlevel.geometries)
      {
        auto& geometry = geometries.ExpandAndGetRef();
        geometry = {};
        geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;

        auto& offset = offsetinfos.ExpandAndGetRef();
        offset = {};

        if (x._flags & RaytracingAccelerationStructureDesc::BottomLevel::Geometry::FLAG_OPAQUE)
        {
          geometry.flags |= VK_GEOMETRY_OPAQUE_BIT_KHR;
        }
        if (x._flags & RaytracingAccelerationStructureDesc::BottomLevel::Geometry::FLAG_NO_DUPLICATE_ANYHIT_INVOCATION)
        {
          geometry.flags |= VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;
        }

        if (x.type == RaytracingAccelerationStructureDesc::BottomLevel::Geometry::TRIANGLES)
        {
          geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
          geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
          geometry.geometry.triangles.vertexStride = x.triangles.vertexStride;
          geometry.geometry.triangles.vertexFormat = _ConvertFormat(x.triangles.vertexFormat);
          geometry.geometry.triangles.indexType = x.triangles.indexFormat == ezRHIIndexBufferFormat::UInt16 ? VkIndexType::VK_INDEX_TYPE_UINT16 : VkIndexType::VK_INDEX_TYPE_UINT32;

          addressinfo.buffer = to_internal(&x.triangles.vertexBuffer)->resource;
          geometry.geometry.triangles.vertexData.deviceAddress = vkGetBufferDeviceAddress(device, &addressinfo) +
                                                                 x.triangles.vertexByteOffset;

          addressinfo.buffer = to_internal(&x.triangles.indexBuffer)->resource;
          geometry.geometry.triangles.indexData.deviceAddress = vkGetBufferDeviceAddress(device, &addressinfo) +
                                                                x.triangles.indexOffset * (x.triangles.indexFormat == ezRHIIndexBufferFormat::UInt16 ? sizeof(ezUInt16) : sizeof(ezUInt32));

          if (x._flags & RaytracingAccelerationStructureDesc::BottomLevel::Geometry::FLAG_USE_TRANSFORM)
          {
            addressinfo.buffer = to_internal(&x.triangles.transform3x4Buffer)->resource;
            geometry.geometry.triangles.transformData.deviceAddress = vkGetBufferDeviceAddress(device, &addressinfo);
            offset.transformOffset = x.triangles.transform3x4BufferOffset;
          }

          offset.primitiveCount = x.triangles.indexCount / 3;
        }
        else if (x.type == RaytracingAccelerationStructureDesc::BottomLevel::Geometry::PROCEDURAL_AABBS)
        {
          geometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
          geometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
          geometry.geometry.aabbs.stride = x.aabbs.stride;

          addressinfo.buffer = to_internal(&x.aabbs.aabbBuffer)->resource;
          geometry.geometry.aabbs.data.deviceAddress = vkGetBufferDeviceAddress(device, &addressinfo);

          offset.primitiveCount = x.aabbs.offset;
          offset.primitiveOffset = x.aabbs.offset;
        }
      }
    }
    break;
    case RaytracingAccelerationStructureDesc::TOPLEVEL:
    {
      info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
      info.geometryCount = 1;
      geometries.Reserve(info.geometryCount);
      offsetinfos.Reserve(info.geometryCount);

      auto& geometry = geometries.ExpandAndGetRef();
      geometry = {};
      geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
      geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
      geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
      geometry.geometry.instances.arrayOfPointers = VK_FALSE;

      addressinfo.buffer = to_internal(&dst->desc.toplevel.instanceBuffer)->resource;
      geometry.geometry.instances.data.deviceAddress = vkGetBufferDeviceAddress(device, &addressinfo);

      auto& offset = offsetinfos.ExpandAndGetRef();
      offset = {};
      offset.primitiveCount = dst->desc.toplevel.count;
      offset.primitiveOffset = dst->desc.toplevel.offset;
    }
    break;
  }

  VkAccelerationStructureGeometryKHR* pGeomtries = geometries.GetData();
  info.ppGeometries = &pGeomtries;

  VkAccelerationStructureBuildOffsetInfoKHR* pOffsetinfo = offsetinfos.GetData();

  cmdBuildAccelerationStructureKHR(GetDirectCommandList(cmd), 1, &info, &pOffsetinfo);
}
void GraphicsDevice_Vulkan::BindRaytracingPipelineState(const RaytracingPipelineState* rtpso, CommandList cmd)
{
  prev_pipeline_hash[cmd] = 0;
  GetFrameResources().descriptors[cmd].dirty = true;
  //active_cs[cmd] = rtpso->desc.shaderlibraries.front().shader; // we just take the first shader (todo: better)
  active_cs[cmd] = rtpso->desc.shaderlibraries[0].shader; // ezTODO: do much better
  active_rt[cmd] = rtpso;

  vkCmdBindPipeline(GetDirectCommandList(cmd), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, to_internal(rtpso)->pipeline);
}
void GraphicsDevice_Vulkan::DispatchRays(const DispatchRaysDesc* desc, CommandList cmd)
{
  preraytrace(cmd);

  VkStridedBufferRegionKHR raygen = {};
  raygen.buffer = desc->raygeneration.buffer ? to_internal(desc->raygeneration.buffer)->resource : VK_NULL_HANDLE;
  raygen.offset = desc->raygeneration.offset;
  raygen.size = desc->raygeneration.size;
  raygen.stride = desc->raygeneration.stride;

  VkStridedBufferRegionKHR miss = {};
  miss.buffer = desc->miss.buffer ? to_internal(desc->miss.buffer)->resource : VK_NULL_HANDLE;
  miss.offset = desc->miss.offset;
  miss.size = desc->miss.size;
  miss.stride = desc->miss.stride;

  VkStridedBufferRegionKHR hitgroup = {};
  hitgroup.buffer = desc->hitgroup.buffer ? to_internal(desc->hitgroup.buffer)->resource : VK_NULL_HANDLE;
  hitgroup.offset = desc->hitgroup.offset;
  hitgroup.size = desc->hitgroup.size;
  hitgroup.stride = desc->hitgroup.stride;

  VkStridedBufferRegionKHR callable = {};
  callable.buffer = desc->callable.buffer ? to_internal(desc->callable.buffer)->resource : VK_NULL_HANDLE;
  callable.offset = desc->callable.offset;
  callable.size = desc->callable.size;
  callable.stride = desc->callable.stride;

  cmdTraceRaysKHR(
    GetDirectCommandList(cmd),
    &raygen,
    &miss,
    &hitgroup,
    &callable,
    desc->Width,
    desc->Height,
    desc->Depth);
}

void GraphicsDevice_Vulkan::BindDescriptorTable(BINDPOINT bindpoint, ezUInt32 space, const DescriptorTable* table, CommandList cmd)
{
  const RootSignature* rootsig = nullptr;
  switch (bindpoint)
  {
    default:
    case GRAPHICS:
      rootsig = active_pso[cmd]->desc.rootSignature;
      break;
    case COMPUTE:
      rootsig = active_cs[cmd]->rootSignature;
      break;
    case BINDPOINT::RAYTRACING:
      rootsig = active_rt[cmd]->desc.rootSignature;
      break;
  }
  auto rootsig_internal = to_internal(rootsig);
  auto& descriptors = GetFrameResources().descriptors[cmd];
  rootsig_internal->last_tables[cmd][space] = table;
  rootsig_internal->last_descriptorsets[cmd][space] = descriptors.commit(table);
  rootsig_internal->dirty[cmd] = true;
  for (auto& x : rootsig_internal->root_descriptors[cmd])
  {
    x = nullptr;
  }
}
void GraphicsDevice_Vulkan::BindRootDescriptor(BINDPOINT bindpoint, ezUInt32 index, const GPUBuffer* buffer, ezUInt32 offset, CommandList cmd)
{
  const RootSignature* rootsig = nullptr;
  switch (bindpoint)
  {
    default:
    case GRAPHICS:
      rootsig = active_pso[cmd]->desc.rootSignature;
      break;
    case COMPUTE:
      rootsig = active_cs[cmd]->rootSignature;
      break;
    case ::RAYTRACING:
      rootsig = active_rt[cmd]->desc.rootSignature;
      break;
  }
  auto rootsig_internal = to_internal(rootsig);
  rootsig_internal->root_offsets[cmd][index] = offset;

  if (buffer != rootsig_internal->root_descriptors[cmd][index])
  {
    rootsig_internal->root_descriptors[cmd][index] = buffer;

    auto remap = rootsig_internal->root_remap[index];

    if (!rootsig_internal->dirty[cmd])
    {
      // Need to recommit descriptor set if root descriptor changes and the set is already bound:
      auto& descriptors = GetFrameResources().descriptors[cmd];
      VkDescriptorSet set = descriptors.commit(rootsig_internal->last_tables[cmd][remap.space]);
      rootsig_internal->last_descriptorsets[cmd][remap.space] = set;
    }

    // Then write root descriptor on top:
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = to_internal(buffer)->resource;
    bufferInfo.offset = 0;

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = rootsig_internal->last_descriptorsets[cmd][remap.space];
    write.dstBinding = remap.binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.pBufferInfo = &bufferInfo;

    switch (rootsig_internal->last_tables[cmd][remap.space]->resources[remap.rangeIndex].binding)
    {
      case ROOT_CONSTANTBUFFER:
        bufferInfo.range = ezMath::Min(buffer->desc.ByteWidth, device_properties.properties.limits.maxUniformBufferRange);
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        break;
      case ROOT_RAWBUFFER:
      case ROOT_STRUCTUREDBUFFER:
      case ROOT_RWRAWBUFFER:
      case ROOT_RWSTRUCTUREDBUFFER:
        bufferInfo.range = VK_WHOLE_SIZE;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        break;
      default:
        assert(0); // this function is only usable for root buffers!
        break;
    }

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
  }

  rootsig_internal->dirty[cmd] = true;
}
void GraphicsDevice_Vulkan::BindRootConstants(BINDPOINT bindpoint, ezUInt32 index, const void* srcdata, CommandList cmd)
{
  const RootSignature* rootsig = nullptr;
  switch (bindpoint)
  {
    default:
    case GRAPHICS:
      rootsig = active_pso[cmd]->desc.rootSignature;
      break;
    case COMPUTE:
      rootsig = active_cs[cmd]->rootSignature;
      break;
    case BINDPOINT::RAYTRACING:
      rootsig = active_rt[cmd]->desc.rootSignature;
      break;
  }
  auto rootsig_internal = to_internal(rootsig);

  const RootConstantRange& range = rootsig->rootconstants[index];
  vkCmdPushConstants(
    GetDirectCommandList(cmd),
    rootsig_internal->pipelineLayout,
    _ConvertStageFlags(range.stage),
    range.offset,
    range.size,
    srcdata);
}

GraphicsDevice::GPUAllocation GraphicsDevice_Vulkan::AllocateGPU(size_t dataSize, CommandList cmd)
{
  GPUAllocation result;
  if (dataSize == 0)
  {
    return result;
  }

  FrameResources::ResourceFrameAllocator& allocator = GetFrameResources().resourceBuffer[cmd];
  ezUInt8* dest = allocator.allocate(dataSize, 256);
  assert(dest != nullptr);

  result.buffer = &allocator.buffer;
  result.offset = (ezUInt32)allocator.calculateOffset(dest);
  result.data = (void*)dest;
  return result;
}

void GraphicsDevice_Vulkan::EventBegin(const char* name, CommandList cmd)
{
  if (cmdBeginDebugUtilsLabelEXT != nullptr)
  {
    VkDebugUtilsLabelEXT label = {};
    label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    label.pLabelName = name;
    label.color[0] = 0;
    label.color[1] = 0;
    label.color[2] = 0;
    label.color[3] = 1;
    cmdBeginDebugUtilsLabelEXT(GetDirectCommandList(cmd), &label);
  }
}
void GraphicsDevice_Vulkan::EventEnd(CommandList cmd)
{
  if (cmdEndDebugUtilsLabelEXT != nullptr)
  {
    cmdEndDebugUtilsLabelEXT(GetDirectCommandList(cmd));
  }
}
void GraphicsDevice_Vulkan::SetMarker(const char* name, CommandList cmd)
{
  if (cmdInsertDebugUtilsLabelEXT != nullptr)
  {
    VkDebugUtilsLabelEXT label = {};
    label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    label.pLabelName = name;
    label.color[0] = 0;
    label.color[1] = 0;
    label.color[2] = 0;
    label.color[3] = 1;
    cmdInsertDebugUtilsLabelEXT(GetDirectCommandList(cmd), &label);
  }
}


#endif // WICKEDENGINE_BUILD_VULKAN
