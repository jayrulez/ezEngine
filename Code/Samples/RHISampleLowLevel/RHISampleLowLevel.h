#pragma once

#include <memory>
#include <Foundation/Application/Application.h>
#include <Foundation/Types/UniquePtr.h>

#include <RHI/Device/Device.h>
#include <RHI/Swapchain/Swapchain.h>
#include <RHI/Fence/Fence.h>
#include <RHI/Instance/Instance.h>

class ezRHISampleWindow;

constexpr uint32_t frame_count = 3;

class ezRHISampleApp : public ezApplication
{
public:
  typedef ezApplication SUPER;

  ezRHISampleApp();

  virtual Execution Run() override;

  virtual void AfterCoreSystemsStartup() override;

  virtual void BeforeHighLevelSystemsShutdown() override;

  void OnResize(ezUInt32 width, ezUInt32 height);

private:
  ezSharedPtr<Instance> instance;
  ezSharedPtr<Adapter> adapter;
  ezSharedPtr<Device> device;
  ezRHISampleWindow* m_pWindow = nullptr;
  ezSharedPtr<Swapchain> swapchain = nullptr;
  ezSharedPtr<Fence> fence = nullptr;
  uint64_t fence_value = 0;
  ezSharedPtr<CommandQueue> command_queue = nullptr;
  std::array<uint64_t, frame_count> fence_values = {};
  //std::vector<std::shared_ptr<CommandList>> command_lists;
  //std::vector<std::shared_ptr<Framebuffer>> framebuffers;
  ezSharedPtr<Pipeline> pipeline;
  ezSharedPtr<RenderPass> render_pass;
  ezSharedPtr<Program> program;
  ezSharedPtr<View> constant_view;
  ezSharedPtr<Resource> constant_buffer;
  ezSharedPtr<Resource> index_buffer;
  ezSharedPtr<Resource> vertex_buffer;
  ezSharedPtr<Shader> vertex_shader;
  ezSharedPtr<Shader> pixel_shader;
  ezSharedPtr<BindingSetLayout> layout;
  ezSharedPtr<BindingSet> binding_set;
  ezUInt32 frame_index = 0;
  RenderDeviceDesc renderDeviceDesc;
};
