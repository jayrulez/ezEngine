
#pragma once

#include <RHI/Renderer/Descriptors/Descriptors.h>
#include <RHI/Renderer/Basics.h>

class EZ_RHI_DLL ezRHIVertexDeclaration : public ezRHIObject<ezRHIVertexDeclarationCreationDescription>
{
public:
protected:
  friend class ezRHIDevice;

  virtual ezResult InitPlatform(ezRHIDevice* pDevice) = 0;

  virtual ezResult DeInitPlatform(ezRHIDevice* pDevice) = 0;

  ezRHIVertexDeclaration(const ezRHIVertexDeclarationCreationDescription& Description);

  virtual ~ezRHIVertexDeclaration();
};
