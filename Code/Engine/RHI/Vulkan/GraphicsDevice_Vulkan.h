#pragma once

#ifdef EZ_RHI_VULKAN_SUPPORTED
#  include <RHI/GraphicsDevice.h>
#  include <RHI/RHIDLL.h>
#  include <RHI/RHIPCH.h>
#include <Foundation/Types/SharedPtr.h>

#include <RHI/Vulkan/Vulkan_Internal.h>

#  include <algorithm>
#  include <atomic>
#  include <deque>
#  include <mutex>
#  include <unordered_map>
#  include <vector>

#  include <RHI/RHIInternal.h>

using namespace Vulkan_Internal;

struct FrameResources;
struct DescriptorTableFrameAllocator;

struct QueueFamilyIndices
{
  int graphicsFamily = -1;
  int presentFamily = -1;
  int copyFamily = -1;

  bool isComplete()
  {
    return graphicsFamily >= 0 && presentFamily >= 0 && copyFamily >= 0;
  }
};

class EZ_RHI_DLL GraphicsDevice_Vulkan : public GraphicsDevice
{
  friend struct DescriptorTableFrameAllocator;

private:
  VkInstance instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT debugUtilsMessenger{VK_NULL_HANDLE};
  /// Deprecated
  VkDebugReportCallbackEXT debugReportCallback = VK_NULL_HANDLE;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;
  QueueFamilyIndices queueIndices;
  VkQueue graphicsQueue = VK_NULL_HANDLE;
  VkQueue presentQueue = VK_NULL_HANDLE;

  VkPhysicalDeviceProperties2 device_properties = {};
  VkPhysicalDeviceVulkan11Properties device_properties_1_1 = {};
  VkPhysicalDeviceVulkan12Properties device_properties_1_2 = {};
  VkPhysicalDeviceRayTracingPropertiesKHR raytracing_properties = {};
  VkPhysicalDeviceMeshShaderPropertiesNV mesh_shader_properties = {};

  VkPhysicalDeviceFeatures2 device_features2 = {};
  VkPhysicalDeviceVulkan11Features features_1_1 = {};
  VkPhysicalDeviceVulkan12Features features_1_2 = {};
  VkPhysicalDeviceRayTracingFeaturesKHR raytracing_features = {};
  VkPhysicalDeviceMeshShaderFeaturesNV mesh_shader_features = {};

  VkSwapchainKHR swapChain = VK_NULL_HANDLE;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  ezUInt32 swapChainImageIndex = 0;
  ezDynamicArray<VkImage> swapChainImages;
  ezDynamicArray<VkImageView> swapChainImageViews;
  ezDynamicArray<VkFramebuffer> swapChainFramebuffers;

  VkRenderPass defaultRenderPass = VK_NULL_HANDLE;

  VkBuffer nullBuffer = VK_NULL_HANDLE;
  VmaAllocation nullBufferAllocation = VK_NULL_HANDLE;
  VkBufferView nullBufferView = VK_NULL_HANDLE;
  VkSampler nullSampler = VK_NULL_HANDLE;
  VmaAllocation nullImageAllocation1D = VK_NULL_HANDLE;
  VmaAllocation nullImageAllocation2D = VK_NULL_HANDLE;
  VmaAllocation nullImageAllocation3D = VK_NULL_HANDLE;
  VkImage nullImage1D = VK_NULL_HANDLE;
  VkImage nullImage2D = VK_NULL_HANDLE;
  VkImage nullImage3D = VK_NULL_HANDLE;
  VkImageView nullImageView1D = VK_NULL_HANDLE;
  VkImageView nullImageView1DArray = VK_NULL_HANDLE;
  VkImageView nullImageView2D = VK_NULL_HANDLE;
  VkImageView nullImageView2DArray = VK_NULL_HANDLE;
  VkImageView nullImageViewCube = VK_NULL_HANDLE;
  VkImageView nullImageViewCubeArray = VK_NULL_HANDLE;
  VkImageView nullImageView3D = VK_NULL_HANDLE;

  ezUInt64 timestamp_frequency = 0;
  VkQueryPool querypool_timestamp = VK_NULL_HANDLE;
  VkQueryPool querypool_occlusion = VK_NULL_HANDLE;
  bool initial_querypool_reset = false;
  ezDynamicArray<ezUInt32> timestamps_to_reset;
  ezDynamicArray<ezUInt32> occlusions_to_reset;

  void CreateBackBufferResources();

  std::mutex copyQueueMutex;
  bool copyQueueUse = false;
  VkSemaphore copySemaphore = VK_NULL_HANDLE;

  struct FrameResources
  {
    VkFence frameFence = VK_NULL_HANDLE;
    VkCommandPool commandPools[COMMANDLIST_COUNT] = {};
    VkCommandBuffer commandBuffers[COMMANDLIST_COUNT] = {};

    VkQueue copyQueue = VK_NULL_HANDLE;
    VkCommandPool copyCommandPool = VK_NULL_HANDLE;
    VkCommandBuffer copyCommandBuffer = VK_NULL_HANDLE;

    VkCommandPool transitionCommandPool = VK_NULL_HANDLE;
    VkCommandBuffer transitionCommandBuffer = VK_NULL_HANDLE;
    ezDynamicArray<VkImageMemoryBarrier> loadedimagetransitions;

    VkSemaphore swapchainAcquireSemaphore = VK_NULL_HANDLE;
    VkSemaphore swapchainReleaseSemaphore = VK_NULL_HANDLE;

    struct DescriptorTableFrameAllocator
    {
      GraphicsDevice_Vulkan* device;
      VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
      ezUInt32 poolSize = 256;

      ezDynamicArray<VkWriteDescriptorSet> descriptorWrites;
      ezDynamicArray<VkDescriptorBufferInfo> bufferInfos;
      ezDynamicArray<VkDescriptorImageInfo> imageInfos;
      ezDynamicArray<VkBufferView> texelBufferViews;
      ezDynamicArray<VkWriteDescriptorSetAccelerationStructureNV> accelerationStructureViews;
      bool dirty = false;

      const GPUBuffer* CBV[GPU_RESOURCE_HEAP_CBV_COUNT];
      const GPUResource* SRV[GPU_RESOURCE_HEAP_SRV_COUNT];
      int SRV_index[GPU_RESOURCE_HEAP_SRV_COUNT];
      const GPUResource* UAV[GPU_RESOURCE_HEAP_UAV_COUNT];
      int UAV_index[GPU_RESOURCE_HEAP_UAV_COUNT];
      const Sampler* SAM[GPU_SAMPLER_HEAP_COUNT];

      void init(GraphicsDevice_Vulkan* device);
      void destroy();

      void reset();
      void validate(bool graphics, CommandList cmd, bool raytracing = false);
      VkDescriptorSet commit(const DescriptorTable* table);
    };
    DescriptorTableFrameAllocator descriptors[COMMANDLIST_COUNT];


    struct ResourceFrameAllocator
    {
      GraphicsDevice_Vulkan* device = nullptr;
      GPUBuffer buffer;
      ezUInt8* dataBegin = nullptr;
      ezUInt8* dataCur = nullptr;
      ezUInt8* dataEnd = nullptr;

      void init(GraphicsDevice_Vulkan* device, size_t size);

      ezUInt8* allocate(size_t dataSize, size_t alignment);
      void clear();
      ezUInt64 calculateOffset(ezUInt8* address);
    };
    ResourceFrameAllocator resourceBuffer[COMMANDLIST_COUNT];
  };
  FrameResources frames[BACKBUFFER_COUNT];
  FrameResources& GetFrameResources() { return frames[GetFrameCount() % BACKBUFFER_COUNT]; }
  inline VkCommandBuffer GetDirectCommandList(CommandList cmd) { return GetFrameResources().commandBuffers[cmd]; }

  std::unordered_map<size_t, VkPipeline> pipelines_global;
  ezDynamicArray<std::pair<size_t, VkPipeline>> pipelines_worker[COMMANDLIST_COUNT];
  size_t prev_pipeline_hash[COMMANDLIST_COUNT] = {};
  const PipelineState* active_pso[COMMANDLIST_COUNT] = {};
  const Shader* active_cs[COMMANDLIST_COUNT] = {};
  const RaytracingPipelineState* active_rt[COMMANDLIST_COUNT] = {};
  const RenderPass* active_renderpass[COMMANDLIST_COUNT] = {};

  bool dirty_pso[COMMANDLIST_COUNT] = {};
  void pso_validate(CommandList cmd);

  void predraw(CommandList cmd);
  void predispatch(CommandList cmd);
  void preraytrace(CommandList cmd);

  std::atomic<CommandList> cmd_count{0};

public:
  GraphicsDevice_Vulkan(RHIWindowType window, bool fullscreen = false, bool debuglayer = false);
  virtual ~GraphicsDevice_Vulkan();

  bool CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer) override;
  bool CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, Texture* pTexture) override;
  bool CreateInputLayout(const InputLayoutDesc* pInputElementDescs, ezUInt32 NumElements, const Shader* shader, InputLayout* pInputLayout) override;
  bool CreateShader(ezEnum<ezRHIShaderStage> stafe, const void* pShaderBytecode, size_t BytecodeLength, Shader* pShader) override;
  bool CreateBlendState(const BlendStateDesc* pBlendStateDesc, BlendState* pBlendState) override;
  bool CreateDepthStencilState(const DepthStencilStateDesc* pDepthStencilStateDesc, DepthStencilState* pDepthStencilState) override;
  bool CreateRasterizerState(const RasterizerStateDesc* pRasterizerStateDesc, RasterizerState* pRasterizerState) override;
  bool CreateSampler(const SamplerDesc* pSamplerDesc, Sampler* pSamplerState) override;
  bool CreateQuery(const GPUQueryDesc* pDesc, GPUQuery* pQuery) override;
  bool CreatePipelineState(const PipelineStateDesc* pDesc, PipelineState* pso) override;
  bool CreateRenderPass(const RenderPassDesc* pDesc, RenderPass* renderpass) override;
  bool CreateRaytracingAccelerationStructure(const RaytracingAccelerationStructureDesc* pDesc, RaytracingAccelerationStructure* bvh) override;
  bool CreateRaytracingPipelineState(const RaytracingPipelineStateDesc* pDesc, RaytracingPipelineState* rtpso) override;
  bool CreateDescriptorTable(DescriptorTable* table) override;
  bool CreateRootSignature(RootSignature* rootsig) override;

  int CreateSubresource(Texture* texture, SUBRESOURCE_TYPE type, ezUInt32 firstSlice, ezUInt32 sliceCount, ezUInt32 firstMip, ezUInt32 mipCount) override;
  int CreateSubresource(GPUBuffer* buffer, SUBRESOURCE_TYPE type, ezUInt64 offset, ezUInt64 size = ~0) override;

  void WriteTopLevelAccelerationStructureInstance(const RaytracingAccelerationStructureDesc::TopLevel::Instance* instance, void* dest) override;
  void WriteShaderIdentifier(const RaytracingPipelineState* rtpso, ezUInt32 group_index, void* dest) override;
  void WriteDescriptor(const DescriptorTable* table, ezUInt32 rangeIndex, ezUInt32 arrayIndex, const GPUResource* resource, int subresource = -1, ezUInt64 offset = 0) override;
  void WriteDescriptor(const DescriptorTable* table, ezUInt32 rangeIndex, ezUInt32 arrayIndex, const Sampler* sampler) override;

  void Map(const GPUResource* resource, Mapping* mapping) override;
  void Unmap(const GPUResource* resource) override;
  bool QueryRead(const GPUQuery* query, GPUQueryResult* result) override;

  void SetName(GPUResource* pResource, const char* name) override;

  void PresentBegin(CommandList cmd) override;
  void PresentEnd(CommandList cmd) override;

  CommandList BeginCommandList() override;
  void SubmitCommandLists() override;

  void WaitForGPU() override;
  void ClearPipelineStateCache() override;

  void SetResolution(int width, int height) override;

  Texture GetBackBuffer() override;

  void SetVSyncEnabled(bool value) override
  {
    VSYNC = value;
    CreateBackBufferResources();
  };

  ///////////////Thread-sensitive////////////////////////

  void RenderPassBegin(const RenderPass* renderpass, CommandList cmd) override;
  void RenderPassEnd(CommandList cmd) override;
  void BindScissorRects(ezUInt32 numRects, const Rect* rects, CommandList cmd) override;
  void BindViewports(ezUInt32 NumViewports, const Viewport* pViewports, CommandList cmd) override;
  void BindResource(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource = -1) override;
  void BindResources(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd) override;
  void BindUAV(ezEnum<ezRHIShaderStage> stage, const GPUResource* resource, ezUInt32 slot, CommandList cmd, int subresource = -1) override;
  void BindUAVs(ezEnum<ezRHIShaderStage> stage, const GPUResource* const* resources, ezUInt32 slot, ezUInt32 count, CommandList cmd) override;
  void UnbindResources(ezUInt32 slot, ezUInt32 num, CommandList cmd) override;
  void UnbindUAVs(ezUInt32 slot, ezUInt32 num, CommandList cmd) override;
  void BindSampler(ezEnum<ezRHIShaderStage> stage, const Sampler* sampler, ezUInt32 slot, CommandList cmd) override;
  void BindConstantBuffer(ezEnum<ezRHIShaderStage> stage, const GPUBuffer* buffer, ezUInt32 slot, CommandList cmd) override;
  void BindVertexBuffers(const GPUBuffer* const* vertexBuffers, ezUInt32 slot, ezUInt32 count, const ezUInt32* strides, const ezUInt32* offsets, CommandList cmd) override;
  void BindIndexBuffer(const GPUBuffer* indexBuffer, const ezEnum<ezRHIIndexBufferFormat> format, ezUInt32 offset, CommandList cmd) override;
  void BindStencilRef(ezUInt32 value, CommandList cmd) override;
  void BindBlendFactor(float r, float g, float b, float a, CommandList cmd) override;
  void BindPipelineState(const PipelineState* pso, CommandList cmd) override;
  void BindComputeShader(const Shader* cs, CommandList cmd) override;
  void Draw(ezUInt32 vertexCount, ezUInt32 startVertexLocation, CommandList cmd) override;
  void DrawIndexed(ezUInt32 indexCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, CommandList cmd) override;
  void DrawInstanced(ezUInt32 vertexCount, ezUInt32 instanceCount, ezUInt32 startVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd) override;
  void DrawIndexedInstanced(ezUInt32 indexCount, ezUInt32 instanceCount, ezUInt32 startIndexLocation, ezUInt32 baseVertexLocation, ezUInt32 startInstanceLocation, CommandList cmd) override;
  void DrawInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) override;
  void DrawIndexedInstancedIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) override;
  void Dispatch(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ, CommandList cmd) override;
  void DispatchIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) override;
  void DispatchMesh(ezUInt32 threadGroupCountX, ezUInt32 threadGroupCountY, ezUInt32 threadGroupCountZ, CommandList cmd) override;
  void DispatchMeshIndirect(const GPUBuffer* args, ezUInt32 args_offset, CommandList cmd) override;
  void CopyResource(const GPUResource* pDst, const GPUResource* pSrc, CommandList cmd) override;
  void UpdateBuffer(const GPUBuffer* buffer, const void* data, CommandList cmd, int dataSize = -1) override;
  void QueryBegin(const GPUQuery* query, CommandList cmd) override;
  void QueryEnd(const GPUQuery* query, CommandList cmd) override;
  void Barrier(const GPUBarrier* barriers, ezUInt32 numBarriers, CommandList cmd) override;
  void BuildRaytracingAccelerationStructure(const RaytracingAccelerationStructure* dst, CommandList cmd, const RaytracingAccelerationStructure* src = nullptr) override;
  void BindRaytracingPipelineState(const RaytracingPipelineState* rtpso, CommandList cmd) override;
  void DispatchRays(const DispatchRaysDesc* desc, CommandList cmd) override;

  void BindDescriptorTable(BINDPOINT bindpoint, ezUInt32 space, const DescriptorTable* table, CommandList cmd) override;
  void BindRootDescriptor(BINDPOINT bindpoint, ezUInt32 index, const GPUBuffer* buffer, ezUInt32 offset, CommandList cmd) override;
  void BindRootConstants(BINDPOINT bindpoint, ezUInt32 index, const void* srcdata, CommandList cmd) override;

  GPUAllocation AllocateGPU(size_t dataSize, CommandList cmd) override;

  void EventBegin(const char* name, CommandList cmd) override;
  void EventEnd(CommandList cmd) override;
  void SetMarker(const char* name, CommandList cmd) override;

  ezSharedPtr<VulkanAllocationHandler> allocationhandler;
};

#endif // WICKEDENGINE_BUILD_VULKAN
