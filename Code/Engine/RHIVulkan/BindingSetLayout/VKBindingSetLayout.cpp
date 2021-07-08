#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>
#include <RHIVulkan/Device/VKDevice.h>

VkDescriptorType GetDescriptorType(ViewType view_type)
{
  switch (view_type)
  {
    case ViewType::kConstantBuffer:
      return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case ViewType::kSampler:
      return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;
    case ViewType::kTexture:
      return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case ViewType::kRWTexture:
      return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case ViewType::kBuffer:
      return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    case ViewType::kRWBuffer:
      return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    case ViewType::kStructuredBuffer:
      return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case ViewType::kRWStructuredBuffer:
      return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case ViewType::kAccelerationStructure:
      return VkDescriptorType::VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    default:
      break;
  }
  assert(false);
  return {};
}

VkShaderStageFlagBits ShaderType2Bit(ShaderType type)
{
  switch (type)
  {
    case ShaderType::kVertex:
      return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
    case ShaderType::kPixel:
      return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
    case ShaderType::kGeometry:
      return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
    case ShaderType::kCompute:
      return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
    case ShaderType::kAmplification:
      return VkShaderStageFlagBits::VK_SHADER_STAGE_TASK_BIT_NV;
    case ShaderType::kMesh:
      return VkShaderStageFlagBits::VK_SHADER_STAGE_MESH_BIT_NV;
    case ShaderType::kLibrary:
      return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
  }
  assert(false);
  return {};
}

VKBindingSetLayout::VKBindingSetLayout(VKDevice& device, const std::vector<BindKey>& descs)
{
  std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> bindings_by_set;
  std::map<uint32_t, std::vector<VkDescriptorBindingFlags>> bindings_flags_by_set;

  for (const auto& bind_key : descs)
  {
    decltype(auto) binding = bindings_by_set[bind_key.space].emplace_back();
    binding.binding = bind_key.slot;
    binding.descriptorType = GetDescriptorType(bind_key.view_type);
    binding.descriptorCount = bind_key.count;
    binding.stageFlags = ShaderType2Bit(bind_key.shader_type);

    decltype(auto) binding_flag = bindings_flags_by_set[bind_key.space].emplace_back();
    if (bind_key.count == ezMath::MaxValue<ezUInt32>())
    {
      binding.descriptorCount = max_bindless_heap_size;
      binding_flag = VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
      m_bindless_type.emplace(bind_key.space, binding.descriptorType);
      binding.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
    }
  }

  for (const auto& set_desc : bindings_by_set)
  {
    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = (ezUInt32)set_desc.second.size();
    layout_info.pBindings = set_desc.second.data();

    VkDescriptorSetLayoutBindingFlagsCreateInfo layout_flags_info = {};
    layout_flags_info.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    layout_flags_info.bindingCount = (ezUInt32)bindings_flags_by_set[set_desc.first].size();
    layout_flags_info.pBindingFlags = bindings_flags_by_set[set_desc.first].data();
    layout_info.pNext = &layout_flags_info;

    size_t set_num = set_desc.first;
    if (m_descriptor_set_layouts.size() <= set_num)
    {
      m_descriptor_set_layouts.resize(set_num + 1);
      m_descriptor_count_by_set.resize(set_num + 1);
    }

    decltype(auto) descriptor_set_layout = m_descriptor_set_layouts[set_num];

    vkCreateDescriptorSetLayout(device.GetDevice(), &layout_info, nullptr, &descriptor_set_layout);

    decltype(auto) descriptor_count = m_descriptor_count_by_set[set_num];
    for (const auto& binding : set_desc.second)
    {
      descriptor_count[binding.descriptorType] += binding.descriptorCount;
    }
  }

  std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
  for (auto& descriptor_set_layout : m_descriptor_set_layouts)
  {
    if (!descriptor_set_layout)
    {
      VkDescriptorSetLayoutCreateInfo layout_info = {};
      layout_info.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

      vkCreateDescriptorSetLayout(device.GetDevice(), &layout_info, nullptr, &descriptor_set_layout);
    }

    descriptor_set_layouts.emplace_back(descriptor_set_layout);
  }

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = (ezUInt32)descriptor_set_layouts.size();
  pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();

  vkCreatePipelineLayout(device.GetDevice(), &pipeline_layout_info, nullptr, &m_pipeline_layout);
}

const std::map<uint32_t, VkDescriptorType>& VKBindingSetLayout::GetBindlessType() const
{
  return m_bindless_type;
}

const std::vector<VkDescriptorSetLayout>& VKBindingSetLayout::GetDescriptorSetLayouts() const
{
  return m_descriptor_set_layouts;
}

const std::vector<std::map<VkDescriptorType, size_t>>& VKBindingSetLayout::GetDescriptorCountBySet() const
{
  return m_descriptor_count_by_set;
}

VkPipelineLayout VKBindingSetLayout::GetPipelineLayout() const
{
  return m_pipeline_layout;
}
