#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>
#include <RHI/Device/DeviceCapabilities.h>

class EZ_RHI_DLL ezRHIDevice : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIDevice() = default;

  ezResult Initialize();
  ezResult Destroy();

  ezRHIMemory* AllocateMemory(ezUInt32 size, ezRHIMemoryType memoryType, ezUInt32 memoryTypeBits);
  ezRHICommandQueue* GetCommandQueue(ezRHICommandListType type);
  ezRHISwapChain* CreateSwapchain(const ezRHISwapChainCreationDescription& desc);
  ezRHICommandList* CreateCommandList(ezRHICommandListType type);
  ezRHIFence* CreateFence(ezUInt64 initialValue);
  ezRHIResource* CreateTexture(const ezRHITextureCreationDescription& desc);
  ezRHIResource* CreateBuffer(const ezRHIBufferCreationDescription& desc);
  ezRHIResource* CreateSampler(const ezRHISamplerCreationDescription& desc);
  ezRHIView* CreateView(ezRHIResource* pResource, const ezRHIViewDesc& desc);
  ezRHIBindingSetLayout* CreateBindingSetLayout(const ezDynamicArray<ezRHIBindKeyDescription>& descs);
  ezRHIBindingSet* CreateBindingSet(ezRHIBindingSetLayout* pLayout);
  ezRHIRenderPass* CreateRenderPass(const ezRHIRenderPassCreationDescription& desc);
  ezRHIFramebuffer* CreateFramebuffer(const ezRHIFramebufferCreationDescription& desc);
  ezRHIShader* CreateShader(const ezRHIShaderCreationDescription& desc);
  ezRHIProgram* CreateProgram(const ezDynamicArray<ezRHIShader*>& shaders);
  ezRHIPipeline* CreateGraphicsPipeline(const ezRHIGraphicsPipelineCreationDescription& desc);
  ezRHIPipeline* CreateComputePipeline(const ezRHIComputePipelineCreationDescription& desc);
  ezRHIPipeline* CreateRayTracingPipeline(const ezRHIRayTracingPipelineCreationDescription& desc);
  ezRHIResource* CreateAccelerationStructure(ezRHIAccelerationStructureType type, ezRHIResource* pResource, ezUInt64 offset);
  ezRHIQueryHeap* CreateQueryHeap(ezRHIQueryHeapType type, ezUInt32 count);

  virtual ezRHIRaytracingASPrebuildInfo GetBLASPrebuildInfo(const ezDynamicArray<ezRHIRaytracingGeometryDescription>& descs,
    ezBitflags<ezRHIBuildAccelerationStructureFlags> flags) const = 0;
  virtual ezRHIRaytracingASPrebuildInfo GetTLASPrebuildInfo(ezUInt32 instanceCount, ezBitflags<ezRHIBuildAccelerationStructureFlags> flags) const = 0;

  const ezRHIDeviceCapabilities& GetCapabilities() const;

protected:
  ezRHIDevice() = default;

  virtual ezResult InitializePlatform() = 0;
  virtual ezResult DestroyPlatform() = 0;

  virtual ezRHIMemory* AllocateMemoryPlatform(ezUInt32 size, ezRHIMemoryType memoryType, ezUInt32 memoryTypeBits) = 0;
  virtual ezRHICommandQueue* GetCommandQueuePlatform(ezRHICommandListType type) = 0;
  virtual ezRHISwapChain* CreateSwapchainPlatform(const ezRHISwapChainCreationDescription& desc) = 0;
  virtual ezRHICommandList* CreateCommandListPlatform(ezRHICommandListType type) = 0;
  virtual ezRHIFence* CreateFencePlatform(ezUInt64 initialValue) = 0;
  virtual ezRHIResource* CreateTexturePlatform(const ezRHITextureCreationDescription& desc) = 0;
  virtual ezRHIResource* CreateBufferPlatform(const ezRHIBufferCreationDescription& desc) = 0;
  virtual ezRHIResource* CreateSamplerPlatform(const ezRHISamplerCreationDescription& desc) = 0;
  virtual ezRHIView* CreateViewPlatform(ezRHIResource* pResource, const ezRHIViewDesc& desc) = 0;
  virtual ezRHIBindingSetLayout* CreateBindingSetLayoutPlatform(const ezDynamicArray<ezRHIBindKeyDescription>& descs) = 0;
  virtual ezRHIBindingSet* CreateBindingSetPlatform(ezRHIBindingSetLayout* pLayout) = 0;
  virtual ezRHIRenderPass* CreateRenderPassPlatform(const ezRHIRenderPassCreationDescription& desc) = 0;
  virtual ezRHIFramebuffer* CreateFramebufferPlatform(const ezRHIFramebufferCreationDescription& desc) = 0;
  virtual ezRHIShader* CreateShaderPlatform(const ezRHIShaderCreationDescription& desc) = 0;
  virtual ezRHIProgram* CreateProgramPlatform(const ezDynamicArray<ezRHIShader*>& shaders) = 0;
  virtual ezRHIPipeline* CreateGraphicsPipelinePlatform(const ezRHIGraphicsPipelineCreationDescription& desc) = 0;
  virtual ezRHIPipeline* CreateComputePipelinePlatform(const ezRHIComputePipelineCreationDescription& desc) = 0;
  virtual ezRHIPipeline* CreateRayTracingPipelinePlatform(const ezRHIRayTracingPipelineCreationDescription& desc) = 0;
  virtual ezRHIResource* CreateAccelerationStructurePlatform(ezRHIAccelerationStructureType type, ezRHIResource* pResource, ezUInt64 offset) = 0;
  virtual ezRHIQueryHeap* CreateQueryHeapPlatform(ezRHIQueryHeapType type, ezUInt32 count) = 0;



protected:
  ezRHIDeviceCapabilities m_Capabilities;
};
