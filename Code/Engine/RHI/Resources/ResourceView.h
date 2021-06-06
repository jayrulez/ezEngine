#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Resources/Resource.h>

class EZ_RHI_DLL ezRHIResourceView : public ezRHIResource<ezRHIResourceViewDesc>
{
public:

protected:
  ezRHIResourceView(const ezRHIResourceViewDesc& desc);

  virtual ~ezRHIResourceView();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;
  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};
