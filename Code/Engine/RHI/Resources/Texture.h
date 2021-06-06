#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Resources/Resource.h>

class EZ_RHI_DLL ezRHITexture : public ezRHIResource<ezRHITextureDesc>
{
public:

protected:
  ezRHITexture(const ezRHITextureDesc& desc);

  virtual ~ezRHITexture();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;
  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};
