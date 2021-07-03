#include <RHIPCH.h>

#include <RHI/Renderer/Shader/VertexDeclaration.h>

ezRHIVertexDeclaration::ezRHIVertexDeclaration(const ezRHIVertexDeclarationCreationDescription& Description)
  : ezRHIObject(Description)
{
}

ezRHIVertexDeclaration::~ezRHIVertexDeclaration() {}



EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Shader_Implementation_VertexDeclaration);
