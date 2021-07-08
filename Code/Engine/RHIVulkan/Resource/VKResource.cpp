#include <RHIVulkanPCH.h>

#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Memory/VKMemory.h>

ezRHIVKResource::ezRHIVKResource(ezRHIVKDevice& device)
  : m_Device{device}
{
}

void ezRHIVKResource::CommitMemory(ezRHIMemoryType memoryType)
{
  ezRHIMemoryRequirements memRequirements = GetMemoryRequirements();
  vk::MemoryDedicatedAllocateInfoKHR dedicatedAllocateInfo = {};
  vk::MemoryDedicatedAllocateInfoKHR* pDedicatedAllocateInfo = nullptr;
  if (m_ResourceType == ezRHIResourceType::Buffer)
  {
    dedicatedAllocateInfo.buffer = m_Buffer.m_Buffer.get();
    pDedicatedAllocateInfo = &dedicatedAllocateInfo;
  }
  else if (m_ResourceType == ezRHIResourceType::Texture)
  {
    dedicatedAllocateInfo.image = m_Image.m_Image;
    pDedicatedAllocateInfo = &dedicatedAllocateInfo;
  }
  ezRHIVKMemory* memory = EZ_DEFAULT_NEW(ezRHIVKMemory, m_Device, memRequirements.m_Size, memoryType, memRequirements.m_MemoryTypeBits, pDedicatedAllocateInfo);
  BindMemory(memory, 0);
}

void ezRHIVKResource::BindMemory(ezRHIMemory* pMemory, ezUInt64 offset)
{
  m_Memory = pMemory;
  m_MemoryType = m_Memory->GetMemoryType();
  m_VkMemory = m_Memory->As<ezRHIVKMemory>()->GetMemory();

  if (m_ResourceType == ezRHIResourceType::Buffer)
  {
    m_Device.GetDevice().bindBufferMemory(m_Buffer.m_Buffer.get(), m_VkMemory, offset);
  }
  else if (m_ResourceType == ezRHIResourceType::Texture)
  {
    m_Device.GetDevice().bindImageMemory(m_Image.m_Image, m_VkMemory, offset);
  }
}

ezUInt64 ezRHIVKResource::GetWidth() const
{
  if (m_ResourceType == ezRHIResourceType::Texture)
    return m_Image.m_Size.width;
  return m_Buffer.m_Size;
}

ezUInt32 ezRHIVKResource::GetHeight() const
{
  return m_Image.m_Size.height;
}

ezUInt16 ezRHIVKResource::GetLayerCount() const
{
  return m_Image.m_ArrayLayers;
}

ezUInt16 ezRHIVKResource::GetLevelCount() const
{
  return m_Image.m_LevelCount;
}

ezUInt32 ezRHIVKResource::GetSampleCount() const
{
  return m_Image.m_SampleCount;
}

ezUInt64 ezRHIVKResource::GetAccelerationStructureHandle() const
{
  return m_Device.GetDevice().getAccelerationStructureAddressKHR({m_AccelerationStructureHandle.get()});
}

void ezRHIVKResource::SetName(const ezString& name)
{
  vk::DebugUtilsObjectNameInfoEXT info = {};
  info.pObjectName = name.GetData();
  if (m_ResourceType == ezRHIResourceType::Buffer)
  {
    info.objectType = m_Buffer.m_Buffer.get().objectType;
    info.objectHandle = reinterpret_cast<ezUInt64>(static_cast<VkBuffer>(m_Buffer.m_Buffer.get()));
  }
  else if (m_ResourceType == ezRHIResourceType::Texture)
  {
    info.objectType = m_Image.m_Image.objectType;
    info.objectHandle = reinterpret_cast<ezUInt64>(static_cast<VkImage>(m_Image.m_Image));
  }
  m_Device.GetDevice().setDebugUtilsObjectNameEXT(info);
}

ezUInt8* ezRHIVKResource::Map()
{
  ezUInt8* dst_data = nullptr;
  vk::Result result = m_Device.GetDevice().mapMemory(m_VkMemory, 0, VK_WHOLE_SIZE, {}, reinterpret_cast<void**>(&dst_data));
  return dst_data;
}

void ezRHIVKResource::Unmap()
{
  m_Device.GetDevice().unmapMemory(m_VkMemory);
}

bool ezRHIVKResource::AllowCommonStatePromotion(ezBitflags<ezRHIResourceState> stateAfter)
{
  return false;
}

ezRHIMemoryRequirements ezRHIVKResource::GetMemoryRequirements() const
{
  vk::MemoryRequirements2 memRequirements = {};
  if (m_ResourceType == ezRHIResourceType::Buffer)
  {
    vk::BufferMemoryRequirementsInfo2KHR bufferMemReq = {};
    bufferMemReq.buffer = m_Buffer.m_Buffer.get();
    m_Device.GetDevice().getBufferMemoryRequirements2(&bufferMemReq, &memRequirements);
  }
  else if (m_ResourceType == ezRHIResourceType::Texture)
  {
    vk::ImageMemoryRequirementsInfo2KHR imageMemReq = {};
    imageMemReq.image = m_Image.m_Image;
    m_Device.GetDevice().getImageMemoryRequirements2(&imageMemReq, &memRequirements);
  }
  return {memRequirements.memoryRequirements.size, memRequirements.memoryRequirements.alignment, memRequirements.memoryRequirements.memoryTypeBits};
}
