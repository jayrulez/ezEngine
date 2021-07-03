
#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Resources/Resource.h>

class EZ_RHI_DLL ezRHIBuffer : public ezRHIResource<ezRHIBufferCreationDescription>
{
public:
  EZ_ALWAYS_INLINE ezUInt32 GetSize() const;

protected:
  friend class ezRHIDevice;

  ezRHIBuffer(const ezRHIBufferCreationDescription& Description);

  virtual ~ezRHIBuffer();

  virtual ezResult InitPlatform(ezRHIDevice* pDevice, ezArrayPtr<const ezUInt8> pInitialData) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;
};

#include <RHI/Renderer/Resources/Implementation/Buffer_inl.h>
