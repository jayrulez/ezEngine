#include <RHIPCH.h>

#include <RHI/Device/Device.h>

ezResult ezRHIDevice::Initialize()
{
  return InitializePlatform();
}

ezResult ezRHIDevice::Destroy()
{
  return DestroyPlatform();
}

ezRHIMemory* ezRHIDevice::AllocateMemory(ezUInt32 size, ezRHIMemoryType memoryType, ezUInt32 memoryTypeBits)
{
  return AllocateMemoryPlatform(size, memoryType, memoryTypeBits);
}

ezRHICommandQueue* ezRHIDevice::GetCommandQueue(ezRHICommandListType type)
{
  return GetCommandQueuePlatform(type);
}

ezRHISwapChain* ezRHIDevice::CreateSwapchain(const ezRHISwapChainCreationDescription& desc)
{
  return CreateSwapchainPlatform(desc);
}

ezRHICommandList* ezRHIDevice::CreateCommandList(ezRHICommandListType type)
{
  return CreateCommandListPlatform(type);
}

ezRHIFence* ezRHIDevice::CreateFence(ezUInt64 initialValue)
{
  return CreateFencePlatform(initialValue);
}

ezRHIResource* ezRHIDevice::CreateTexture(const ezRHITextureCreationDescription& desc)
{
  return CreateTexturePlatform(desc);
}

ezRHIResource* ezRHIDevice::CreateBuffer(const ezRHIBufferCreationDescription& desc)
{
  return CreateBufferPlatform(desc);
}

ezRHIResource* ezRHIDevice::CreateSampler(const ezRHISamplerCreationDescription& desc)
{
  return CreateSamplerPlatform(desc);
}

ezRHIView* ezRHIDevice::CreateView(ezRHIResource* pResource, const ezRHIViewDesc& desc)
{
  return CreateViewPlatform(pResource, desc);
}

ezRHIBindingSetLayout* ezRHIDevice::CreateBindingSetLayout(const ezDynamicArray<ezRHIBindKeyDescription>& descs)
{
  return CreateBindingSetLayoutPlatform(descs);
}

ezRHIBindingSet* ezRHIDevice::CreateBindingSet(const ezRHIBindingSetLayout* pLayout)
{
  return CreateBindingSetPlatform(pLayout);
}

ezRHIRenderPass* ezRHIDevice::CreateRenderPass(const ezRHIRenderPassCreationDescription& desc)
{
  return CreateRenderPassPlatform(desc);
}

ezRHIFramebuffer* ezRHIDevice::CreateFramebuffer(const ezRHIFramebufferCreationDescription& desc)
{
  return CreateFramebufferPlatform(desc);
}

ezRHIShader* ezRHIDevice::CreateShader(const ezRHIShaderCreationDescription& desc)
{
  return CreateShaderPlatform(desc);
}

ezRHIProgram* ezRHIDevice::CreateProgram(const ezDynamicArray<ezRHIShader*>& shaders)
{
  return CreateProgramPlatform(shaders);
}

ezRHIPipeline* ezRHIDevice::CreateGraphicsPipeline(const ezRHIGraphicsPipelineCreationDescription& desc)
{
  return CreateGraphicsPipelinePlatform(desc);
}

ezRHIPipeline* ezRHIDevice::CreateComputePipeline(const ezRHIComputePipelineCreationDescription& desc)
{
  return CreateComputePipelinePlatform(desc);
}

ezRHIPipeline* ezRHIDevice::CreateRayTracingPipeline(const ezRHIRayTracingPipelineCreationDescription& desc)
{
  return CreateRayTracingPipelinePlatform(desc);
}

ezRHIResource* ezRHIDevice::CreateAccelerationStructure(ezRHIAccelerationStructureType type, const ezRHIResource* pResource, ezUInt64 offset)
{
  return CreateAccelerationStructurePlatform(type, pResource, offset);
}

ezRHIQueryHeap* ezRHIDevice::CreateQueryHeap(ezRHIQueryHeapType type, ezUInt32 count)
{
  return CreateQueryHeapPlatform(type, count);
}

const ezRHIDeviceCapabilities& ezRHIDevice::GetCapabilities() const
{
  return m_Capabilities;
}
