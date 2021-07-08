#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/Utilities/VKUtility.h>
#include <RHIVulkan/View/VKView.h>

VKView::VKView(VKDevice& device, const std::shared_ptr<VKResource>& resource, const ViewDesc& view_desc)
  : m_device(device)
  , m_resource(resource)
  , m_view_desc(view_desc)
{
  if (resource)
  {
    CreateView();
  }

  if (view_desc.bindless)
  {
    VkDescriptorType type = GetDescriptorType(view_desc.view_type);
    decltype(auto) pool = device.GetGPUBindlessDescriptorPool(type);
    m_range = std::make_shared<VKGPUDescriptorPoolRange>(pool.Allocate(1));

    m_descriptor.dstSet = m_range->GetDescriptoSet();
    m_descriptor.dstArrayElement = m_range->GetOffset();
    m_descriptor.descriptorType = type;
    m_descriptor.dstBinding = 0;
    m_descriptor.descriptorCount = 1;
    m_device.GetDevice().updateDescriptorSets(1, &m_descriptor, 0, nullptr);
  }
}

VkImageViewType GetImageViewType(ViewDimension dimension)
{
  switch (dimension)
  {
    case ViewDimension::kTexture1D:
      return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;
    case ViewDimension::kTexture1DArray:
      return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    case ViewDimension::kTexture2D:
    case ViewDimension::kTexture2DMS:
      return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
    case ViewDimension::kTexture2DArray:
    case ViewDimension::kTexture2DMSArray:
      return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case ViewDimension::kTexture3D:
      return VkImageViewType::VK_IMAGE_VIEW_TYPE_3D;
    case ViewDimension::kTextureCube:
      return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
    case ViewDimension::kTextureCubeArray:
      return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
  }
  assert(false);
  return {};
}

void VKView::CreateView()
{
  switch (m_view_desc.view_type)
  {
    case ViewType::kSampler:
      m_descriptor_image.sampler = m_resource->sampler.res.get();
      m_descriptor.pImageInfo = &m_descriptor_image;
      break;
    case ViewType::kTexture:
    {
      CreateImageView();
      m_descriptor_image.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      m_descriptor_image.imageView = m_image_view;
      m_descriptor.pImageInfo = &m_descriptor_image;
      break;
    }
    case ViewType::kRWTexture:
    {
      CreateImageView();
      m_descriptor_image.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;
      m_descriptor_image.imageView = m_image_view;
      m_descriptor.pImageInfo = &m_descriptor_image;
      break;
    }
    case ViewType::kAccelerationStructure:
    {
      m_descriptor_acceleration_structure.accelerationStructureCount = 1;
      m_descriptor_acceleration_structure.pAccelerationStructures = &m_resource->acceleration_structure_handle.get();
      m_descriptor.pNext = &m_descriptor_acceleration_structure;
      break;
    }
    case ViewType::kShadingRateSource:
    case ViewType::kRenderTarget:
    case ViewType::kDepthStencil:
    {
      CreateImageView();
      break;
    }
    case ViewType::kConstantBuffer:
    case ViewType::kStructuredBuffer:
    case ViewType::kRWStructuredBuffer:
      m_descriptor_buffer.buffer = m_resource->buffer.res.get();
      m_descriptor_buffer.offset = m_view_desc.offset;
      m_descriptor_buffer.range = m_view_desc.buffer_size;
      m_descriptor.pBufferInfo = &m_descriptor_buffer;
      break;
    case ViewType::kBuffer:
    case ViewType::kRWBuffer:
      CreateBufferView();
      m_descriptor.pTexelBufferView = &m_buffer_view;
      break;
    default:
      assert(false);
      break;
  }
}

void VKView::CreateImageView()
{
  VkImageViewCreateInfo image_view_desc = {};
  image_view_desc.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_desc.image = m_resource->image.res;
  image_view_desc.format = m_resource->image.format;
  image_view_desc.viewType = GetImageViewType(m_view_desc.dimension);
  image_view_desc.subresourceRange.baseMipLevel = GetBaseMipLevel();
  image_view_desc.subresourceRange.levelCount = GetLevelCount();
  image_view_desc.subresourceRange.baseArrayLayer = GetBaseArrayLayer();
  image_view_desc.subresourceRange.layerCount = GetLayerCount();
  image_view_desc.subresourceRange.aspectMask = m_device.GetAspectFlags(image_view_desc.format);

  if (image_view_desc.subresourceRange.aspectMask & (VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT))
  {
    if (m_view_desc.plane_slice == 0)
    {
      image_view_desc.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    else
    {
      image_view_desc.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
      image_view_desc.components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
    }
  }

  vkCreateImageView(m_device.GetDevice(), &image_view_desc, nullptr, &m_image_view);
}

void VKView::CreateBufferView()
{
  VkBufferViewCreateInfo buffer_view_desc = {};
  buffer_view_desc.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_view_desc.buffer = m_resource->buffer.res.get();
  buffer_view_desc.format = VKUtils::ToVkFormat(m_view_desc.buffer_format);
  buffer_view_desc.offset = m_view_desc.offset;
  buffer_view_desc.range = m_view_desc.buffer_size;
  m_buffer_view = m_device.GetDevice().createBufferViewUnique(buffer_view_desc);
}

std::shared_ptr<Resource> VKView::GetResource()
{
  return m_resource;
}

uint32_t VKView::GetDescriptorId() const
{
  if (m_range)
    return m_range->GetOffset();
  return -1;
}

uint32_t VKView::GetBaseMipLevel() const
{
  return m_view_desc.base_mip_level;
}

uint32_t VKView::GetLevelCount() const
{
  return std::min<uint32_t>(m_view_desc.level_count, m_resource->GetLevelCount() - m_view_desc.base_mip_level);
}

uint32_t VKView::GetBaseArrayLayer() const
{
  return m_view_desc.base_array_layer;
}

uint32_t VKView::GetLayerCount() const
{
  return std::min<uint32_t>(m_view_desc.layer_count, m_resource->GetLayerCount() - m_view_desc.base_array_layer);
}

VkImageView VKView::GetImageView() const
{
  return m_image_view;
}

VkWriteDescriptorSet VKView::GetDescriptor() const
{
  return m_descriptor;
}
