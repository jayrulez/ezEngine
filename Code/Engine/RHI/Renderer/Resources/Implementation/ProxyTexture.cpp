#include <RHIPCH.h>

#include <RHI/Renderer/Resources/ProxyTexture.h>

namespace
{
  ezRHITextureCreationDescription MakeProxyDesc(const ezRHITextureCreationDescription& parentDesc)
  {
    ezRHITextureCreationDescription desc = parentDesc;
    desc.m_Type = ezRHITextureType::Texture2DProxy;
    return desc;
  }
} // namespace

ezRHIProxyTexture::ezRHIProxyTexture(const ezRHITexture& parentTexture)
  : ezRHITexture(MakeProxyDesc(parentTexture.GetDescription()))
  , m_pParentTexture(&parentTexture)
{
}

ezRHIProxyTexture::~ezRHIProxyTexture() {}


const ezRHIResourceBase* ezRHIProxyTexture::GetParentResource() const
{
  return m_pParentTexture;
}

ezResult ezRHIProxyTexture::InitPlatform(ezRHIRenderDevice* pDevice, ezArrayPtr<ezRHISystemMemoryDescription> pInitialData)
{
  return EZ_SUCCESS;
}

ezResult ezRHIProxyTexture::DeInitPlatform(ezRHIRenderDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezResult ezRHIProxyTexture::ReplaceExisitingNativeObject(void* pExisitingNativeObject)
{
  EZ_ASSERT_NOT_IMPLEMENTED;
  return EZ_FAILURE;
}

void ezRHIProxyTexture::SetDebugNamePlatform(const char* szName) const {}


EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Resources_Implementation_ProxyTexture);
