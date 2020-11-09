#pragma once

#if __has_include("vulkan/vulkan.h")
#  define WICKEDENGINE_BUILD_VULKAN
#endif // HAS VULKAN

#ifdef WICKEDENGINE_BUILD_VULKAN
#  include <RHI/GraphicsDevice.h>
#  include <RHI/RHIDLL.h>
#  include <RHI/RHIPCH.h>

#  ifdef _WIN32
#    define VK_USE_PLATFORM_WIN32_KHR
#  endif // _WIN32

#  define VK_ENABLE_BETA_EXTENSIONS
#  include <vulkan/vulkan.h>

#  include <RHI/Vulkan/ThirdParty/vk_mem_alloc.h>

#  include <algorithm>
#  include <atomic>
#  include <deque>
#  include <mutex>
#  include <unordered_map>
#  include <vector>

#  include <RHI/RHIInternal.h>


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
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

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
  static const size_t timestamp_query_count = 1024;
  static const size_t occlusion_query_count = 1024;
  bool initial_querypool_reset = false;
  std::vector<ezUInt32> timestamps_to_reset;
  std::vector<ezUInt32> occlusions_to_reset;

  void CreateBackBufferResources();

  std::mutex copyQueueLock;
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
    std::vector<VkImageMemoryBarrier> loadedimagetransitions;

    VkSemaphore swapchainAcquireSemaphore = VK_NULL_HANDLE;
    VkSemaphore swapchainReleaseSemaphore = VK_NULL_HANDLE;

    struct DescriptorTableFrameAllocator
    {
      GraphicsDevice_Vulkan* device;
      VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
      ezUInt32 poolSize = 256;

      std::vector<VkWriteDescriptorSet> descriptorWrites;
      std::vector<VkDescriptorBufferInfo> bufferInfos;
      std::vector<VkDescriptorImageInfo> imageInfos;
      std::vector<VkBufferView> texelBufferViews;
      std::vector<VkWriteDescriptorSetAccelerationStructureNV> accelerationStructureViews;
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
  std::vector<std::pair<size_t, VkPipeline>> pipelines_worker[COMMANDLIST_COUNT];
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

  static PFN_vkCreateRayTracingPipelinesKHR createRayTracingPipelinesKHR;
  static PFN_vkCreateAccelerationStructureKHR createAccelerationStructureKHR;
  static PFN_vkBindAccelerationStructureMemoryKHR bindAccelerationStructureMemoryKHR;
  static PFN_vkDestroyAccelerationStructureKHR destroyAccelerationStructureKHR;
  static PFN_vkGetAccelerationStructureMemoryRequirementsKHR getAccelerationStructureMemoryRequirementsKHR;
  static PFN_vkGetAccelerationStructureDeviceAddressKHR getAccelerationStructureDeviceAddressKHR;
  static PFN_vkGetRayTracingShaderGroupHandlesKHR getRayTracingShaderGroupHandlesKHR;
  static PFN_vkCmdBuildAccelerationStructureKHR cmdBuildAccelerationStructureKHR;
  static PFN_vkCmdTraceRaysKHR cmdTraceRaysKHR;

  static PFN_vkCmdDrawMeshTasksNV cmdDrawMeshTasksNV;
  static PFN_vkCmdDrawMeshTasksIndirectNV cmdDrawMeshTasksIndirectNV;

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


  struct AllocationHandler
  {
    VmaAllocator allocator = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkInstance instance;
    ezUInt64 framecount = 0;
    std::mutex destroylocker;
    std::deque<std::pair<std::pair<VkImage, VmaAllocation>, ezUInt64>> destroyer_images;
    std::deque<std::pair<VkImageView, ezUInt64>> destroyer_imageviews;
    std::deque<std::pair<std::pair<VkBuffer, VmaAllocation>, ezUInt64>> destroyer_buffers;
    std::deque<std::pair<VkBufferView, ezUInt64>> destroyer_bufferviews;
    std::deque<std::pair<VkAccelerationStructureKHR, ezUInt64>> destroyer_bvhs;
    std::deque<std::pair<VkSampler, ezUInt64>> destroyer_samplers;
    std::deque<std::pair<VkDescriptorPool, ezUInt64>> destroyer_descriptorPools;
    std::deque<std::pair<VkDescriptorSetLayout, ezUInt64>> destroyer_descriptorSetLayouts;
    std::deque<std::pair<VkDescriptorUpdateTemplate, ezUInt64>> destroyer_descriptorUpdateTemplates;
    std::deque<std::pair<VkShaderModule, ezUInt64>> destroyer_shadermodules;
    std::deque<std::pair<VkPipelineLayout, ezUInt64>> destroyer_pipelineLayouts;
    std::deque<std::pair<VkPipeline, ezUInt64>> destroyer_pipelines;
    std::deque<std::pair<VkRenderPass, ezUInt64>> destroyer_renderpasses;
    std::deque<std::pair<VkFramebuffer, ezUInt64>> destroyer_framebuffers;
    std::deque<std::pair<ezUInt32, ezUInt64>> destroyer_queries_occlusion;
    std::deque<std::pair<ezUInt32, ezUInt64>> destroyer_queries_timestamp;

    ThreadSafeRingBuffer<ezUInt32, timestamp_query_count> free_timestampqueries;
    ThreadSafeRingBuffer<ezUInt32, occlusion_query_count> free_occlusionqueries;

    ~AllocationHandler()
    {
      Update(~0, 0); // destroy all remaining
      vmaDestroyAllocator(allocator);
      vkDestroyDevice(device, nullptr);
      vkDestroyInstance(instance, nullptr);
    }

    // Deferred destroy of resources that the GPU is already finished with:
    void Update(ezUInt64 FRAMECOUNT, ezUInt32 BACKBUFFER_COUNT)
    {
      destroylocker.lock();
      framecount = FRAMECOUNT;
      while (!destroyer_images.empty())
      {
        if (destroyer_images.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_images.front();
          destroyer_images.pop_front();
          vmaDestroyImage(allocator, item.first.first, item.first.second);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_imageviews.empty())
      {
        if (destroyer_imageviews.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_imageviews.front();
          destroyer_imageviews.pop_front();
          vkDestroyImageView(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_buffers.empty())
      {
        if (destroyer_buffers.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_buffers.front();
          destroyer_buffers.pop_front();
          vmaDestroyBuffer(allocator, item.first.first, item.first.second);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_bufferviews.empty())
      {
        if (destroyer_bufferviews.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_bufferviews.front();
          destroyer_bufferviews.pop_front();
          vkDestroyBufferView(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_bvhs.empty())
      {
        if (destroyer_bvhs.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_bvhs.front();
          destroyer_bvhs.pop_front();
          destroyAccelerationStructureKHR(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_samplers.empty())
      {
        if (destroyer_samplers.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_samplers.front();
          destroyer_samplers.pop_front();
          vkDestroySampler(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_descriptorPools.empty())
      {
        if (destroyer_descriptorPools.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_descriptorPools.front();
          destroyer_descriptorPools.pop_front();
          vkDestroyDescriptorPool(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_descriptorSetLayouts.empty())
      {
        if (destroyer_descriptorSetLayouts.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_descriptorSetLayouts.front();
          destroyer_descriptorSetLayouts.pop_front();
          vkDestroyDescriptorSetLayout(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_descriptorUpdateTemplates.empty())
      {
        if (destroyer_descriptorUpdateTemplates.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_descriptorUpdateTemplates.front();
          destroyer_descriptorUpdateTemplates.pop_front();
          vkDestroyDescriptorUpdateTemplate(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_shadermodules.empty())
      {
        if (destroyer_shadermodules.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_shadermodules.front();
          destroyer_shadermodules.pop_front();
          vkDestroyShaderModule(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_pipelineLayouts.empty())
      {
        if (destroyer_pipelineLayouts.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_pipelineLayouts.front();
          destroyer_pipelineLayouts.pop_front();
          vkDestroyPipelineLayout(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_pipelines.empty())
      {
        if (destroyer_pipelines.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_pipelines.front();
          destroyer_pipelines.pop_front();
          vkDestroyPipeline(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_renderpasses.empty())
      {
        if (destroyer_renderpasses.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_renderpasses.front();
          destroyer_renderpasses.pop_front();
          vkDestroyRenderPass(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_framebuffers.empty())
      {
        if (destroyer_framebuffers.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_framebuffers.front();
          destroyer_framebuffers.pop_front();
          vkDestroyFramebuffer(device, item.first, nullptr);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_queries_occlusion.empty())
      {
        if (destroyer_queries_occlusion.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_queries_occlusion.front();
          destroyer_queries_occlusion.pop_front();
          free_occlusionqueries.push_back(item.first);
        }
        else
        {
          break;
        }
      }
      while (!destroyer_queries_timestamp.empty())
      {
        if (destroyer_queries_timestamp.front().second + BACKBUFFER_COUNT < FRAMECOUNT)
        {
          auto item = destroyer_queries_timestamp.front();
          destroyer_queries_timestamp.pop_front();
          free_timestampqueries.push_back(item.first);
        }
        else
        {
          break;
        }
      }
      destroylocker.unlock();
    }
  };
  std::shared_ptr<AllocationHandler> allocationhandler;
};

#endif // WICKEDENGINE_BUILD_VULKAN
