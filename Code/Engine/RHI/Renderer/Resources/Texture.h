
#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Resources/Resource.h>

class EZ_RHI_DLL ezRHITexture : public ezRHIResource<ezRHITextureCreationDescription>
{
public:
protected:
  friend class ezRHIDevice;

  ezRHITexture(const ezRHITextureCreationDescription& Description);

  virtual ~ezRHITexture();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice, ezArrayPtr<ezRHISystemMemoryDescription> pInitialData) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;

  virtual ezResult ReplaceExisitingNativeObject(void* pExisitingNativeObject) = 0;
};
