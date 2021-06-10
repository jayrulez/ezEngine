
#pragma once

#include <RendererVk/RendererVkDLL.h>
#include <RendererFoundation/RendererFoundationDLL.h>
#include <RendererFoundation/Shader/VertexDeclaration.h>

class ezGALVertexDeclarationVk : public ezGALVertexDeclaration
{
public:

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;

  ezGALVertexDeclarationVk(const ezGALVertexDeclarationCreationDescription& Description);

  virtual ~ezGALVertexDeclarationVk();

};

#include <RendererVk/Shader/Implementation/VertexDeclarationVk_inl.h>
