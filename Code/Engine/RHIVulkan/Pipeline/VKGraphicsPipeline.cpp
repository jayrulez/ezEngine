#include <RHIVulkan/Pipeline/VKGraphicsPipeline.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Program/VKProgram.h>
#include <RHIVulkan/Utilities/VKUtility.h>
#include <map>

VkCompareOp Convert(ComparisonFunc func)
{
    switch (func)
    {
    case ComparisonFunc::kNever:
        return VkCompareOp::eNever;
    case ComparisonFunc::kLess:
        return VkCompareOp::eLess;
    case ComparisonFunc::kEqual:
        return VkCompareOp::eEqual;
    case ComparisonFunc::kLessEqual:
        return VkCompareOp::eLessOrEqual;
    case ComparisonFunc::kGreater:
        return VkCompareOp::eGreater;
    case ComparisonFunc::kNotEqual:
        return VkCompareOp::eNotEqual;
    case ComparisonFunc::kGreaterEqual:
        return VkCompareOp::eGreaterOrEqual;
    case ComparisonFunc::kAlways:
        return VkCompareOp::eAlways;
    default:
        assert(false);
        return VkCompareOp::eLess;
    }
}

VkStencilOp Convert(StencilOp op)
{
    switch (op)
    {
    case StencilOp::kKeep:
        return VkStencilOp::eKeep;
    case StencilOp::kZero:
        return VkStencilOp::eZero;
    case StencilOp::kReplace:
        return VkStencilOp::eReplace;
    case StencilOp::kIncrSat:
        return VkStencilOp::eIncrementAndClamp;
    case StencilOp::kDecrSat:
        return VkStencilOp::eDecrementAndClamp;
    case StencilOp::kInvert:
        return VkStencilOp::eInvert;
    case StencilOp::kIncr:
        return VkStencilOp::eIncrementAndWrap;
    case StencilOp::kDecr:
        return VkStencilOp::eDecrementAndWrap;
    default:
        assert(false);
        return VkStencilOp::eKeep;
    }
}

VkStencilOpState Convert(const StencilOpDesc& desc, uint8_t read_mask, uint8_t write_mask)
{
    VkStencilOpState res = {};
    res.failOp = Convert(desc.fail_op);
    res.passOp = Convert(desc.pass_op);
    res.depthFailOp = Convert(desc.depth_fail_op);
    res.compareOp = Convert(desc.func);
    res.compareMask = read_mask;
    res.writeMask = write_mask;
    return res;
}

VKGraphicsPipeline::VKGraphicsPipeline(VKDevice& device, const GraphicsPipelineDesc& desc)
    : VKPipeline(device, desc.program, desc.layout)
    , m_desc(desc)
{
    if (desc.program->HasShader(ShaderType::kVertex))
    {
        CreateInputLayout(m_binding_desc, m_attribute_desc);
    }

    const RenderPassDesc& render_pass_desc = m_desc.render_pass->GetDesc();

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.vertexBindingDescriptionCount = (ezUInt32)m_binding_desc.size();
    vertex_input_info.pVertexBindingDescriptions = m_binding_desc.data();
    vertex_input_info.vertexAttributeDescriptionCount = (ezUInt32)m_attribute_desc.size();
    vertex_input_info.pVertexAttributeDescriptions = m_attribute_desc.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    input_assembly.topology = VkPrimitiveTopology::eTriangleList;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
    rasterizer.frontFace = VkFrontFace::eClockwise;
    rasterizer.depthBiasEnable = m_desc.rasterizer_desc.depth_bias != 0;
    rasterizer.depthBiasConstantFactor = (float)m_desc.rasterizer_desc.depth_bias;
    switch (m_desc.rasterizer_desc.fill_mode)
    {
    case FillMode::kWireframe:
        rasterizer.polygonMode = VkPolygonMode::eLine;
        break;
    case FillMode::kSolid:
        rasterizer.polygonMode = VkPolygonMode::eFill;
        break;
    }
    switch (m_desc.rasterizer_desc.cull_mode)
    {
    case CullMode::kNone:
        rasterizer.cullMode = VkCullModeFlagBits::eNone;
        break;
    case CullMode::kFront:
        rasterizer.cullMode = VkCullModeFlagBits::eFront;
        break;
    case CullMode::kBack:
        rasterizer.cullMode = VkCullModeFlagBits::eBack;
        break;
    }

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask = VkColorComponentFlagBits::eR | VkColorComponentFlagBits::eG | VkColorComponentFlagBits::eB | VkColorComponentFlagBits::eA;
    color_blend_attachment.blendEnable = m_desc.blend_desc.blend_enable;

    if (color_blend_attachment.blendEnable)
    {
        auto convert = [](Blend type)
        {
            switch (type)
            {
            case Blend::kZero:
                return VkBlendFactor::eZero;
            case Blend::kSrcAlpha:
                return VkBlendFactor::eSrcAlpha;
            case Blend::kInvSrcAlpha:
                return VkBlendFactor::eOneMinusSrcAlpha;
            }
            throw std::runtime_error("unsupported");
        };

        auto convert_op = [](BlendOp type)
        {
            switch (type)
            {
            case BlendOp::kAdd:
                return VkBlendOp::eAdd;
            }
            throw std::runtime_error("unsupported");
        };

        color_blend_attachment.srcColorBlendFactor = convert(m_desc.blend_desc.blend_src);
        color_blend_attachment.dstColorBlendFactor = convert(m_desc.blend_desc.blend_dest);
        color_blend_attachment.colorBlendOp = convert_op(m_desc.blend_desc.blend_op);
        color_blend_attachment.srcAlphaBlendFactor = convert(m_desc.blend_desc.blend_src_alpha);
        color_blend_attachment.dstAlphaBlendFactor = convert(m_desc.blend_desc.blend_dest_apha);
        color_blend_attachment.alphaBlendOp = convert_op(m_desc.blend_desc.blend_op_alpha);
    }

    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments(render_pass_desc.colors.size(), color_blend_attachment);

    VkPipelineColorBlendStateCreateInfo color_blending = {};
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VkLogicOp::eAnd;
    color_blending.attachmentCount = (ezUInt32)color_blend_attachments.size();
    color_blending.pAttachments = color_blend_attachments.data();

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.rasterizationSamples = static_cast<VkSampleCountFlagBits>(render_pass_desc.sample_count);
    multisampling.sampleShadingEnable = multisampling.rasterizationSamples != VkSampleCountFlagBits::e1;

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
    depth_stencil.depthTestEnable = m_desc.depth_stencil_desc.depth_test_enable;
    depth_stencil.depthWriteEnable = m_desc.depth_stencil_desc.depth_write_enable;
    depth_stencil.depthCompareOp = Convert(m_desc.depth_stencil_desc.depth_func);
    depth_stencil.depthBoundsTestEnable = m_desc.depth_stencil_desc.depth_bounds_test_enable;
    depth_stencil.stencilTestEnable = m_desc.depth_stencil_desc.stencil_enable;
    depth_stencil.back = Convert(m_desc.depth_stencil_desc.back_face, m_desc.depth_stencil_desc.stencil_read_mask, m_desc.depth_stencil_desc.stencil_write_mask);
    depth_stencil.front = Convert(m_desc.depth_stencil_desc.front_face, m_desc.depth_stencil_desc.stencil_read_mask, m_desc.depth_stencil_desc.stencil_write_mask);

    std::vector<VkDynamicState> dynamic_state_enables = {
        VkDynamicState::eViewport,
        VkDynamicState::eScissor,
        VkDynamicState::eFragmentShadingRateKHR,
    };
    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
    pipelineDynamicStateCreateInfo.pDynamicStates = dynamic_state_enables.data();
    pipelineDynamicStateCreateInfo.dynamicStateCount = (ezUInt32)dynamic_state_enables.size();

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.stageCount = (ezUInt32)m_shader_stage_create_info.size();
    pipeline_info.pStages = m_shader_stage_create_info.data();
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.layout = m_pipeline_layout;
    pipeline_info.renderPass = GetRenderPass();
    pipeline_info.pDynamicState = &pipelineDynamicStateCreateInfo;

    m_pipeline = m_device.GetDevice().createGraphicsPipelineUnique({}, pipeline_info).value;
}

PipelineType VKGraphicsPipeline::GetPipelineType() const
{
    return PipelineType::kGraphics;
}

VkRenderPass VKGraphicsPipeline::GetRenderPass() const
{
    return m_desc.render_pass->As<VKRenderPass>().GetRenderPass();
}

void VKGraphicsPipeline::CreateInputLayout(std::vector<VkVertexInputBindingDescription>& m_binding_desc, std::vector<VkVertexInputAttributeDescription>& m_attribute_desc)
{
    for (auto& vertex : m_desc.input)
    {
        decltype(auto) binding = m_binding_desc.emplace_back();
        decltype(auto) attribute =  m_attribute_desc.emplace_back();
        attribute.location = m_desc.program->GetShader(ShaderType::kVertex)->GetInputLayoutLocation(vertex.semantic_name);
        attribute.binding = binding.binding = vertex.slot;
        binding.inputRate = VkVertexInputRate::eVertex;
        binding.stride = vertex.stride;
        attribute.format = VKUtils::ToVkFormat(vertex.format);
    }
}
