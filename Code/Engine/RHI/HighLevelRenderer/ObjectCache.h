#pragma once
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Pipeline/Pipeline.h>
#include <RHI/Device/Device.h>
#include <map>

class ObjectCache
{
public:
    ObjectCache(Device& device);

    ezSharedPtr<Pipeline> GetPipeline(const GraphicsPipelineDesc& desc);
    ezSharedPtr<Pipeline> GetPipeline(const ComputePipelineDesc& desc);
    ezSharedPtr<Pipeline> GetPipeline(const RayTracingPipelineDesc& desc);
    ezSharedPtr<RenderPass> GetRenderPass(const RenderPassDesc& desc);
    ezSharedPtr<BindingSetLayout> GetBindingSetLayout(const std::vector<BindKey>& keys);
    ezSharedPtr<BindingSet> GetBindingSet(const ezSharedPtr<BindingSetLayout>& layout, const std::vector<BindingDesc>& bindings);
    ezSharedPtr<Framebuffer> GetFramebuffer(const FramebufferDesc& desc);
    ezSharedPtr<View> GetView(const ezSharedPtr<Program>& program, const BindKey& bind_key, const ezSharedPtr<Resource>& resource, const LazyViewDesc& view_desc);

private:
    Device& m_device;
    std::map<GraphicsPipelineDesc, ezSharedPtr<Pipeline>> m_graphics_object_cache;
    std::map<ComputePipelineDesc, ezSharedPtr<Pipeline>> m_compute_object_cache;
    std::map<RayTracingPipelineDesc, ezSharedPtr<Pipeline>> m_ray_tracing_object_cache;
    std::map<RenderPassDesc, ezSharedPtr<RenderPass>> m_render_pass_cache;
    std::map<std::vector<BindKey>, ezSharedPtr<BindingSetLayout>> m_layout_cache;
    std::map<std::pair<ezSharedPtr<BindingSetLayout>, std::vector<BindingDesc>>, ezSharedPtr<BindingSet>> m_binding_set_cache;
    std::map<FramebufferDesc, ezSharedPtr<Framebuffer>> m_framebuffers;
    std::map<std::tuple<ezSharedPtr<Program>, BindKey, ezSharedPtr<Resource>, LazyViewDesc>, ezSharedPtr<View>> m_views;
};
