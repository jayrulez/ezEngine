#include <RHI/Resources/Buffer.h>

ezRHIBuffer::ezRHIBuffer(const ezRHIGPUBufferDesc& desc)
  : ezRHIResource(desc, ezRHIGPUResourceType::Buffer)
{
}

ezRHIBuffer::~ezRHIBuffer()
{
}
