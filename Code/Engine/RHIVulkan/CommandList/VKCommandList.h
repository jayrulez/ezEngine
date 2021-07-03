#pragma once
#include <RHI/CommandList/CommandList.h>
#include <vulkan/vulkan.hpp>

class VKDevice;
class VKPipeline;

class VKCommandList : public CommandList
{
public:
    VKCommandList(VKDevice& device, CommandListType type);
    void Reset() override;
    void Close() override;
    void BindPipeline(const ezSharedPtr<Pipeline>& state) override;
    void BindBindingSet(const ezSharedPtr<BindingSet>& binding_set) override;
    void BeginRenderPass(const ezSharedPtr<RenderPass>& render_pass, const ezSharedPtr<Framebuffer>& framebuffer, const ClearDesc& clear_desc) override;
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
    void DispatchRays(const RayTracingShaderTables& shader_tables, uint32_t width, uint32_t height, uint32_t depth) override;
    void ResourceBarrier(const std::vector<ResourceBarrierDesc>& barriers) override;
    void UAVResourceBarrier(const ezSharedPtr<Resource>& resource) override;
    void SetViewport(float x, float y, float width, float height) override;
    void SetScissorRect(int32_t left, int32_t top, uint32_t right, uint32_t bottom) override;
    void IASetIndexBuffer(const ezSharedPtr<Resource>& resource, ezRHIResourceFormat::Enum format) override;
    void IASetVertexBuffer(uint32_t slot, const ezSharedPtr<Resource>& resource) override;
    void RSSetShadingRate(ShadingRate shading_rate, const std::array<ShadingRateCombiner, 2>& combiners) override;
    void BuildBottomLevelAS(
        const ezSharedPtr<Resource>& src,
        const ezSharedPtr<Resource>& dst,
        const ezSharedPtr<Resource>& scratch,
        uint64_t scratch_offset,
        const std::vector<RaytracingGeometryDesc>& descs,
        BuildAccelerationStructureFlags flags) override;
    void BuildTopLevelAS(
        const ezSharedPtr<Resource>& src,
        const ezSharedPtr<Resource>& dst,
        const ezSharedPtr<Resource>& scratch,
        uint64_t scratch_offset,
        const ezSharedPtr<Resource>& instance_data,
        uint64_t instance_offset,
        uint32_t instance_count,
        BuildAccelerationStructureFlags flags) override;
    void CopyAccelerationStructure(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, CopyAccelerationStructureMode mode) override;
    void CopyBuffer(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_buffer,
                    const std::vector<BufferCopyRegion>& regions) override;
    void CopyBufferToTexture(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_texture,
                             const std::vector<BufferToTextureCopyRegion>& regions) override;
    void CopyTexture(const ezSharedPtr<Resource>& src_texture, const ezSharedPtr<Resource>& dst_texture,
                     const std::vector<TextureCopyRegion>& regions) override;
    void WriteAccelerationStructuresProperties(
        const std::vector<ezSharedPtr<Resource>>& acceleration_structures,
        const ezSharedPtr<QueryHeap>& query_heap,
        uint32_t first_query) override;
    void ResolveQueryData(
        const ezSharedPtr<QueryHeap>& query_heap,
        uint32_t first_query,
        uint32_t query_count,
        const ezSharedPtr<Resource>& dst_buffer,
        uint64_t dst_offset) override;

    vk::CommandBuffer GetCommandList();

private:
    void BuildAccelerationStructure(vk::AccelerationStructureCreateInfoKHR& build_info, const vk::Buffer& instance_data, uint64_t instance_offset, const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, const ezSharedPtr<Resource>& scratch, uint64_t scratch_offset);

    VKDevice& m_device;
    vk::UniqueCommandBuffer m_command_list;
    bool m_closed = false;
    ezSharedPtr<VKPipeline> m_state;
    ezSharedPtr<BindingSet> m_binding_set;
};
