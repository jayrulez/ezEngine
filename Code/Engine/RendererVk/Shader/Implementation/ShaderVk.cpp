#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Shader/ShaderVk.h>

ezGALShaderVk::ezGALShaderVk(const ezGALShaderCreationDescription& Description)
  : ezGALShader(Description)
  , m_pDeviceVk{nullptr}
  , m_pVkShaderModule{VK_NULL_HANDLE}
  , m_VkShaderStage{}
{
}

ezGALShaderVk::~ezGALShaderVk() {}

void ezGALShaderVk::SetDebugName(const char* szName) const
{
}

ezResult ezGALShaderVk::InitPlatform(ezGALDevice* pDevice)
{
  m_pDeviceVk = static_cast<ezGALDeviceVk*>(pDevice);

  const ezUInt32* shaderByteCode = nullptr;
  ezUInt32 shaderByteCodeSize = 0;

  if (m_Description.HasByteCodeForStage(ezGALShaderStage::VertexShader))
  {
    shaderByteCode = reinterpret_cast<const ezUInt32*>(m_Description.m_ByteCodes[ezGALShaderStage::VertexShader]->GetByteCode());
    shaderByteCodeSize = m_Description.m_ByteCodes[ezGALShaderStage::VertexShader]->GetSize();
    m_VkShaderStage = VK_SHADER_STAGE_VERTEX_BIT;
  }
  else if (m_Description.HasByteCodeForStage(ezGALShaderStage::HullShader))
  {
    shaderByteCode = reinterpret_cast<const ezUInt32*>(m_Description.m_ByteCodes[ezGALShaderStage::HullShader]->GetByteCode());
    shaderByteCodeSize = m_Description.m_ByteCodes[ezGALShaderStage::HullShader]->GetSize();
    m_VkShaderStage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
  }
  else if (m_Description.HasByteCodeForStage(ezGALShaderStage::DomainShader))
  {
    shaderByteCode = reinterpret_cast<const ezUInt32*>(m_Description.m_ByteCodes[ezGALShaderStage::DomainShader]->GetByteCode());
    shaderByteCodeSize = m_Description.m_ByteCodes[ezGALShaderStage::DomainShader]->GetSize();
    m_VkShaderStage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
  }
  else if (m_Description.HasByteCodeForStage(ezGALShaderStage::GeometryShader))
  {
    shaderByteCode = reinterpret_cast<const ezUInt32*>(m_Description.m_ByteCodes[ezGALShaderStage::GeometryShader]->GetByteCode());
    shaderByteCodeSize = m_Description.m_ByteCodes[ezGALShaderStage::GeometryShader]->GetSize();
    m_VkShaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;
  }
  else if (m_Description.HasByteCodeForStage(ezGALShaderStage::PixelShader))
  {
    shaderByteCode = reinterpret_cast<const ezUInt32*>(m_Description.m_ByteCodes[ezGALShaderStage::PixelShader]->GetByteCode());
    shaderByteCodeSize = m_Description.m_ByteCodes[ezGALShaderStage::PixelShader]->GetSize();
    m_VkShaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
  }
  else if (m_Description.HasByteCodeForStage(ezGALShaderStage::ComputeShader))
  {
    shaderByteCode = reinterpret_cast<const ezUInt32*>(m_Description.m_ByteCodes[ezGALShaderStage::ComputeShader]->GetByteCode());
    shaderByteCodeSize = m_Description.m_ByteCodes[ezGALShaderStage::ComputeShader]->GetSize();
    m_VkShaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
  }
  else
  {
    ezLog::Error("No bytecode specified for any supported shader stages.");
    return EZ_FAILURE;
  }

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = shaderByteCodeSize;
  createInfo.pCode = shaderByteCode;

  if (vkCreateShaderModule(m_pDeviceVk->GetVkDevice(), &createInfo, nullptr, &m_pVkShaderModule) != VK_SUCCESS)
  {
    ezLog::Error("Failed to create shader module.");
    return EZ_FAILURE;
  }

  return EZ_SUCCESS;
}

ezResult ezGALShaderVk::DeInitPlatform(ezGALDevice* pDevice)
{
  if (m_pDeviceVk)
  {
    vkDestroyShaderModule(m_pDeviceVk->GetVkDevice(), m_pVkShaderModule, nullptr);
  }
  return EZ_SUCCESS;
}



EZ_STATICLINK_FILE(RendererVk, RendererVk_Shader_Implementation_ShaderVk);
