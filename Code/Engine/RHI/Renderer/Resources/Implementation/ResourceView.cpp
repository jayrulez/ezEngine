#include <RHIPCH.h>

#include <RHI/Renderer/Resources/ResourceView.h>
#include <RHI/Renderer/Device/RenderDeviceImpl.h>


bool IsArrayView(const ezRHITextureCreationDescription& texDesc, const ezRHIResourceViewCreationDescription& viewDesc)
{
  return texDesc.m_uiArraySize > 1 || viewDesc.m_uiFirstArraySlice > 0;
}

ezRHIResourceView::ezRHIResourceView(ezRHIResourceBase* pResource, const ezRHIResourceViewCreationDescription& description)
  : ezRHIObject(description)
  , m_pResource(pResource)
{
  EZ_ASSERT_DEV(m_pResource != nullptr, "Resource must not be null");
}

ezRHIResourceView::~ezRHIResourceView() {}

ezResult ezRHIResourceView::InitPlatform(ezRHIRenderDevice* pDevice)
{
  const ezRHITexture* pTexture = nullptr;
  if (!m_Description.m_hTexture.IsInvalidated())
    pTexture = pDevice->GetTexture(m_Description.m_hTexture);

  const ezRHIBuffer* pBuffer = nullptr;
  if (!m_Description.m_hBuffer.IsInvalidated())
    pBuffer = pDevice->GetBuffer(m_Description.m_hBuffer);

  if (pTexture == nullptr && pBuffer == nullptr)
  {
    ezLog::Error("No valid texture handle or buffer handle given for resource view creation!");
    return EZ_FAILURE;
  }

  ezRHIResourceFormat::Enum ViewFormat = m_Description.m_OverrideViewFormat;

  if (pTexture != nullptr)
  {
    if (ViewFormat == ezRHIResourceFormat::Invalid)
      ViewFormat = pTexture->GetDescription().m_Format;
  }
  else if (pBuffer != nullptr)
  {
    if (ViewFormat == ezRHIResourceFormat::Invalid)
      ViewFormat = ezRHIResourceFormat::RUInt;

    if (!pBuffer->GetDescription().m_bAllowRawViews && m_Description.m_bRawView)
    {
      ezLog::Error("Trying to create a raw view for a buffer with no raw view flag is invalid!");
      return EZ_FAILURE;
    }
  }

  ezRHIRenderDeviceImpl& pDeviceImpl = pDevice->As<ezRHIRenderDeviceImpl>();

  // fill in here

  ViewDesc viewDesc{};
  //viewDesc.buffer_format = ViewFormat;

  if (pTexture)
  {
    const ezRHITextureCreationDescription& texDesc = pTexture->GetDescription();
    const bool bIsArrayView = IsArrayView(texDesc, m_Description);

    switch (texDesc.m_Type)
    {
      case ezRHITextureType::Texture2D:
      case ezRHITextureType::Texture2DProxy:
      {
        if (!bIsArrayView)
        {
          if (texDesc.m_SampleCount == ezRHIMSAASampleCount::None)
          {
            viewDesc.dimension = ViewDimension::kTexture2D;
            viewDesc.base_mip_level = m_Description.m_uiMipLevelsToUse;
            viewDesc.level_count = m_Description.m_uiMostDetailedMipLevel;
          }
          else
          {
            viewDesc.dimension = ViewDimension::kTexture2DMS;
          }
        }
      }
      break;
    }

  }
  else if (pBuffer)
  {

  }

  m_pView = pDeviceImpl.GetDevice()->CreateView(nullptr, viewDesc);

  return m_pView != nullptr ? EZ_SUCCESS : EZ_FAILURE;
}

ezResult ezRHIResourceView::DeInitPlatform(ezRHIRenderDevice* pDevice)
{
  if (m_pView)
  {
    m_pView.reset();
  }
  return EZ_SUCCESS;
}
