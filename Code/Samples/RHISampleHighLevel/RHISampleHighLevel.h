#pragma once

#include <memory>
#include <Foundation/Application/Application.h>
#include <Foundation/Types/UniquePtr.h>

#include <RHI/HighLevelRenderer/RenderDevice.h>
#include <RHI/Swapchain/Swapchain.h>
#include <RHI/Fence/Fence.h>
#include <RHI/Instance/Instance.h>
#include <RHI/HighLevelRenderer/ProgramHolder.h>

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
  ezRHISampleWindow* m_pWindow = nullptr;
  RenderDeviceDesc renderDeviceDesc;
  std::shared_ptr<RenderDevice> device;
  std::shared_ptr<RenderCommandList> upload_command_list;
  std::vector<std::shared_ptr<RenderCommandList>> command_lists;
};
