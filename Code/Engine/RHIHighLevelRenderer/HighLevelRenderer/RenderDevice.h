#pragma once
#include <RHI/RHIDLL.h>
#include <RHIHighLevelRenderer/RHIHighLevelRendererDLL.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/RenderCommandList.h>
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

class EZ_RHIHIGHLEVELRENDERER_DLL RenderDevice : public QueryInterface
{
public:
    virtual ~RenderDevice() = default;
    virtual std::shared_ptr<RenderCommandList> CreateRenderCommandList(CommandListType type = CommandListType::kGraphics) = 0;
    virtual std::shared_ptr<Resource> CreateTexture(uint32_t bind_flag, ezRHIResourceFormat::Enum format, uint32_t sample_count, int width, int height, int depth = 1, int mip_levels = 1) = 0;
    virtual std::shared_ptr<Resource> CreateBuffer(uint32_t bind_flag, uint32_t buffer_size, MemoryType memory_type = MemoryType::kDefault) = 0;
    virtual std::shared_ptr<Resource> CreateSampler(const SamplerDesc& desc) = 0;
    virtual std::shared_ptr<Resource> CreateBottomLevelAS(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) = 0;
    virtual std::shared_ptr<Resource> CreateTopLevelAS(uint32_t instance_count, BuildAccelerationStructureFlags flags) = 0;
    virtual std::shared_ptr<View> CreateView(const std::shared_ptr<Resource>& resource, const ViewDesc& view_desc) = 0;
    virtual std::shared_ptr<Shader> CreateShader(const ShaderDesc& desc, std::vector<uint8_t> byteCode, std::shared_ptr<ShaderReflection> reflection) = 0;
    virtual std::shared_ptr<Program> CreateProgram(const std::vector<std::shared_ptr<Shader>>& shaders) = 0;
    virtual bool IsDxrSupported() const = 0;
    virtual bool IsRayQuerySupported() const = 0;
    virtual bool IsVariableRateShadingSupported() const = 0;
    virtual bool IsMeshShadingSupported() const = 0;
    virtual uint32_t GetShadingRateImageTileSize() const = 0;
    virtual uint32_t GetFrameIndex() const = 0;
    virtual ezRHIResourceFormat::Enum GetFormat() const = 0;
    virtual std::shared_ptr<Resource> GetBackBuffer(uint32_t buffer) = 0;
    virtual const ezString& GetGpuName() const = 0;
    virtual void ExecuteCommandLists(const std::vector<std::shared_ptr<RenderCommandList>>& command_lists) = 0;
    virtual void Present() = 0;
    virtual void Wait(uint64_t fence_value) = 0;
    virtual void WaitForIdle() = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;
};

EZ_RHIHIGHLEVELRENDERER_DLL std::shared_ptr<RenderDevice> CreateRenderDevice(const RenderDeviceDesc& settings, ezWindowBase* window);
