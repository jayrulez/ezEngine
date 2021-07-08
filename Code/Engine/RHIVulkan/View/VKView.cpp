#include <RHIVulkanPCH.h>

#include <RHIVulkan/View/VKView.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPoolRange.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUBindlessDescriptorPoolTyped.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/Resource/VKResource.h>


vk::ImageViewType GetImageViewType(ezRHIViewDimension dimension)
{
  switch (dimension)
  {
    case ezRHIViewDimension::Texture1D:
      return vk::ImageViewType::e1D;
    case ezRHIViewDimension::Texture1DArray:
      return vk::ImageViewType::e1DArray;
    case ezRHIViewDimension::Texture2D:
    case ezRHIViewDimension::Texture2DMS:
      return vk::ImageViewType::e2D;
    case ezRHIViewDimension::Texture2DArray:
    case ezRHIViewDimension::Texture2DMSArray:
      return vk::ImageViewType::e2DArray;
    case ezRHIViewDimension::Texture3D:
      return vk::ImageViewType::e3D;
    case ezRHIViewDimension::TextureCube:
      return vk::ImageViewType::eCube;
    case ezRHIViewDimension::TextureCubeArray:
      return vk::ImageViewType::eCubeArray;
  }
  EZ_ASSERT_NOT_IMPLEMENTED;
  return {};
}

ezRHIVKView::ezRHIVKView(ezRHIVKDevice& device, ezRHIVKResource* resource, const ezRHIViewDesc& viewDesc)
  : m_Device{device}
  , m_Resource{resource}
  , m_ViewDesc{viewDesc}
  , m_Range{nullptr}
{
  if (resource)
  {
    CreateView();
  }

  if (viewDesc.m_Bindless)
  {
    vk::DescriptorType type = GetDescriptorType(viewDesc.m_ViewType);
    decltype(auto) pool = device.GetGPUBindlessDescriptorPool(type);
    m_Range = EZ_DEFAULT_NEW(ezRHIVKGPUDescriptorPoolRange, pool.Allocate(1));

    m_Descriptor.dstSet = m_Range->GetDescriptoSet();
    m_Descriptor.dstArrayElement = m_Range->GetOffset();
    m_Descriptor.descriptorType = type;
    m_Descriptor.dstBinding = 0;
    m_Descriptor.descriptorCount = 1;
    m_Device.GetDevice().updateDescriptorSets(1, &m_Descriptor, 0, nullptr);
  }
}

ezRHIResource* ezRHIVKView::GetResource()
{
  return m_Resource;
}

ezUInt32 ezRHIVKView::GetDescriptorId() const
{
  if (m_Range)
    return m_Range->GetOffset();
  return -1;
}

ezUInt32 ezRHIVKView::GetBaseMipLevel() const
{
  return m_ViewDesc.m_BaseMipLevel;
}

ezUInt32 ezRHIVKView::GetLevelCount() const
{
  return std::min<uint32_t>(m_ViewDesc.m_LevelCount, m_Resource->GetLevelCount() - m_ViewDesc.m_BaseMipLevel);
}

ezUInt32 ezRHIVKView::GetBaseArrayLayer() const
{
  return m_ViewDesc.m_BaseArray_layer;
}

ezUInt32 ezRHIVKView::GetLayerCount() const
{
  return ezMath::Min(m_ViewDesc.m_LayerCount, m_Resource->GetLayerCount() - m_ViewDesc.m_BaseArray_layer);
}

vk::ImageView ezRHIVKView::GetImageView() const
{
  return m_ImageView.get();
}

vk::WriteDescriptorSet ezRHIVKView::GetDescriptor() const
{
  return m_Descriptor;
}

void ezRHIVKView::CreateView()
{
  switch (m_ViewDesc.m_ViewType)
  {
    case ezRHIViewType::Sampler:
      m_DescriptorImage.sampler = m_Resource->m_Sampler.m_Sampler.get();
      m_Descriptor.pImageInfo = &m_DescriptorImage;
      break;
    case ezRHIViewType::Texture:
    {
      CreateImageView();
      m_DescriptorImage.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      m_DescriptorImage.imageView = m_ImageView.get();
      m_Descriptor.pImageInfo = &m_DescriptorImage;
      break;
    }
    case ezRHIViewType::RWTexture:
    {
      CreateImageView();
      m_DescriptorImage.imageLayout = vk::ImageLayout::eGeneral;
      m_DescriptorImage.imageView = m_ImageView.get();
      m_Descriptor.pImageInfo = &m_DescriptorImage;
      break;
    }
    case ezRHIViewType::AccelerationStructure:
    {
      m_DescriptorAccelerationStructure.accelerationStructureCount = 1;
      m_DescriptorAccelerationStructure.pAccelerationStructures = &m_Resource->m_AccelerationStructureHandle.get();
      m_Descriptor.pNext = &m_DescriptorAccelerationStructure;
      break;
    }
    case ezRHIViewType::ShadingRateSource:
    case ezRHIViewType::RenderTarget:
    case ezRHIViewType::DepthStencil:
    {
      CreateImageView();
      break;
    }
    case ezRHIViewType::ConstantBuffer:
    case ezRHIViewType::StructuredBuffer:
    case ezRHIViewType::RWStructuredBuffer:
      m_DescriptorBuffer.buffer = m_Resource->m_Buffer.m_Buffer.get();
      m_DescriptorBuffer.offset = m_ViewDesc.m_Offset;
      m_DescriptorBuffer.range = m_ViewDesc.m_BufferSize;
      m_Descriptor.pBufferInfo = &m_DescriptorBuffer;
      break;
    case ezRHIViewType::Buffer:
    case ezRHIViewType::RWBuffer:
      CreateBufferView();
      m_Descriptor.pTexelBufferView = &m_BufferView.get();
      break;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      break;
  }
}

void ezRHIVKView::CreateImageView()
{
  vk::ImageViewCreateInfo imageViewDesc = {};
  imageViewDesc.image = m_Resource->m_Image.m_Image;
  imageViewDesc.format = m_Resource->m_Image.m_Format;
  imageViewDesc.viewType = GetImageViewType(m_ViewDesc.m_Dimension);
  imageViewDesc.subresourceRange.baseMipLevel = GetBaseMipLevel();
  imageViewDesc.subresourceRange.levelCount = GetLevelCount();
  imageViewDesc.subresourceRange.baseArrayLayer = GetBaseArrayLayer();
  imageViewDesc.subresourceRange.layerCount = GetLayerCount();
  imageViewDesc.subresourceRange.aspectMask = m_Device.GetAspectFlags(imageViewDesc.format);

  if (imageViewDesc.subresourceRange.aspectMask & (vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil))
  {
    if (m_ViewDesc.m_PlaneSlice == 0)
    {
      imageViewDesc.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    }
    else
    {
      imageViewDesc.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eStencil;
      imageViewDesc.components.g = vk::ComponentSwizzle::eR;
    }
  }

  m_ImageView = m_Device.GetDevice().createImageViewUnique(imageViewDesc);
}

void ezRHIVKView::CreateBufferView()
{
  vk::BufferViewCreateInfo bufferViewDesc = {};
  bufferViewDesc.buffer = m_Resource->m_Buffer.m_Buffer.get();
  bufferViewDesc.format = VKUtilities::ToVKFormat(m_ViewDesc.m_BufferFormat);
  bufferViewDesc.offset = m_ViewDesc.m_Offset;
  bufferViewDesc.range = m_ViewDesc.m_BufferSize;
  m_BufferView = m_Device.GetDevice().createBufferViewUnique(bufferViewDesc);
}
