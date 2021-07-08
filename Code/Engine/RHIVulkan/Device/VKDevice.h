#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/Device/Device.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPool.h>

#include <map>

class ezRHIVKDevice : public ezRHIDevice
{
public:
  ezRHIVKDevice(ezRHIVKAdapter& adapter);

  virtual ezRHIRaytracingASPrebuildInfo GetBLASPrebuildInfo(const ezDynamicArray<ezRHIRaytracingGeometryDescription>& descs,
    ezBitflags<ezRHIBuildAccelerationStructureFlags> flags) const override;
  virtual ezRHIRaytracingASPrebuildInfo GetTLASPrebuildInfo(ezUInt32 instanceCount, ezBitflags<ezRHIBuildAccelerationStructureFlags> flags) const override;


  ezRHIVKAdapter& GetAdapter();
  vk::Device GetDevice();
  ezRHICommandListType GetAvailableCommandListType(ezRHICommandListType type);
  vk::CommandPool GetCmdPool(ezRHICommandListType type);
  vk::ImageAspectFlags GetAspectFlags(vk::Format format) const;
  ezRHIVKGPUBindlessDescriptorPoolTyped& GetGPUBindlessDescriptorPool(vk::DescriptorType type);
  ezRHIVKGPUDescriptorPool& GetGPUDescriptorPool();
  ezUInt32 FindMemoryType(ezUInt32 type_filter, vk::MemoryPropertyFlags properties);
  vk::AccelerationStructureGeometryKHR FillRaytracingGeometryTriangles(const ezRHIBufferDescription& vertex, const ezRHIBufferDescription& index, ezRHIRaytracingGeometryFlags flags) const;

protected:
  virtual ezResult InitializePlatform() override;
  virtual ezResult DestroyPlatform() override;

  virtual ezRHIMemory* AllocateMemoryPlatform(ezUInt32 size, ezRHIMemoryType memoryType, ezUInt32 memoryTypeBits) override;
  virtual ezRHICommandQueue* GetCommandQueuePlatform(ezRHICommandListType type) override;
  virtual ezRHISwapChain* CreateSwapchainPlatform(const ezRHISwapChainCreationDescription& desc) override;
  virtual ezRHICommandList* CreateCommandListPlatform(ezRHICommandListType type) override;
  virtual ezRHIFence* CreateFencePlatform(ezUInt64 initialValue) override;
  virtual ezRHIResource* CreateTexturePlatform(const ezRHITextureCreationDescription& desc) override;
  virtual ezRHIResource* CreateBufferPlatform(const ezRHIBufferCreationDescription& desc) override;
  virtual ezRHIResource* CreateSamplerPlatform(const ezRHISamplerCreationDescription& desc) override;
  virtual ezRHIView* CreateViewPlatform(ezRHIResource* pResource, const ezRHIViewDesc& desc) override;
  virtual ezRHIBindingSetLayout* CreateBindingSetLayoutPlatform(const ezDynamicArray<ezRHIBindKeyDescription>& descs) override;
  virtual ezRHIBindingSet* CreateBindingSetPlatform(ezRHIBindingSetLayout* pLayout) override;
  virtual ezRHIRenderPass* CreateRenderPassPlatform(const ezRHIRenderPassCreationDescription& desc) override;
  virtual ezRHIFramebuffer* CreateFramebufferPlatform(const ezRHIFramebufferCreationDescription& desc) override;
  virtual ezRHIShader* CreateShaderPlatform(const ezRHIShaderCreationDescription& desc) override;
  virtual ezRHIProgram* CreateProgramPlatform(const ezDynamicArray<ezRHIShader*>& shaders) override;
  virtual ezRHIPipeline* CreateGraphicsPipelinePlatform(const ezRHIGraphicsPipelineCreationDescription& desc) override;
  virtual ezRHIPipeline* CreateComputePipelinePlatform(const ezRHIComputePipelineCreationDescription& desc) override;
  virtual ezRHIPipeline* CreateRayTracingPipelinePlatform(const ezRHIRayTracingPipelineCreationDescription& desc) override;
  virtual ezRHIResource* CreateAccelerationStructurePlatform(ezRHIAccelerationStructureType type, ezRHIResource* pResource, ezUInt64 offset) override;
  virtual ezRHIQueryHeap* CreateQueryHeapPlatform(ezRHIQueryHeapType type, ezUInt32 count) override;

private:
  ezRHIRaytracingASPrebuildInfo GetAccelerationStructurePrebuildInfo(const vk::AccelerationStructureBuildGeometryInfoKHR& accelerationStructureInfo, const ezDynamicArray<ezUInt32>& maxPrimitiveCounts) const;

  ezRHIVKAdapter& m_Adapter;
  const vk::PhysicalDevice& m_PhysicalDevice;
  vk::UniqueDevice m_Device;
  struct QueueInfo
  {
    ezUInt32 m_QueueFamilyIndex;
    ezUInt32 m_QueueCount;
  };
  ezMap<ezRHICommandListType, QueueInfo> m_QueuesInfo;
  ezMap<ezRHICommandListType, vk::UniqueCommandPool> m_CmdPools;
  ezMap<ezRHICommandListType, ezRHIVKCommandQueue*> m_CommandQueues;
  //ezMap<vk::DescriptorType, ezRHIVKGPUBindlessDescriptorPoolTyped> m_GpuBindlessDescriptorPool;
  std::map<vk::DescriptorType, ezRHIVKGPUBindlessDescriptorPoolTyped> m_GpuBindlessDescriptorPool;
  ezRHIVKGPUDescriptorPool m_GpuDescriptorPool;

  //bool m_is_variable_rate_shading_supported = false;
  //ezUInt32 m_shading_rate_image_tile_size = 0;
  //bool m_is_dxr_supported = false;
  //bool m_is_ray_query_supported = false;
  //bool m_is_mesh_shading_supported = false;
  //ezUInt32 m_shader_group_handle_size = 0;
  //ezUInt32 m_shader_record_alignment = 0;
  //ezUInt32 m_shader_table_alignment = 0;
};

vk::ImageLayout ConvertState(ezBitflags<ezRHIResourceState> state);
vk::BuildAccelerationStructureFlagsKHR Convert(ezBitflags<ezRHIBuildAccelerationStructureFlags> flags);
