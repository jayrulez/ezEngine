#pragma once
#include <RHI/Device/Device.h>
#include <RHIDX12/CPUDescriptorPool/DXCPUDescriptorPool.h>
#include <RHIDX12/GPUDescriptorPool/DXGPUDescriptorPool.h>
#include <dxgi.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXAdapter;
class DXCommandQueue;

class DXDevice : public Device
{
public:
    DXDevice(DXAdapter& adapter);
    ezSharedPtr<Memory> AllocateMemory(uint64_t size, MemoryType memory_type, uint32_t memory_type_bits) override;
    ezSharedPtr<CommandQueue> GetCommandQueue(CommandListType type) override;
    uint32_t GetTextureDataPitchAlignment() const override;
    ezSharedPtr<Swapchain> CreateSwapchain(Window window, uint32_t width, uint32_t height, uint32_t frame_count, bool vsync) override;
    ezSharedPtr<CommandList> CreateCommandList(CommandListType type) override;
    ezSharedPtr<Fence> CreateFence(uint64_t initial_value) override;
    ezSharedPtr<Resource> CreateTexture(TextureType type, uint32_t bind_flag, ezRHIResourceFormat::Enum format, uint32_t sample_count, int width, int height, int depth, int mip_levels) override;
    ezSharedPtr<Resource> CreateBuffer(uint32_t bind_flag, uint32_t buffer_size) override;
    ezSharedPtr<Resource> CreateSampler(const SamplerDesc& desc) override;
    ezSharedPtr<View> CreateView(const ezSharedPtr<Resource>& resource, const ViewDesc& view_desc) override;
    ezSharedPtr<BindingSetLayout> CreateBindingSetLayout(const std::vector<BindKey>& descs) override;
    ezSharedPtr<BindingSet> CreateBindingSet(const ezSharedPtr<BindingSetLayout>& layout) override;
    ezSharedPtr<RenderPass> CreateRenderPass(const RenderPassDesc& desc) override;
    ezSharedPtr<Framebuffer> CreateFramebuffer(const FramebufferDesc& desc) override;
    ezSharedPtr<Shader> CreateShader(const ShaderDesc& desc, std::vector<uint8_t> byteCode, ezSharedPtr<ShaderReflection> reflection) override;
    ezSharedPtr<Program> CreateProgram(const std::vector<ezSharedPtr<Shader>>& shaders) override;
    ezSharedPtr<Pipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) override;
    ezSharedPtr<Pipeline> CreateComputePipeline(const ComputePipelineDesc& desc) override;
    ezSharedPtr<Pipeline> CreateRayTracingPipeline(const RayTracingPipelineDesc& desc) override;
    ezSharedPtr<Resource> CreateAccelerationStructure(AccelerationStructureType type, const ezSharedPtr<Resource>& resource, uint64_t offset) override;
    ezSharedPtr<QueryHeap> CreateQueryHeap(QueryHeapType type, uint32_t count) override;
    bool IsDxrSupported() const override;
    bool IsRayQuerySupported() const override;
    bool IsVariableRateShadingSupported() const override;
    bool IsMeshShadingSupported() const override;
    uint32_t GetShadingRateImageTileSize() const override;
    MemoryBudget GetMemoryBudget() const override;
    uint32_t GetShaderGroupHandleSize() const override;
    uint32_t GetShaderRecordAlignment() const override;
    uint32_t GetShaderTableAlignment() const override;
    RaytracingASPrebuildInfo GetBLASPrebuildInfo(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) const override;
    RaytracingASPrebuildInfo GetTLASPrebuildInfo(uint32_t instance_count, BuildAccelerationStructureFlags flags) const override;

    DXAdapter& GetAdapter();
    ComPtr<ID3D12Device> GetDevice();
    DXCPUDescriptorPool& GetCPUDescriptorPool();
    DXGPUDescriptorPool& GetGPUDescriptorPool();
    bool IsRenderPassesSupported() const;
    bool IsUnderGraphicsDebugger() const;
    bool IsCreateNotZeroedAvailable() const;
    ID3D12CommandSignature* GetCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE type, uint32_t stride);

private:
    RaytracingASPrebuildInfo GetAccelerationStructurePrebuildInfo(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs) const;

    DXAdapter& m_adapter;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Device5> m_device5;
    std::map<CommandListType, ezSharedPtr<DXCommandQueue>> m_command_queues;
    DXCPUDescriptorPool m_cpu_descriptor_pool;
    DXGPUDescriptorPool m_gpu_descriptor_pool;
    bool m_is_dxr_supported = false;
    bool m_is_ray_query_supported = false;
    bool m_is_render_passes_supported = false;
    bool m_is_variable_rate_shading_supported = false;
    bool m_is_mesh_shading_supported = false;
    uint32_t m_shading_rate_image_tile_size = 0;
    bool m_is_under_graphics_debugger = false;
    bool m_is_create_not_zeroed_available = false;
    std::map<std::pair<D3D12_INDIRECT_ARGUMENT_TYPE, uint32_t>, ComPtr<ID3D12CommandSignature>> m_command_signature_cache;
};

D3D12_RESOURCE_STATES ConvertState(ResourceState state);
D3D12_HEAP_TYPE GetHeapType(MemoryType memory_type);
D3D12_RAYTRACING_GEOMETRY_DESC FillRaytracingGeometryDesc(const BufferDesc& vertex, const BufferDesc& index, RaytracingGeometryFlags flags);
D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS Convert(BuildAccelerationStructureFlags flags);
