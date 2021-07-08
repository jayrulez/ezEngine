#include <RHIVulkan/Pipeline/VKPipeline.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>

VkShaderStageFlagBits ExecutionModel2Bit(ShaderKind kind)
{
    switch (kind)
    {
    case ShaderKind::kVertex:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
    case ShaderKind::kPixel:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
    case ShaderKind::kCompute:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
    case ShaderKind::kGeometry:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
    case ShaderKind::kAmplification:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_TASK_BIT_NV;
    case ShaderKind::kMesh:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_MESH_BIT_NV;
    case ShaderKind::kRayGeneration:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    case ShaderKind::kIntersection:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
    case ShaderKind::kAnyHit:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    case ShaderKind::kClosestHit:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    case ShaderKind::kMiss:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_MISS_BIT_KHR;
    case ShaderKind::kCallable:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    }
    EZ_ASSERT_NOT_IMPLEMENTED;
    return {};
}

VKPipeline::VKPipeline(VKDevice& device, const std::shared_ptr<Program>& program, const std::shared_ptr<BindingSetLayout>& layout)
    : m_device(device)
{
    decltype(auto) vk_layout = layout->As<VKBindingSetLayout>();
    m_pipeline_layout = vk_layout.GetPipelineLayout();

    decltype(auto) shaders = program->GetShaders();
    for (const auto& shader : shaders)
    {
        decltype(auto) blob = shader->GetBlob();
        VkShaderModuleCreateInfo shader_module_info = {};
        shader_module_info.sType = VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_info.codeSize = blob.size();
        shader_module_info.pCode = (uint32_t*)blob.data();
        vkCreateShaderModule(m_device.GetDevice(), &shader_module_info, nullptr, &m_shader_modules.emplace_back());

        decltype(auto) reflection = shader->GetReflection();
        decltype(auto) entry_points = reflection->GetEntryPoints();
        for (const auto& entry_point : entry_points)
        {
          m_shader_ids[shader->GetId(entry_point.name)] = (ezUInt32)m_shader_stage_create_info.size();
            decltype(auto) shader_stage_create_info = m_shader_stage_create_info.emplace_back();
            shader_stage_create_info.stage = ExecutionModel2Bit(entry_point.kind);
            shader_stage_create_info.module = m_shader_modules.back();
            decltype(auto) name = entry_point_names.emplace_back(entry_point.name);
            shader_stage_create_info.pName = name.c_str();
        }
    }
}

VkPipeline VKPipeline::GetPipeline() const
{
    return m_pipeline;
}

VkPipelineLayout VKPipeline::GetPipelineLayout() const
{
    return m_pipeline_layout;
}

std::vector<uint8_t> VKPipeline::GetRayTracingShaderGroupHandles(uint32_t first_group, uint32_t group_count) const
{
    return {};
}
