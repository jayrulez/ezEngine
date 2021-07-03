#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Resource/Resource.h>
#include <RHI/Instance/BaseTypes.h>

class RenderCommandList : public QueryInterface
{
public:
    virtual ~RenderCommandList() = default;
    virtual void Reset() = 0;
    virtual void Close() = 0;
    virtual void Attach(const BindKey& bind_key, const ezSharedPtr<Resource>& resource = {}, const LazyViewDesc& view_desc = {}) = 0;
    virtual void Attach(const BindKey& bind_key, const ezSharedPtr<DeferredView>& view) = 0;
    virtual void Attach(const BindKey& bind_key, const ezSharedPtr<View>& view) = 0;
    virtual void SetRasterizeState(const RasterizerDesc& desc) = 0;
    virtual void SetBlendState(const BlendDesc& desc) = 0;
    virtual void SetDepthStencilState(const DepthStencilDesc& desc) = 0;
    virtual void UseProgram(const ezSharedPtr<Program>& program) = 0;
    virtual void BeginRenderPass(const RenderPassBeginDesc& desc) = 0;
    virtual void EndRenderPass() = 0;
    virtual void BeginEvent(const std::string& name) = 0;
    virtual void EndEvent() = 0;
    virtual void Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) = 0;
    virtual void DrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) = 0;
    virtual void DrawIndirect(const ezSharedPtr<Resource>& argument_buffer, uint64_t argument_buffer_offset) = 0;
    virtual void DrawIndexedIndirect(const ezSharedPtr<Resource>& argument_buffer, uint64_t argument_buffer_offset) = 0;
    virtual void DrawIndirectCount(
        const ezSharedPtr<Resource>& argument_buffer,
        uint64_t argument_buffer_offset,
        const ezSharedPtr<Resource>& count_buffer,
        uint64_t count_buffer_offset,
        uint32_t max_draw_count,
        uint32_t stride) = 0;
    virtual void DrawIndexedIndirectCount(
        const ezSharedPtr<Resource>& argument_buffer,
        uint64_t argument_buffer_offset,
        const ezSharedPtr<Resource>& count_buffer,
        uint64_t count_buffer_offset,
        uint32_t max_draw_count,
        uint32_t stride) = 0;
    virtual void Dispatch(uint32_t thread_group_count_x, uint32_t thread_group_count_y, uint32_t thread_group_count_z) = 0;
    virtual void DispatchIndirect(const ezSharedPtr<Resource>& argument_buffer, uint64_t argument_buffer_offset) = 0;
    virtual void DispatchMesh(uint32_t thread_group_count_x) = 0;
    virtual void DispatchRays(uint32_t width, uint32_t height, uint32_t depth) = 0;
    virtual void SetViewport(float x, float y, float width, float height) = 0;
    virtual void SetScissorRect(int32_t left, int32_t top, uint32_t right, uint32_t bottom) = 0;
    virtual void IASetIndexBuffer(const ezSharedPtr<Resource>& resource, ezRHIResourceFormat::Enum format) = 0;
    virtual void IASetVertexBuffer(uint32_t slot, const ezSharedPtr<Resource>& resource) = 0;
    virtual void RSSetShadingRateImage(const ezSharedPtr<View>& view) = 0;
    virtual void BuildBottomLevelAS(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags = BuildAccelerationStructureFlags::kNone) = 0;
    virtual void BuildTopLevelAS(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, const std::vector<std::pair<ezSharedPtr<Resource>, ezMat4>>& geometry, BuildAccelerationStructureFlags flags = BuildAccelerationStructureFlags::kNone) = 0;
    virtual void CopyAccelerationStructure(const ezSharedPtr<Resource>& src, const ezSharedPtr<Resource>& dst, CopyAccelerationStructureMode mode) = 0;
    virtual void CopyBuffer(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_buffer,
                            const std::vector<BufferCopyRegion>& regions) = 0;
    virtual void CopyBufferToTexture(const ezSharedPtr<Resource>& src_buffer, const ezSharedPtr<Resource>& dst_texture,
                                     const std::vector<BufferToTextureCopyRegion>& regions) = 0;
    virtual void CopyTexture(const ezSharedPtr<Resource>& src_texture, const ezSharedPtr<Resource>& dst_texture,
                             const std::vector<TextureCopyRegion>& regions) = 0;
    virtual void UpdateSubresource(const ezSharedPtr<Resource>& resource, uint32_t subresource, const void* data, uint32_t row_pitch = 0, uint32_t depth_pitch = 0) = 0;
    virtual uint64_t& GetFenceValue() = 0;
};
