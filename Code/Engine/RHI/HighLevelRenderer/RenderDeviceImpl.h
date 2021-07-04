#pragma once
#include <RHI/HighLevelRenderer/RenderDevice.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/Instance.h>
#include <memory>

#include <memory>
#include <array>
#include <set>
#include <deque>

#include <Resource/Resource.h>

#include <RHI/HighLevelRenderer/RenderCommandList.h>
#include <RHI/HighLevelRenderer/RenderDevice.h>
#include <RHI/HighLevelRenderer/ObjectCache.h>

class RenderDeviceImpl : public RenderDevice
{
public:
    RenderDeviceImpl(const RenderDeviceDesc& settings, ezWindowBase* window);
    ~RenderDeviceImpl();

    ezSharedPtr<RenderCommandList> CreateRenderCommandList(CommandListType type) override;
    ezSharedPtr<Resource> CreateTexture(uint32_t bind_flag, ezRHIResourceFormat::Enum format, uint32_t sample_count, int width, int height, int depth, int mip_levels) override;
    ezSharedPtr<Resource> CreateBuffer(uint32_t bind_flag, uint32_t buffer_size, MemoryType memory_type) override;
    ezSharedPtr<Resource> CreateSampler(const SamplerDesc& desc) override;
    ezSharedPtr<Resource> CreateBottomLevelAS(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) override;
    ezSharedPtr<Resource> CreateTopLevelAS(uint32_t instance_count, BuildAccelerationStructureFlags flags) override;
    ezSharedPtr<View> CreateView(const ezSharedPtr<Resource>& resource, const ViewDesc& view_desc) override;
    ezSharedPtr<Shader> CreateShader(const ShaderDesc& desc, std::vector<uint8_t> byteCode, ezSharedPtr<ShaderReflection> reflection) override;
    ezSharedPtr<Program> CreateProgram(const std::vector<ezSharedPtr<Shader>>& shaders) override;
    bool IsDxrSupported() const override;
    bool IsRayQuerySupported() const override;
    bool IsVariableRateShadingSupported() const override;
    bool IsMeshShadingSupported() const override;
    uint32_t GetShadingRateImageTileSize() const override;
    uint32_t GetFrameIndex() const override;
    ezRHIResourceFormat::Enum GetFormat() const override;
    ezSharedPtr<Resource> GetBackBuffer(uint32_t buffer) override;
    const std::string& GetGpuName() const override;
    void ExecuteCommandLists(const std::vector<ezSharedPtr<RenderCommandList>>& command_lists) override;
    void Present() override;
    void Wait(uint64_t fence_value) override;
    void WaitForIdle() override;
    void Resize(uint32_t width, uint32_t height) override;

    void ExecuteCommandListsImpl(const std::vector<ezSharedPtr<RenderCommandList>>& command_lists);

private:
    void InsertPresentBarrier();

    ezWindowBase* m_window;
    bool m_vsync;
    uint32_t m_frame_count;
    uint32_t m_frame_index = 0;
    int m_width = 0;
    int m_height = 0;
    ezSharedPtr<Instance> m_instance;
    ezSharedPtr<Adapter> m_adapter;
    ezSharedPtr<Device> m_device;
    ezSharedPtr<CommandQueue> m_command_queue;
    ezSharedPtr<Swapchain> m_swapchain;
    std::vector<ezSharedPtr<CommandList>> m_barrier_command_lists;
    std::vector<uint64_t> m_frame_fence_values;
    uint64_t m_fence_value = 0;
    ezSharedPtr<Fence> m_fence;
    std::vector<ezSharedPtr<CommandList>> m_command_list_pool;
    std::deque<std::pair<uint64_t /*fence_value*/, size_t /*offset*/>> m_fence_value_by_cmd;
    ezUniquePtr<ObjectCache> m_object_cache;
};
