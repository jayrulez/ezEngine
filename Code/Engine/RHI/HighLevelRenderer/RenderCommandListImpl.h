#pragma once
#include <RHI/HighLevelRenderer/RenderCommandList.h>
#include <RHI/Device/Device.h>
#include <RHI/HighLevelRenderer/ObjectCache.h>

struct LazyResourceBarrierDesc
{
    ezSharedPtr<Resource> resource;
    ResourceState state;
    uint32_t base_mip_level = 0;
    uint32_t level_count = 1;
    uint32_t base_array_layer = 0;
    uint32_t layer_count = 1;
};

constexpr bool kUseFakeClose = true;

class RenderCommandListImpl : public RenderCommandList
{
public:
    RenderCommandListImpl(Device& device, ObjectCache& object_cache, CommandListType type);
    const ezSharedPtr<CommandList>& GetCommandList();
    void LazyResourceBarrier(const std::vector<LazyResourceBarrierDesc>& barriers);
    const std::map<ezSharedPtr<Resource>, ResourceStateTracker>& GetResourceStateTrackers() const;
    const std::vector<ResourceBarrierDesc>& GetLazyBarriers() const;

    void Reset() override;
    void Close() override;
    void Attach(const BindKey& bind_key, const ezSharedPtr<Resource>& resource = {}, const LazyViewDesc& view_desc = {}) override;
    void Attach(const BindKey& bind_key, const ezSharedPtr<DeferredView>& view) override;
    void Attach(const BindKey& bind_key, const ezSharedPtr<View>& view) override;
    void SetRasterizeState(const RasterizerDesc& desc) override;
    void SetBlendState(const BlendDesc& desc) override;
    void SetDepthStencilState(const DepthStencilDesc& desc) override;
    void UseProgram(const ezSharedPtr<Program>& program) override;
    void BeginRenderPass(const RenderPassBeginDesc& desc) override;
    void EndRenderPass() override;
    void BeginEvent(const std::string& name) override;
    void EndEvent() override;
    void Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) override;
    void DrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) override;
    void DrawIndirect(const ezSharedPtr<Resource>& argument_buffer, uint64_t argument_buffer_offset) override;
    void DrawIndexedIndirect(const ezSharedPtr<Resource>& argument_buffer, uint64_t argument_buffer_offset) override;
    void DrawIndirectCount(
        const ezSharedPtr<Resource>& argument_buffer,
        uint64_t argument_buffer_offset,
        const ezSharedPtr<Resource>& count_buffer,
        uint64_t count_buffer_offset,
        uint32_t max_draw_count,
        uint32_t stride) override;
    void DrawIndexedIndirectCount(
        const ezSharedPtr<Resource>& argument_buffer,
        uint64_t argument_buffer_offset,
        const ezSharedPtr<Resource>& count_buffer,
        uint64_t count_buffer_offset,
        uint32_t max_draw_count,
        uint32_t stride) override;
    void Dispatch(uint32_t thread_group_count_x, uint32_t thread_group_count_y, uint32_t thread_group_count_z) override;
    void DispatchIndirect(const ezSharedPtr<Resource>& argument_buffer, uint64_t argument_buffer_offset) override;
    void DispatchMesh(uint32_t thread_group_count_x) override;
    void DispatchRays(uint32_t width, uint32_t height, uint32_t depth) override;
    void SetViewport(float x, float y, float width, float height) override;
    void SetScissorRect(int32_t left, int32_t top, uint32_t right, uint32_t bottom) override;
    void IASetIndexBuffer(const ezSharedPtr<Resource>& resource, ezRHIResourceFormat::Enum format) override;
    void IASetVertexBuffer(uint32_t slot, const ezSharedPtr<Resource>& resource) override;
    void RSSetShadingRateImage(const ezSharedPtr<View>& view) override;
    void BuildBottomLevelAS(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags = BuildAccelerationStructureFlags::kNone) override;
    void BuildTopLevelAS(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, const std::vector<std::pair<ezSharedPtr<Resource>, ezMat4>>& geometry, BuildAccelerationStructureFlags flags = BuildAccelerationStructureFlags::kNone) override;
    void CopyAccelerationStructure(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, CopyAccelerationStructureMode mode) override;
    void CopyBuffer(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_buffer,
                            const std::vector<BufferCopyRegion>& regions) override;
    void CopyBufferToTexture(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_texture,
                                     const std::vector<BufferToTextureCopyRegion>& regions) override;
    void CopyTexture(const ezSharedPtr<Resource>& src_texture, const ezSharedPtr<Resource>& dst_texture,
                             const std::vector<TextureCopyRegion>& regions) override;
    void UpdateSubresource(const ezSharedPtr<Resource>& resource, uint32_t subresource, const void* data, uint32_t row_pitch = 0, uint32_t depth_pitch = 0) override;
    uint64_t& GetFenceValue() override;

private:
    void BufferBarrier(const ezSharedPtr<Resource>& resource, ResourceState state);
    void ViewBarrier(const ezSharedPtr<View>& view, ResourceState state);
    void ImageBarrier(const ezSharedPtr<Resource>& resource, uint32_t base_mip_level, uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count, ResourceState state);
    void OnAttachSRV(const BindKey& bind_key, const ezSharedPtr<View>& view);
    void OnAttachUAV(const BindKey& bind_key, const ezSharedPtr<View>& view);
    void SetBinding(const BindKey& bind_key, const ezSharedPtr<View>& view);
    void UpdateDefaultSubresource(const ezSharedPtr<Resource>& resource, uint32_t subresource, const void* data, uint32_t row_pitch, uint32_t depth_pitch);
    void Apply();
    void ApplyPipeline();
    void ApplyBindingSet();
    ResourceStateTracker& GetResourceStateTracker(const ezSharedPtr<Resource>& resource);
    void CreateShaderTable(ezSharedPtr<Pipeline> pipeline);

    Device& m_device;
    ObjectCache& m_object_cache;
    std::vector<ezSharedPtr<Framebuffer>> m_framebuffers;
    ezSharedPtr<CommandList> m_command_list;
    uint32_t m_viewport_width = 0;
    uint32_t m_viewport_height = 0;

    std::map<BindKey, ezSharedPtr<View>> m_bound_resources;
    std::map<BindKey, ezSharedPtr<DeferredView>> m_bound_deferred_view;
    std::vector<ezSharedPtr<ResourceLazyViewDesc>> m_resource_lazy_view_descs;

    ezSharedPtr<Program> m_program;
    std::vector<ezSharedPtr<BindingSet>> m_binding_sets;

    ComputePipelineDesc m_compute_pipeline_desc = {};
    RayTracingPipelineDesc m_ray_tracing_pipeline_desc = {};
    GraphicsPipelineDesc m_graphic_pipeline_desc = {};

    std::vector<ezSharedPtr<Resource>> m_cmd_resources;

    std::map<ezSharedPtr<Resource>, ResourceStateTracker> m_resource_state_tracker;
    std::vector<ResourceBarrierDesc> m_lazy_barriers;
    uint64_t m_fence_value = 0;
    ezSharedPtr<BindingSetLayout> m_layout;

    RayTracingShaderTables m_shader_tables = {};
    ezSharedPtr<Resource> m_shader_table;
    PipelineType m_pipeline_type = PipelineType::kGraphics;
    ezSharedPtr<Pipeline> m_pipeline;
    ezSharedPtr<View> m_shading_rate_image;
    ShadingRateCombiner m_shading_rate_combiner = ShadingRateCombiner::kPassthrough;
};
