#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Memory/VKMemory.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/Utilities/VKUtility.h>
#include <RHIVulkan/View/VKView.h>

VKResource::VKResource(VKDevice& device)
  : m_device(device)
{
}

VKResource::~VKResource()
{
  if (resource_type == ResourceType::kBuffer)
  {
    if (buffer.res != VK_NULL_HANDLE)
      vkDestroyBuffer(m_device.GetDevice(), buffer.res, nullptr);
  }
  else if (resource_type == ResourceType::kTexture)
  {
    //if (image.res != VK_NULL_HANDLE)
      //vkDestroyImage(m_device.GetDevice(), image.res, nullptr);
  }
  else if (resource_type == ResourceType::kSampler)
  {
    if (sampler.res != VK_NULL_HANDLE)
      vkDestroySampler(m_device.GetDevice(), sampler.res, nullptr);
  }
}

void VKResource::CommitMemory(MemoryType memory_type)
{
  MemoryRequirements mem_requirements = GetMemoryRequirements();
  VkMemoryDedicatedAllocateInfoKHR dedicated_allocate_info = {};
  dedicated_allocate_info.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
  VkMemoryDedicatedAllocateInfoKHR* p_dedicated_allocate_info = nullptr;
  if (resource_type == ResourceType::kBuffer)
  {
    dedicated_allocate_info.buffer = buffer.res;
    p_dedicated_allocate_info = &dedicated_allocate_info;
  }
  else if (resource_type == ResourceType::kTexture)
  {
    dedicated_allocate_info.image = image.res;
    p_dedicated_allocate_info = &dedicated_allocate_info;
  }
  auto memory = std::make_shared<VKMemory>(m_device, mem_requirements.size, memory_type, mem_requirements.memory_type_bits, p_dedicated_allocate_info);
  BindMemory(memory, 0);
}

void VKResource::BindMemory(const std::shared_ptr<Memory>& memory, uint64_t offset)
{
  m_memory = memory;
  m_memory_type = m_memory->GetMemoryType();
  m_vk_memory = m_memory->As<VKMemory>().GetMemory();

  if (resource_type == ResourceType::kBuffer)
  {
    vkBindBufferMemory(m_device.GetDevice(), buffer.res, m_vk_memory, offset);
  }
  else if (resource_type == ResourceType::kTexture)
  {
    vkBindImageMemory(m_device.GetDevice(), image.res, m_vk_memory, offset);
  }
}

uint64_t VKResource::GetWidth() const
{
  if (resource_type == ResourceType::kTexture)
    return image.size.width;
  return buffer.size;
}

uint32_t VKResource::GetHeight() const
{
  return image.size.height;
}

uint16_t VKResource::GetLayerCount() const
{
  return image.array_layers;
}

uint16_t VKResource::GetLevelCount() const
{
  return image.level_count;
}

uint32_t VKResource::GetSampleCount() const
{
  return image.sample_count;
}

uint64_t VKResource::GetAccelerationStructureHandle() const
{
  VkAccelerationStructureDeviceAddressInfoKHR info = {};
  info.accelerationStructure = acceleration_structure_handle;
  return vkGetAccelerationStructureDeviceAddressKHR(m_device.GetDevice(), &info);
}

void VKResource::SetName(const std::string& name)
{
  VkDebugUtilsObjectNameInfoEXT info = {};
  info.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
  info.pObjectName = name.c_str();
  if (resource_type == ResourceType::kBuffer)
  {
    info.objectType = VkObjectType::VK_OBJECT_TYPE_BUFFER;
    info.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkBuffer>(buffer.res));
  }
  else if (resource_type == ResourceType::kTexture)
  {
    info.objectType = VkObjectType::VK_OBJECT_TYPE_IMAGE;
    info.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImage>(image.res));
  }
  vkSetDebugUtilsObjectNameEXT(m_device.GetDevice(), &info);
}

uint8_t* VKResource::Map()
{
  uint8_t* dst_data = nullptr;
  VkResult res = vkMapMemory(m_device.GetDevice(), m_vk_memory, 0, VK_WHOLE_SIZE, {}, reinterpret_cast<void**>(&dst_data));
  return dst_data;
}

void VKResource::Unmap()
{
  vkUnmapMemory(m_device.GetDevice(), m_vk_memory);
}

bool VKResource::AllowCommonStatePromotion(ResourceState state_after)
{
  return false;
}

MemoryRequirements VKResource::GetMemoryRequirements() const
{
  VkMemoryRequirements2 mem_requirements = {};
  mem_requirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
  if (resource_type == ResourceType::kBuffer)
  {
    VkBufferMemoryRequirementsInfo2KHR buffer_mem_req = {};
    buffer_mem_req.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
    buffer_mem_req.buffer = buffer.res;
    vkGetBufferMemoryRequirements2(m_device.GetDevice(), &buffer_mem_req, &mem_requirements);
  }
  else if (resource_type == ResourceType::kTexture)
  {
    VkImageMemoryRequirementsInfo2KHR image_mem_req = {};
    image_mem_req.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
    image_mem_req.image = image.res;
    vkGetImageMemoryRequirements2(m_device.GetDevice(), &image_mem_req, &mem_requirements);
  }
  return {mem_requirements.memoryRequirements.size, mem_requirements.memoryRequirements.alignment, mem_requirements.memoryRequirements.memoryTypeBits};
}
