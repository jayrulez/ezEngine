#include <RHIPCH.h>

#include <RHI/Renderer/Resources/Buffer.h>

ezRHIBuffer::ezRHIBuffer(const ezRHIBufferCreationDescription& Description)
  : ezRHIResource(Description)
{
}

ezRHIBuffer::~ezRHIBuffer() {}



EZ_STATICLINK_FILE(RendererFoundation, RendererFoundation_Resources_Implementation_Buffer);
