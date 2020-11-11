#pragma once

#ifdef EZ_RHI_D3D12_SUPPORTED
#  include <RHI/DX12/D3D12MemAlloc.h>
#  include <RHI/DX12/DX12_Internal.h>
#  include <RHI/GraphicsDevice.h>
#  include <RHI/RHIDLL.h>
#  include <RHI/RHIPCH.h>

#  include <d3d12.h>
#  include <dxgi1_4.h>
#  include <wrl/client.h> // ComPtr

#  include <atomic>
#  include <mutex>


EZ_DEFINE_AS_POD_TYPE(D3D12_STATIC_SAMPLER_DESC);
EZ_DEFINE_AS_POD_TYPE(D3D12_SHADER_RESOURCE_VIEW_DESC);
EZ_DEFINE_AS_POD_TYPE(D3D12_UNORDERED_ACCESS_VIEW_DESC);
EZ_DEFINE_AS_POD_TYPE(D3D12_RENDER_TARGET_VIEW_DESC);
EZ_DEFINE_AS_POD_TYPE(D3D12_DEPTH_STENCIL_VIEW_DESC);
EZ_DEFINE_AS_POD_TYPE(D3D12_DESCRIPTOR_RANGE);
EZ_DEFINE_AS_POD_TYPE(D3D12_INPUT_ELEMENT_DESC);
EZ_DEFINE_AS_POD_TYPE(D3D12_SUBRESOURCE_DATA);
EZ_DEFINE_AS_POD_TYPE(D3D12_PLACED_SUBRESOURCE_FOOTPRINT);
EZ_DEFINE_AS_POD_TYPE(D3D12_RAYTRACING_GEOMETRY_DESC);
EZ_DEFINE_AS_POD_TYPE(D3D12_EXPORT_DESC);
EZ_DEFINE_AS_POD_TYPE(D3D12_DXIL_LIBRARY_DESC);
EZ_DEFINE_AS_POD_TYPE(D3D12_HIT_GROUP_DESC);
EZ_DEFINE_AS_POD_TYPE(D3D12_ROOT_PARAMETER);
EZ_DEFINE_AS_POD_TYPE(D3D12_STATE_SUBOBJECT);

using namespace DX12_Internal;

class EZ_RHI_DLL GraphicsDevice_DX12 : public GraphicsDevice
{
private:
  Microsoft::WRL::ComPtr<ID3D12Device5> device;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> directQueue;
  Microsoft::WRL::ComPtr<ID3D12Fence> frameFence;
  HANDLE frameFenceEvent;

  ezUInt32 backbuffer_index = 0;
  Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers[BACKBUFFER_COUNT];

  Microsoft::WRL::ComPtr<ID3D12CommandSignature> dispatchIndirectCommandSignature;
  Microsoft::WRL::ComPtr<ID3D12CommandSignature> drawInstancedIndirectCommandSignature;
  Microsoft::WRL::ComPtr<ID3D12CommandSignature> drawIndexedInstancedIndirectCommandSignature;
  Microsoft::WRL::ComPtr<ID3D12CommandSignature> dispatchMeshIndirectCommandSignature;

  Microsoft::WRL::ComPtr<ID3D12QueryHeap> querypool_timestamp;
  Microsoft::WRL::ComPtr<ID3D12QueryHeap> querypool_occlusion;
  Microsoft::WRL::ComPtr<ID3D12Resource> querypool_timestamp_readback;
  Microsoft::WRL::ComPtr<ID3D12Resource> querypool_occlusion_readback;
  D3D12MA::Allocation* allocation_querypool_timestamp_readback = nullptr;
  D3D12MA::Allocation* allocation_querypool_occlusion_readback = nullptr;

  D3D12_FEATURE_DATA_D3D12_OPTIONS features_0;
  D3D12_FEATURE_DATA_D3D12_OPTIONS5 features_5;
  D3D12_FEATURE_DATA_D3D12_OPTIONS6 features_6;
  D3D12_FEATURE_DATA_D3D12_OPTIONS7 features_7;

  ezUInt32 rtv_descriptor_size = 0;
  ezUInt32 dsv_descriptor_size = 0;
  ezUInt32 resource_descriptor_size = 0;
  ezUInt32 sampler_descriptor_size = 0;

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorheap_RTV;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorheap_DSV;

  D3D12_CPU_DESCRIPTOR_HANDLE rtv_descriptor_heap_start = {};
  D3D12_CPU_DESCRIPTOR_HANDLE dsv_descriptor_heap_start = {};

  std::mutex copyQueueMutex;
  bool copyQueueUse = false;
  Microsoft::WRL::ComPtr<ID3D12Fence> copyFence; // GPU only

  struct FrameResources
  {
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[COMMANDLIST_COUNT];
    Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[COMMANDLIST_COUNT];

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> copyQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> copyAllocator;
    Microsoft::WRL::ComPtr<ID3D12CommandList> copyCommandList;

    struct DescriptorTableFrameAllocator
    {
      GraphicsDevice_DX12* device = nullptr;
      struct DescriptorHeap
      {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_GPU;
        D3D12_CPU_DESCRIPTOR_HANDLE start_cpu = {};
        D3D12_GPU_DESCRIPTOR_HANDLE start_gpu = {};
        ezUInt32 ringOffset = 0;
      };
      ezDynamicArray<DescriptorHeap> heaps_resource;
      ezDynamicArray<DescriptorHeap> heaps_sampler;
      ezUInt32 current_resource_heap = 0;
      ezUInt32 current_sampler_heap = 0;
      bool heaps_bound = false;
      bool dirty = false;

      const GPUBuffer* CBV[GPU_RESOURCE_HEAP_CBV_COUNT];
      const GPUResource* SRV[GPU_RESOURCE_HEAP_SRV_COUNT];
      int SRV_index[GPU_RESOURCE_HEAP_SRV_COUNT];
      const GPUResource* UAV[GPU_RESOURCE_HEAP_UAV_COUNT];
      int UAV_index[GPU_RESOURCE_HEAP_UAV_COUNT];
      const Sampler* SAM[GPU_SAMPLER_HEAP_COUNT];

      struct DescriptorHandles
      {
        D3D12_GPU_DESCRIPTOR_HANDLE sampler_handle = {};
        D3D12_GPU_DESCRIPTOR_HANDLE resource_handle = {};
      };

      void init(GraphicsDevice_DX12* device);

      void reset();
      void request_heaps(ezUInt32 resources, ezUInt32 samplers, CommandList cmd);
      void validate(bool graphics, CommandList cmd);
      DescriptorHandles commit(const DescriptorTable* table, CommandList cmd);
    };
    DescriptorTableFrameAllocator descriptors[COMMANDLIST_COUNT];

    struct ResourceFrameAllocator
    {
      GraphicsDevice_DX12* device = nullptr;
      GPUBuffer buffer;
      ezUInt8* dataBegin = nullptr;
      ezUInt8* dataCur = nullptr;
      ezUInt8* dataEnd = nullptr;

      void init(GraphicsDevice_DX12* device, size_t size);

      ezUInt8* allocate(size_t dataSize, size_t alignment);
      void clear();
      ezUInt64 calculateOffset(ezUInt8* address);
    };
    ResourceFrameAllocator resourceBuffer[COMMANDLIST_COUNT];
  };
  FrameResources frames[BACKBUFFER_COUNT];
  FrameResources& GetFrameResources() { return frames[GetFrameCount() % BACKBUFFER_COUNT]; }
  inline ID3D12GraphicsCommandList6* GetDirectCommandList(CommandList cmd) { return static_cast<ID3D12GraphicsCommandList6*>(GetFrameResources().commandLists[cmd].Get()); }

  Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;

  ezRHIPrimitiveTopology::Enum prev_pt[COMMANDLIST_COUNT] = {};

  std::unordered_map<size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelines_global;
  ezDynamicArray<std::pair<size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>>> pipelines_worker[COMMANDLIST_COUNT];
  size_t prev_pipeline_hash[COMMANDLIST_COUNT] = {};
  const PipelineState* active_pso[COMMANDLIST_COUNT] = {};
  const Shader* active_cs[COMMANDLIST_COUNT] = {};
  const RaytracingPipelineState* active_rt[COMMANDLIST_COUNT] = {};
  const RootSignature* active_rootsig_graphics[COMMANDLIST_COUNT] = {};
  const RootSignature* active_rootsig_compute[COMMANDLIST_COUNT] = {};
  const RenderPass* active_renderpass[COMMANDLIST_COUNT] = {};
  D3D12_RENDER_PASS_ENDING_ACCESS_RESOLVE_SUBRESOURCE_PARAMETERS resolve_subresources[COMMANDLIST_COUNT][D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
  D3D12_SHADING_RATE prev_shadingrate[COMMANDLIST_COUNT] = {};

  bool dirty_pso[COMMANDLIST_COUNT] = {};
  void pso_validate(CommandList cmd);

  void predraw(CommandList cmd);
  void predispatch(CommandList cmd);
  void preraytrace(CommandList cmd);

  struct Query_Resolve
  {
    EZ_DECLARE_POD_TYPE();
    GPU_QUERY_TYPE type;
    UINT index;
  };
  ezDynamicArray<Query_Resolve> query_resolves[COMMANDLIST_COUNT] = {};

  std::atomic<CommandList> cmd_count{0};

public:
  GraphicsDevice_DX12(RHIWindowType window, bool fullscreen = false, bool debuglayer = false);
  virtual ~GraphicsDevice_DX12();

  bool CreateBuffer(const GPUBufferDesc* pDesc, const SubresourceData* pInitialData, GPUBuffer* pBuffer) override;
  bool CreateTexture(const TextureDesc* pDesc, const SubresourceData* pInitialData, Texture* pTexture) override;
  bool CreateInputLayout(const InputLayoutDesc* pInputElementDescs, ezUInt32 NumElements, const Shader* shader, InputLayout* pInputLayout) override;
  bool CreateShader(ezEnum<ezRHIShaderStage> stage, const void* pShaderBytecode, size_t BytecodeLength, Shader* pShader) override;
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

  void WriteShadingRateValue(SHADING_RATE rate, void* dest) override;
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
  void BindShadingRate(SHADING_RATE rate, CommandList cmd) override;
  void BindShadingRateImage(const Texture* texture, CommandList cmd) override;
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

  std::shared_ptr<DX12AllocationHandler> allocationhandler;
};

#endif // WICKEDENGINE_BUILD_DX12
