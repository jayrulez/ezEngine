#include <RHIVulkan/Pipeline/VKPipeline.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/BindingSetLayout/VKBindingSetLayout.h>

VkShaderStageFlagBits ExecutionModel2Bit(ShaderKind kind)
{
    switch (kind)
    {
    case ShaderKind::kVertex:
        return VkShaderStageFlagBits::eVertex;
    case ShaderKind::kPixel:
        return VkShaderStageFlagBits::eFragment;
    case ShaderKind::kCompute:
        return VkShaderStageFlagBits::eCompute;
    case ShaderKind::kGeometry:
        return VkShaderStageFlagBits::eGeometry;
    case ShaderKind::kAmplification:
        return VkShaderStageFlagBits::eTaskNV;
    case ShaderKind::kMesh:
        return VkShaderStageFlagBits::eMeshNV;
    case ShaderKind::kRayGeneration:
        return VkShaderStageFlagBits::eRaygenKHR;
    case ShaderKind::kIntersection:
        return VkShaderStageFlagBits::eIntersectionKHR;
    case ShaderKind::kAnyHit:
        return VkShaderStageFlagBits::eAnyHitKHR;
    case ShaderKind::kClosestHit:
        return VkShaderStageFlagBits::eClosestHitKHR;
    case ShaderKind::kMiss:
        return VkShaderStageFlagBits::eMissKHR;
    case ShaderKind::kCallable:
        return VkShaderStageFlagBits::eCallableKHR;
    }
    assert(false);
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
        shader_module_info.codeSize = blob.size();
        shader_module_info.pCode = (uint32_t*)blob.data();
        m_shader_modules.emplace_back(m_device.GetDevice().createShaderModuleUnique(shader_module_info));

        decltype(auto) reflection = shader->GetReflection();
        decltype(auto) entry_points = reflection->GetEntryPoints();
        for (const auto& entry_point : entry_points)
        {
          m_shader_ids[shader->GetId(entry_point.name)] = (ezUInt32)m_shader_stage_create_info.size();
            decltype(auto) shader_stage_create_info = m_shader_stage_create_info.emplace_back();
            shader_stage_create_info.stage = ExecutionModel2Bit(entry_point.kind);
            shader_stage_create_info.module = m_shader_modules.back().get();
            decltype(auto) name = entry_point_names.emplace_back(entry_point.name);
            shader_stage_create_info.pName = name.c_str();
        }
    }
}

VkPipeline VKPipeline::GetPipeline() const
{
    return m_pipeline.get();
}

VkPipelineLayout VKPipeline::GetPipelineLayout() const
{
    return m_pipeline_layout;
}

std::vector<uint8_t> VKPipeline::GetRayTracingShaderGroupHandles(uint32_t first_group, uint32_t group_count) const
{
    return {};
}
