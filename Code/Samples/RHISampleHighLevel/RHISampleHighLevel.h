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
  std::shared_ptr<Instance> instance;
  std::shared_ptr<Adapter> adapter;
  std::shared_ptr<Device> device;
  ezRHISampleWindow* m_pWindow = nullptr;
  std::shared_ptr<Swapchain> swapchain = nullptr;
  std::shared_ptr<Fence> fence = nullptr;
  uint64_t fence_value = 0;
  std::shared_ptr<CommandQueue> command_queue = nullptr;
  std::array<uint64_t, frame_count> fence_values = {};
  std::vector<std::shared_ptr<CommandList>> command_lists;
  std::vector<std::shared_ptr<Framebuffer>> framebuffers;
  std::shared_ptr<Pipeline> pipeline;
  std::shared_ptr<RenderPass> render_pass;
  std::shared_ptr<Program> program;
  std::shared_ptr<View> constant_view;
  std::shared_ptr<Resource> constant_buffer;
  std::shared_ptr<Resource> index_buffer;
  std::shared_ptr<Resource> vertex_buffer;
  std::shared_ptr<Shader> vertex_shader;
  std::shared_ptr<Shader> pixel_shader;
  std::shared_ptr<BindingSetLayout> layout;
  std::shared_ptr<BindingSet> binding_set;
  ezUInt32 frame_index = 0;
  RenderDeviceDesc renderDeviceDesc;
};
