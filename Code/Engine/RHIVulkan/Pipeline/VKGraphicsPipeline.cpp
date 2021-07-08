#include <RHIVulkanPCH.h>

#include <RHIVulkan/Pipeline/VKGraphicsPipeline.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>
#include <RHIVulkan/Program/VKProgram.h>
#include <RHI/Shader/Shader.h>
#include <RHIVulkan/Device/VKDevice.h>

vk::CompareOp Convert(ezRHIComparisonFunc func)
{
  switch (func)
  {
    case ezRHIComparisonFunc::Never:
      return vk::CompareOp::eNever;
    case ezRHIComparisonFunc::Less:
      return vk::CompareOp::eLess;
    case ezRHIComparisonFunc::Equal:
      return vk::CompareOp::eEqual;
    case ezRHIComparisonFunc::LessEqual:
      return vk::CompareOp::eLessOrEqual;
    case ezRHIComparisonFunc::Greater:
      return vk::CompareOp::eGreater;
    case ezRHIComparisonFunc::NotEqual:
      return vk::CompareOp::eNotEqual;
    case ezRHIComparisonFunc::GreaterEqual:
      return vk::CompareOp::eGreaterOrEqual;
    case ezRHIComparisonFunc::Always:
      return vk::CompareOp::eAlways;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      return vk::CompareOp::eLess;
  }
}

vk::StencilOp Convert(ezRHIStencilOp op)
{
  switch (op)
  {
    case ezRHIStencilOp::Keep:
      return vk::StencilOp::eKeep;
    case ezRHIStencilOp::Zero:
      return vk::StencilOp::eZero;
    case ezRHIStencilOp::Replace:
      return vk::StencilOp::eReplace;
    case ezRHIStencilOp::IncrementSaturated:
      return vk::StencilOp::eIncrementAndClamp;
    case ezRHIStencilOp::DecrementSaturated:
      return vk::StencilOp::eDecrementAndClamp;
    case ezRHIStencilOp::Invert:
      return vk::StencilOp::eInvert;
    case ezRHIStencilOp::Increment:
      return vk::StencilOp::eIncrementAndWrap;
    case ezRHIStencilOp::Decrement:
      return vk::StencilOp::eDecrementAndWrap;
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      return vk::StencilOp::eKeep;
  }
}

vk::StencilOpState Convert(const ezRHIStencilOpDescription& desc, ezUInt8 readMask, ezUInt8 writeMask)
{
  vk::StencilOpState res = {};
  res.failOp = Convert(desc.m_FailOp);
  res.passOp = Convert(desc.m_PassOp);
  res.depthFailOp = Convert(desc.m_DepthFailOp);
  res.compareOp = Convert(desc.m_Func);
  res.compareMask = readMask;
  res.writeMask = writeMask;
  return res;
}

ezRHIVKGraphicsPipeline::ezRHIVKGraphicsPipeline(ezRHIVKDevice& device, const ezRHIGraphicsPipelineCreationDescription& desc)
  : ezRHIVKPipeline(device, desc.m_Program, desc.m_Layout)
  , m_Desc{desc}
{
  if (desc.m_Program->HasShader(ezRHIShaderType::Vertex))
  {
    CreateInputLayout(m_BindingDesc, m_AttributeDesc);
  }

  const ezRHIRenderPassCreationDescription& renderPassDesc = m_Desc.m_RenderPass->GetDescription();

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.vertexBindingDescriptionCount = m_BindingDesc.GetCount();
  vertexInputInfo.pVertexBindingDescriptions = m_BindingDesc.GetData();
  vertexInputInfo.vertexAttributeDescriptionCount = m_AttributeDesc.GetCount();
  vertexInputInfo.pVertexAttributeDescriptions = m_AttributeDesc.GetData();

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  vk::PipelineViewportStateCreateInfo viewportState = {};
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  vk::PipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.lineWidth = 1.0f;
  rasterizer.frontFace = vk::FrontFace::eClockwise;
  rasterizer.depthBiasEnable = m_Desc.m_RasterizerDesc.m_DepthBias != 0;
  rasterizer.depthBiasConstantFactor = (float)m_Desc.m_RasterizerDesc.m_DepthBias;
  switch (m_Desc.m_RasterizerDesc.m_FillMode)
  {
    case ezRHIFillMode::Wireframe:
      rasterizer.polygonMode = vk::PolygonMode::eLine;
      break;
    case ezRHIFillMode::Solid:
      rasterizer.polygonMode = vk::PolygonMode::eFill;
      break;
  }
  switch (m_Desc.m_RasterizerDesc.m_CullMode)
  {
    case ezRHICullMode::None:
      rasterizer.cullMode = vk::CullModeFlagBits::eNone;
      break;
    case ezRHICullMode::Front:
      rasterizer.cullMode = vk::CullModeFlagBits::eFront;
      break;
    case ezRHICullMode::Back:
      rasterizer.cullMode = vk::CullModeFlagBits::eBack;
      break;
  }

  vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
  colorBlendAttachment.blendEnable = m_Desc.m_BlendDesc.m_BlendEnable;

  if (colorBlendAttachment.blendEnable)
  {
    auto convert = [](ezRHIBlend type) {
      switch (type)
      {
        case ezRHIBlend::Zero:
          return vk::BlendFactor::eZero;
        case ezRHIBlend::SrcAlpha:
          return vk::BlendFactor::eSrcAlpha;
        case ezRHIBlend::InvSrcAlpha:
          return vk::BlendFactor::eOneMinusSrcAlpha;
      }
      EZ_REPORT_FAILURE("unsupported");
      return vk::BlendFactor::eZero;
    };

    auto convert_op = [](ezRHIBlendOp type) {
      switch (type)
      {
        case ezRHIBlendOp::Add:
          return vk::BlendOp::eAdd;
      }
      EZ_REPORT_FAILURE("unsupported");
    };

    colorBlendAttachment.srcColorBlendFactor = convert(m_Desc.m_BlendDesc.m_BlendSrc);
    colorBlendAttachment.dstColorBlendFactor = convert(m_Desc.m_BlendDesc.m_BlendDest);
    colorBlendAttachment.colorBlendOp = convert_op(m_Desc.m_BlendDesc.m_BlendOp);
    colorBlendAttachment.srcAlphaBlendFactor = convert(m_Desc.m_BlendDesc.m_BlendSrcAlpha);
    colorBlendAttachment.dstAlphaBlendFactor = convert(m_Desc.m_BlendDesc.m_BlendDestApha);
    colorBlendAttachment.alphaBlendOp = convert_op(m_Desc.m_BlendDesc.m_BlendOpAlpha);
  }

  ezDynamicArray<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
  colorBlendAttachments.SetCount(renderPassDesc.m_Colors.GetCount());
  for (ezUInt32 i = 0; i < colorBlendAttachments.GetCount(); i++)
  {
    colorBlendAttachments[i] = colorBlendAttachment;
  }

  vk::PipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = vk::LogicOp::eAnd;
  colorBlending.attachmentCount = colorBlendAttachments.GetCount();
  colorBlending.pAttachments = colorBlendAttachments.GetData();

  vk::PipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.rasterizationSamples = static_cast<vk::SampleCountFlagBits>(renderPassDesc.m_SampleCount);
  multisampling.sampleShadingEnable = multisampling.rasterizationSamples != vk::SampleCountFlagBits::e1;

  vk::PipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.depthTestEnable = m_Desc.m_DepthStencilDesc.m_DepthTestEnable;
  depthStencil.depthWriteEnable = m_Desc.m_DepthStencilDesc.m_DepthWriteEnable;
  depthStencil.depthCompareOp = Convert(m_Desc.m_DepthStencilDesc.m_DepthFunc);
  depthStencil.depthBoundsTestEnable = m_Desc.m_DepthStencilDesc.m_DepthBoundsTestEnable;
  depthStencil.stencilTestEnable = m_Desc.m_DepthStencilDesc.m_StencilEnable;
  depthStencil.back = Convert(m_Desc.m_DepthStencilDesc.m_BackFace, m_Desc.m_DepthStencilDesc.m_StencilReadMask, m_Desc.m_DepthStencilDesc.m_StencilWriteMask);
  depthStencil.front = Convert(m_Desc.m_DepthStencilDesc.m_FrontFace, m_Desc.m_DepthStencilDesc.m_StencilReadMask, m_Desc.m_DepthStencilDesc.m_StencilWriteMask);

  ezDynamicArray<vk::DynamicState> dynamicStateEnables;
  dynamicStateEnables.PushBack(vk::DynamicState::eViewport);
  dynamicStateEnables.PushBack(vk::DynamicState::eScissor);
  dynamicStateEnables.PushBack(vk::DynamicState::eFragmentShadingRateKHR);

  vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
  pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.GetData();
  pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateEnables.GetCount();

  vk::GraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.stageCount = m_ShaderStageCreateInfo.GetCount();
  pipelineInfo.pStages = m_ShaderStageCreateInfo.GetData();
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.layout = m_PipelineLayout;
  pipelineInfo.renderPass = GetRenderPass();
  pipelineInfo.pDynamicState = &pipelineDynamicStateCreateInfo;

  m_Pipeline = m_Device.GetDevice().createGraphicsPipelineUnique({}, pipelineInfo).value;
}

ezRHIPipelineType ezRHIVKGraphicsPipeline::GetPipelineType() const
{
  return ezRHIPipelineType::Graphics;
}

vk::RenderPass ezRHIVKGraphicsPipeline::GetRenderPass() const
{
  return m_Desc.m_RenderPass->As<ezRHIVKRenderPass>()->GetRenderPass();
}

void ezRHIVKGraphicsPipeline::CreateInputLayout(ezDynamicArray<vk::VertexInputBindingDescription>& bindingDesc, ezDynamicArray<vk::VertexInputAttributeDescription>& attributeDesc)
{
  for (auto& vertex : m_Desc.m_Input)
  {
    decltype(auto) binding = m_BindingDesc.ExpandAndGetRef();
    decltype(auto) attribute = m_AttributeDesc.ExpandAndGetRef();

    attribute.location = m_Desc.m_Program->GetShader(ezRHIShaderType::Vertex)->GetInputLayoutLocation(vertex.m_SemanticName);
    attribute.binding = binding.binding = vertex.m_Slot;
    binding.inputRate = vk::VertexInputRate::eVertex;
    binding.stride = vertex.m_Stride;
    attribute.format = VKUtilities::ToVKFormat(vertex.m_Format);
  }
}
