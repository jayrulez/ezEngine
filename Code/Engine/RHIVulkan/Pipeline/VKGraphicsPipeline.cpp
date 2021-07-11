#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Pipeline/VKGraphicsPipeline.h>
#include <RHIVulkan/Program/VKProgram.h>
#include <RHIVulkan/Utilities/VKUtility.h>
#include <map>

VkCompareOp Convert(ComparisonFunc func)
{
  switch (func)
  {
    case ComparisonFunc::kNever:
      return VkCompareOp::VK_COMPARE_OP_NEVER;
    case ComparisonFunc::kLess:
      return VkCompareOp::VK_COMPARE_OP_LESS;
    case ComparisonFunc::kEqual:
      return VkCompareOp::VK_COMPARE_OP_EQUAL;
    case ComparisonFunc::kLessEqual:
      return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
    case ComparisonFunc::kGreater:
      return VkCompareOp::VK_COMPARE_OP_GREATER;
    case ComparisonFunc::kNotEqual:
      return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;
    case ComparisonFunc::kGreaterEqual:
      return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;
    case ComparisonFunc::kAlways:
      return VkCompareOp::VK_COMPARE_OP_ALWAYS;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      return VkCompareOp::VK_COMPARE_OP_LESS;
  }
}

VkStencilOp Convert(StencilOp op)
{
  switch (op)
  {
    case StencilOp::kKeep:
      return VkStencilOp::VK_STENCIL_OP_KEEP;
    case StencilOp::kZero:
      return VkStencilOp::VK_STENCIL_OP_ZERO;
    case StencilOp::kReplace:
      return VkStencilOp::VK_STENCIL_OP_REPLACE;
    case StencilOp::kIncrSat:
      return VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    case StencilOp::kDecrSat:
      return VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_CLAMP;
    case StencilOp::kInvert:
      return VkStencilOp::VK_STENCIL_OP_INVERT;
    case StencilOp::kIncr:
      return VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case StencilOp::kDecr:
      return VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_WRAP;
    default:
      assert(false);
      return VkStencilOp::VK_STENCIL_OP_KEEP;
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
  vertex_input_info.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = (ezUInt32)m_binding_desc.size();
  vertex_input_info.pVertexBindingDescriptions = m_binding_desc.data();
  vertex_input_info.vertexAttributeDescriptionCount = (ezUInt32)m_attribute_desc.size();
  vertex_input_info.pVertexAttributeDescriptions = m_attribute_desc.data();

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.lineWidth = 1.0f;
  rasterizer.frontFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = m_desc.rasterizer_desc.depth_bias != 0;
  rasterizer.depthBiasConstantFactor = (float)m_desc.rasterizer_desc.depth_bias;
  switch (m_desc.rasterizer_desc.fill_mode)
  {
    case FillMode::kWireframe:
      rasterizer.polygonMode = VkPolygonMode::VK_POLYGON_MODE_LINE;
      break;
    case FillMode::kSolid:
      rasterizer.polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
      break;
  }
  switch (m_desc.rasterizer_desc.cull_mode)
  {
    case CullMode::kNone:
      rasterizer.cullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
      break;
    case CullMode::kFront:
      rasterizer.cullMode = VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
      break;
    case CullMode::kBack:
      rasterizer.cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
      break;
  }

  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask = VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = m_desc.blend_desc.blend_enable;

  if (color_blend_attachment.blendEnable)
  {
    auto convert = [](Blend type) {
      switch (type)
      {
        case Blend::kZero:
          return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
        case Blend::kSrcAlpha:
          return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
        case Blend::kInvSrcAlpha:
          return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      }
      throw std::runtime_error("unsupported");
    };

    auto convert_op = [](BlendOp type) {
      switch (type)
      {
        case BlendOp::kAdd:
          return VkBlendOp::VK_BLEND_OP_ADD;
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
  color_blending.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VkLogicOp::VK_LOGIC_OP_AND;
  color_blending.attachmentCount = (ezUInt32)color_blend_attachments.size();
  color_blending.pAttachments = color_blend_attachments.data();

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.rasterizationSamples = static_cast<VkSampleCountFlagBits>(render_pass_desc.sample_count);
  multisampling.sampleShadingEnable = multisampling.rasterizationSamples != VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

  VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
  depth_stencil.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = m_desc.depth_stencil_desc.depth_test_enable;
  depth_stencil.depthWriteEnable = m_desc.depth_stencil_desc.depth_write_enable;
  depth_stencil.depthCompareOp = Convert(m_desc.depth_stencil_desc.depth_func);
  depth_stencil.depthBoundsTestEnable = m_desc.depth_stencil_desc.depth_bounds_test_enable;
  depth_stencil.stencilTestEnable = m_desc.depth_stencil_desc.stencil_enable;
  depth_stencil.back = Convert(m_desc.depth_stencil_desc.back_face, m_desc.depth_stencil_desc.stencil_read_mask, m_desc.depth_stencil_desc.stencil_write_mask);
  depth_stencil.front = Convert(m_desc.depth_stencil_desc.front_face, m_desc.depth_stencil_desc.stencil_read_mask, m_desc.depth_stencil_desc.stencil_write_mask);

  std::vector<VkDynamicState> dynamic_state_enables = {
    VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT,
    VkDynamicState::VK_DYNAMIC_STATE_SCISSOR,
    VkDynamicState::VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR,
  };
  VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
  pipelineDynamicStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  pipelineDynamicStateCreateInfo.pDynamicStates = dynamic_state_enables.data();
  pipelineDynamicStateCreateInfo.dynamicStateCount = (ezUInt32)dynamic_state_enables.size();

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
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


  vkCreateGraphicsPipelines(m_device.GetDevice(), {}, 1, &pipeline_info, nullptr, &m_pipeline);
}

VKGraphicsPipeline::~VKGraphicsPipeline()
{
  vkDestroyPipeline(m_device.GetDevice(), m_pipeline, nullptr);
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
    decltype(auto) attribute = m_attribute_desc.emplace_back();
    attribute.location = m_desc.program->GetShader(ShaderType::kVertex)->GetInputLayoutLocation(vertex.semantic_name);
    attribute.binding = binding.binding = vertex.slot;
    binding.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
    binding.stride = vertex.stride;
    attribute.format = VKUtils::ToVkFormat(vertex.format);
  }
}
