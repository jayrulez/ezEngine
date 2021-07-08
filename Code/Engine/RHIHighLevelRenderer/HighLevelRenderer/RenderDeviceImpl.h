#pragma once
#include <RHIHighLevelRenderer/RHIHighLevelRendererDLL.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/RenderDevice.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/Instance.h>
#include <memory>

#include <memory>
#include <array>
#include <set>
#include <deque>

#include <RHI/Resource/Resource.h>

#include <RHIHighLevelRenderer/HighLevelRenderer/RenderCommandList.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/RenderDevice.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/ObjectCache.h>

class EZ_RHIHIGHLEVELRENDERER_DLL RenderDeviceImpl : public RenderDevice
{
public:
    RenderDeviceImpl(const RenderDeviceDesc& settings, ezWindowBase* window);
    ~RenderDeviceImpl();

    std::shared_ptr<RenderCommandList> CreateRenderCommandList(CommandListType type) override;
    std::shared_ptr<Resource> CreateTexture(uint32_t bind_flag, ezRHIResourceFormat::Enum format, uint32_t sample_count, int width, int height, int depth, int mip_levels) override;
    std::shared_ptr<Resource> CreateBuffer(uint32_t bind_flag, uint32_t buffer_size, MemoryType memory_type) override;
    std::shared_ptr<Resource> CreateSampler(const SamplerDesc& desc) override;
    std::shared_ptr<Resource> CreateBottomLevelAS(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) override;
    std::shared_ptr<Resource> CreateTopLevelAS(uint32_t instance_count, BuildAccelerationStructureFlags flags) override;
    std::shared_ptr<View> CreateView(const std::shared_ptr<Resource>& resource, const ViewDesc& view_desc) override;
    std::shared_ptr<Shader> CreateShader(const ShaderDesc& desc, std::vector<uint8_t> byteCode, std::shared_ptr<ShaderReflection> reflection) override;
    std::shared_ptr<Program> CreateProgram(const std::vector<std::shared_ptr<Shader>>& shaders) override;
    bool IsDxrSupported() const override;
    bool IsRayQuerySupported() const override;
    bool IsVariableRateShadingSupported() const override;
    bool IsMeshShadingSupported() const override;
    uint32_t GetShadingRateImageTileSize() const override;
    uint32_t GetFrameIndex() const override;
    ezRHIResourceFormat::Enum GetFormat() const override;
    std::shared_ptr<Resource> GetBackBuffer(uint32_t buffer) override;
    const std::string& GetGpuName() const override;
    void ExecuteCommandLists(const std::vector<std::shared_ptr<RenderCommandList>>& command_lists) override;
    void Present() override;
    void Wait(uint64_t fence_value) override;
    void WaitForIdle() override;
    void Resize(uint32_t width, uint32_t height) override;

    void ExecuteCommandListsImpl(const std::vector<std::shared_ptr<RenderCommandList>>& command_lists);

private:
    void InsertPresentBarrier();

    ezWindowBase* m_window;
    bool m_vsync;
    uint32_t m_frame_count;
    uint32_t m_frame_index = 0;
    int m_width = 0;
    int m_height = 0;
    std::shared_ptr<Instance> m_instance;
    std::shared_ptr<Adapter> m_adapter;
    std::shared_ptr<Device> m_device;
    std::shared_ptr<CommandQueue> m_command_queue;
    std::shared_ptr<Swapchain> m_swapchain;
    std::vector<std::shared_ptr<CommandList>> m_barrier_command_lists;
    std::vector<uint64_t> m_frame_fence_values;
    uint64_t m_fence_value = 0;
    std::shared_ptr<Fence> m_fence;
    std::vector<std::shared_ptr<CommandList>> m_command_list_pool;
    std::deque<std::pair<uint64_t /*fence_value*/, size_t /*offset*/>> m_fence_value_by_cmd;
    std::unique_ptr<ObjectCache> m_object_cache;
};
