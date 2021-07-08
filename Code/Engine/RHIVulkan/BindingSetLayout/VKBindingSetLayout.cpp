#include <RHIVulkanPCH.h>

#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/Device/VKDevice.h>

vk::DescriptorType GetDescriptorType(ezRHIViewType viewType)
{
  switch (viewType)
  {
    case ezRHIViewType::ConstantBuffer:
      return vk::DescriptorType::eUniformBuffer;
    case ezRHIViewType::Sampler:
      return vk::DescriptorType::eSampler;
    case ezRHIViewType::Texture:
      return vk::DescriptorType::eSampledImage;
    case ezRHIViewType::RWTexture:
      return vk::DescriptorType::eStorageImage;
    case ezRHIViewType::Buffer:
      return vk::DescriptorType::eUniformTexelBuffer;
    case ezRHIViewType::RWBuffer:
      return vk::DescriptorType::eStorageTexelBuffer;
    case ezRHIViewType::StructuredBuffer:
      return vk::DescriptorType::eStorageBuffer;
    case ezRHIViewType::RWStructuredBuffer:
      return vk::DescriptorType::eStorageBuffer;
    case ezRHIViewType::AccelerationStructure:
      return vk::DescriptorType::eAccelerationStructureKHR;
    default:
      break;
  }
  EZ_ASSERT_NOT_IMPLEMENTED;
  return {};
}

vk::ShaderStageFlagBits ShaderType2Bit(ezRHIShaderType type)
{
  switch (type)
  {
    case ezRHIShaderType::Vertex:
      return vk::ShaderStageFlagBits::eVertex;
    case ezRHIShaderType::Pixel:
      return vk::ShaderStageFlagBits::eFragment;
    case ezRHIShaderType::Geometry:
      return vk::ShaderStageFlagBits::eGeometry;
    case ezRHIShaderType::Compute:
      return vk::ShaderStageFlagBits::eCompute;
    case ezRHIShaderType::Amplification:
      return vk::ShaderStageFlagBits::eTaskNV;
    case ezRHIShaderType::Mesh:
      return vk::ShaderStageFlagBits::eMeshNV;
    case ezRHIShaderType::Library:
      return vk::ShaderStageFlagBits::eAll;
  }
  EZ_ASSERT_NOT_IMPLEMENTED;
  return {};
}

ezRHIVKBindingSetLayout::ezRHIVKBindingSetLayout(ezRHIVKDevice& device, const ezDynamicArray<ezRHIBindKeyDescription>& descs)
{
  ezMap<ezUInt32, ezDynamicArray<vk::DescriptorSetLayoutBinding>> bindingsBySet;
  ezMap<ezUInt32, ezDynamicArray<vk::DescriptorBindingFlags>> bindingsFlagsBySet;

  for (const ezRHIBindKeyDescription& bindKey : descs)
  {
    vk::DescriptorSetLayoutBinding& binding = bindingsBySet[bindKey.m_Space].ExpandAndGetRef();
    binding.binding = bindKey.m_Slot;
    binding.descriptorType = GetDescriptorType(bindKey.m_ViewType);
    binding.descriptorCount = bindKey.m_Count;
    binding.stageFlags = ShaderType2Bit(bindKey.m_ShaderType);

    vk::DescriptorBindingFlags& bindingFlag = bindingsFlagsBySet[bindKey.m_Space].ExpandAndGetRef();
    if (bindKey.m_Count == ezMath::MaxValue<ezUInt32>())
    {
      binding.descriptorCount = MAX_BINDLESS_HEAP_SIZE;
      bindingFlag = vk::DescriptorBindingFlagBits::eVariableDescriptorCount;
      m_BindlessType.Insert(bindKey.m_Space, binding.descriptorType);
      binding.stageFlags = vk::ShaderStageFlagBits::eAll;
    }
  }

  for (const auto& setDesc : bindingsBySet)
  {
    vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.bindingCount = setDesc.Value().GetCount();
    layoutInfo.pBindings = setDesc.Value().GetData();

    vk::DescriptorSetLayoutBindingFlagsCreateInfo layoutFlagsInfo = {};
    layoutFlagsInfo.bindingCount = bindingsFlagsBySet[setDesc.Key()].GetCount();
    layoutFlagsInfo.pBindingFlags = bindingsFlagsBySet[setDesc.Key()].GetData();
    layoutInfo.pNext = &layoutFlagsInfo;

    ezUInt32 setNum = setDesc.Key();
    if (m_DescriptorSetLayouts.GetCount() <= setNum)
    {
      m_DescriptorSetLayouts.SetCount(setNum + 1);
      m_DescriptorCountBySet.SetCount(setNum + 1);
    }

    vk::UniqueDescriptorSetLayout& descriptorSetLayout = m_DescriptorSetLayouts[setNum];
    descriptorSetLayout = device.GetDevice().createDescriptorSetLayoutUnique(layoutInfo);

    ezMap<vk::DescriptorType, ezUInt32>& descriptorCount = m_DescriptorCountBySet[setNum];
    for (const auto& binding : setDesc.Value())
    {
      descriptorCount[binding.descriptorType] += binding.descriptorCount;
    }
  }

  ezDynamicArray<vk::DescriptorSetLayout> descriptorSetLayouts;
  for (auto& descriptorSetLayout : m_DescriptorSetLayouts)
  {
    if (!descriptorSetLayout)
    {
      vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
      descriptorSetLayout = device.GetDevice().createDescriptorSetLayoutUnique(layoutInfo);
    }

    descriptorSetLayouts.PushBack(descriptorSetLayout.get());
  }

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.GetCount();
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.GetData();

  m_PipelineLayout = device.GetDevice().createPipelineLayoutUnique(pipelineLayoutInfo);
}

const ezMap<ezUInt32, vk::DescriptorType>& ezRHIVKBindingSetLayout::GetBindlessType() const
{
  return m_BindlessType;
}

const ezDynamicArray<vk::UniqueDescriptorSetLayout>& ezRHIVKBindingSetLayout::GetDescriptorSetLayouts() const
{
  return m_DescriptorSetLayouts;
}

const ezDynamicArray<ezMap<vk::DescriptorType, ezUInt32>>& ezRHIVKBindingSetLayout::GetDescriptorCountBySet() const
{
  return m_DescriptorCountBySet;
}

vk::PipelineLayout ezRHIVKBindingSetLayout::GetPipelineLayout() const
{
  return m_PipelineLayout.get();
}
