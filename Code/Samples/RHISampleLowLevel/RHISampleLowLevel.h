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


struct RenderDeviceDesc
{
  ezRHIApiType api_type = ezRHIApiType::Vulkan;
  bool vsync = true;
  bool round_fps = false;
  uint32_t required_gpu_index = 0;
  uint32_t frame_count = 3;
};

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
  ezRHIInstance* instance;
  ezRHIAdapter* adapter;
  ezRHIDevice* device;
  ezRHISampleWindow* m_pWindow = nullptr;
  ezRHISwapChain* swapchain = nullptr;
  ezRHIFence* fence = nullptr;
  ezUInt64 fence_value = 0;
  ezRHICommandQueue* command_queue = nullptr;
  std::array<uint64_t, frame_count> fence_values = {};
  //std::vector<ezRHICommandList>> command_lists;
  //std::vector<ezRHIFramebuffer>> framebuffers;
  ezRHIPipeline* pipeline;
  ezRHIRenderPass* render_pass;
  ezRHIProgram* program;
  ezRHIView* constant_view;
  ezRHIResource* constant_buffer;
  ezRHIResource* index_buffer;
  ezRHIResource* vertex_buffer;
  ezRHIShader* vertex_shader;
  ezRHIShader* pixel_shader;
  ezRHIBindingSetLayout* layout;
  ezRHIBindingSet* binding_set;
  ezUInt32 frame_index = 0;
  RenderDeviceDesc renderDeviceDesc;
};
