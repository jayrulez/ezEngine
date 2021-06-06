#pragma once

#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Resources/Resource.h>

class EZ_RHI_DLL ezRHIUnorderedAccessView : public ezRHIResource<ezRHIUnorderedAccessViewDesc>
{
public:

protected:
  ezRHIUnorderedAccessView(const ezRHIUnorderedAccessViewDesc& desc);

  virtual ~ezRHIUnorderedAccessView();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;
  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};
