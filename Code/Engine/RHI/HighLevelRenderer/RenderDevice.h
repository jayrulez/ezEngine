#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/HighLevelRenderer/RenderCommandList.h>
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Swapchain/Swapchain.h>
#include <RHI/CommandList/CommandList.h>
#include <RHI/Fence/Fence.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Program/Program.h>
#include <RHI/Framebuffer/Framebuffer.h>
#include <RHI/Pipeline/Pipeline.h>
#include <RHI/Shader/Shader.h>
#include <RHI/RenderPass/RenderPass.h>
#include <RHI/CommandQueue/CommandQueue.h>
#include <memory>
#include <vector>

class ezWindowBase;

class RenderDevice : public QueryInterface
{
public:
    virtual ~RenderDevice() = default;
    virtual ezSharedPtr<RenderCommandList> CreateRenderCommandList(CommandListType type = CommandListType::kGraphics) = 0;
    virtual ezSharedPtr<Resource> CreateTexture(uint32_t bind_flag, ezRHIResourceFormat::Enum format, uint32_t sample_count, int width, int height, int depth = 1, int mip_levels = 1) = 0;
    virtual ezSharedPtr<Resource> CreateBuffer(uint32_t bind_flag, uint32_t buffer_size, MemoryType memory_type = MemoryType::kDefault) = 0;
    virtual ezSharedPtr<Resource> CreateSampler(const SamplerDesc& desc) = 0;
    virtual ezSharedPtr<Resource> CreateBottomLevelAS(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) = 0;
    virtual ezSharedPtr<Resource> CreateTopLevelAS(uint32_t instance_count, BuildAccelerationStructureFlags flags) = 0;
    virtual ezSharedPtr<View> CreateView(const ezSharedPtr<Resource>& resource, const ViewDesc& view_desc) = 0;
    virtual ezSharedPtr<Shader> CreateShader(const ShaderDesc& desc, std::vector<uint8_t> byteCode, ezSharedPtr<ShaderReflection> reflection) = 0;
    virtual ezSharedPtr<Program> CreateProgram(const std::vector<ezSharedPtr<Shader>>& shaders) = 0;
    virtual bool IsDxrSupported() const = 0;
    virtual bool IsRayQuerySupported() const = 0;
    virtual bool IsVariableRateShadingSupported() const = 0;
    virtual bool IsMeshShadingSupported() const = 0;
    virtual uint32_t GetShadingRateImageTileSize() const = 0;
    virtual uint32_t GetFrameIndex() const = 0;
    virtual ezRHIResourceFormat::Enum GetFormat() const = 0;
    virtual ezSharedPtr<Resource> GetBackBuffer(uint32_t buffer) = 0;
    virtual const std::string& GetGpuName() const = 0;
    virtual void ExecuteCommandLists(const std::vector<ezSharedPtr<RenderCommandList>>& command_lists) = 0;
    virtual void Present() = 0;
    virtual void Wait(uint64_t fence_value) = 0;
    virtual void WaitForIdle() = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;
};

EZ_RHI_DLL ezSharedPtr<RenderDevice> CreateRenderDevice(const RenderDeviceDesc& settings, ezWindowBase* window);
