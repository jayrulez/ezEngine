#include <Core/Input/InputManager.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/Time/Clock.h>
#include <RHISampleHighLevel/RHISampleHighLevel.h>
#include <Foundation/Containers/ArrayMap.h>

#include <Core/System/Window.h>


static ezUInt32 g_uiWindowWidth = 640;
static ezUInt32 g_uiWindowHeight = 480;



class ezRHISampleWindow : public ezWindow
{
public:
  ezRHISampleWindow(ezRHISampleApp* pApp)
    : ezWindow()
  {
    m_pApp = pApp;
    m_bCloseRequested = false;
  }

  virtual void OnClickClose() override { m_bCloseRequested = true; }

  virtual void OnResize(const ezSizeU32& newWindowSize) override
  {
    if (m_pApp)
    {
      m_CreationDescription.m_Resolution = newWindowSize;
      m_pApp->OnResize(m_CreationDescription.m_Resolution.width, m_CreationDescription.m_Resolution.height);
    }
  }


  bool m_bCloseRequested;

private:
  ezRHISampleApp* m_pApp = nullptr;
};

ezRHISampleApp::ezRHISampleApp()
  : ezApplication("RHI Sample")
{
}

void ezRHISampleApp::AfterCoreSystemsStartup()
{
  ezStringBuilder sProjectDir = ">sdk/Data/Samples/RHISample";
  ezStringBuilder sProjectDirResolved;
  ezFileSystem::ResolveSpecialDirectory(sProjectDir, sProjectDirResolved).IgnoreResult();

  ezFileSystem::SetSpecialDirectory("project", sProjectDirResolved);

  ezFileSystem::AddDataDirectory("", "", ":", ezFileSystem::AllowWrites).IgnoreResult();
  ezFileSystem::AddDataDirectory(">appdir/", "AppBin", "bin", ezFileSystem::AllowWrites).IgnoreResult();                              // writing to the binary directory
  ezFileSystem::AddDataDirectory(">appdir/", "ShaderCache", "shadercache", ezFileSystem::AllowWrites).IgnoreResult();                 // for shader files
  ezFileSystem::AddDataDirectory(">user/ezEngine Project/RHISample", "AppData", "appdata", ezFileSystem::AllowWrites).IgnoreResult(); // app user data

  ezFileSystem::AddDataDirectory(">sdk/Data/Base", "Base", "base").IgnoreResult();
  ezFileSystem::AddDataDirectory(">project/", "Project", "project", ezFileSystem::AllowWrites).IgnoreResult();

  ezGlobalLog::AddLogWriter(ezLogWriter::Console::LogMessageHandler);
  ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);

  ezPlugin::LoadPlugin("ezInspectorPlugin").IgnoreResult();

  // Register Input
  {
    ezInputActionConfig cfg;

    cfg = ezInputManager::GetInputActionConfig("Main", "CloseApp");
    cfg.m_sInputSlotTrigger[0] = ezInputSlot_KeyEscape;
    ezInputManager::SetInputActionConfig("Main", "CloseApp", cfg, true);
  }

  ShaderBlobType shaderBlobType = ShaderBlobType::kSPIRV;
  ApiType apiType = ApiType::kVulkan;

  const char* szRendererName = ezCommandLineUtils::GetGlobalInstance()->GetStringOption("-rhi", 0, "dx");
  {
    if (ezStringUtils::Compare(szRendererName,"dx") == 0)
    {
      shaderBlobType = ShaderBlobType::kDXIL;
      apiType = ApiType::kDX12;
    }

    if (ezStringUtils::Compare(szRendererName, "vk") == 0)
    {
      shaderBlobType = ShaderBlobType::kSPIRV;
      apiType = ApiType::kVulkan;
    }
  }

  // Create a window for rendering
  {
    ezWindowCreationDesc WindowCreationDesc;
    WindowCreationDesc.m_Resolution.width = g_uiWindowWidth;
    WindowCreationDesc.m_Resolution.height = g_uiWindowHeight;
    WindowCreationDesc.m_Title = ezStringBuilder("RHISample ", szRendererName);
    WindowCreationDesc.m_bShowMouseCursor = true;
    WindowCreationDesc.m_bClipMouseCursor = false;
    WindowCreationDesc.m_WindowMode = ezWindowMode::WindowResizable;
    m_pWindow = EZ_DEFAULT_NEW(ezRHISampleWindow, this);
    m_pWindow->Initialize(WindowCreationDesc).IgnoreResult();
  }

  // now that we have a window and device, tell the engine to initialize the rendering infrastructure
  ezStartup::StartupHighLevelSystems();

  renderDeviceDesc.api_type = apiType;
  renderDeviceDesc.frame_count = frame_count;
  renderDeviceDesc.round_fps = true;
  renderDeviceDesc.vsync = false;

  device = CreateRenderDevice(renderDeviceDesc, m_pWindow);


  std::vector<uint32_t> ibuf = {0, 1, 2};
  index = device->CreateBuffer(BindFlag::kIndexBuffer | BindFlag::kCopyDest, (ezUInt32)(sizeof(uint32_t) * ibuf.size()));
  std::vector<ezVec3> pbuf = {
    ezVec3(-0.5, -0.5, 0.0),
    ezVec3(0.0, 0.5, 0.0),
    ezVec3(0.5, -0.5, 0.0)};
  pos = device->CreateBuffer(BindFlag::kVertexBuffer | BindFlag::kCopyDest, (ezUInt32)(sizeof(ezVec3) * pbuf.size()));

  upload_command_list = device->CreateRenderCommandList();
  upload_command_list->UpdateSubresource(index, 0, ibuf.data(), 0, 0);
  upload_command_list->UpdateSubresource(pos, 0, pbuf.data(), 0, 0);
  upload_command_list->Close();
  device->ExecuteCommandLists({upload_command_list});

  m_program = std::make_shared<ProgramHolder<PixelShader, VertexShader>>(*device, shaderBlobType);
  m_program->ps.cbuffer.Settings.color = ezColor(1, 0, 0, 1);

  //for (uint32_t i = 0; i < renderDeviceDesc.frame_count; ++i)
  //{
  //  RenderPassBeginDesc render_pass_desc = {};
  //  render_pass_desc.colors[m_program->ps.om.rtv0].texture = device->GetBackBuffer(i);
  //  render_pass_desc.colors[m_program->ps.om.rtv0].clear_color = {0.0f, 0.2f, 0.4f, 1.0f};

  //  decltype(auto) command_list = device->CreateRenderCommandList();
  //  command_list->UseProgram(*m_program);
  //  command_list->Attach(m_program->ps.cbv.Settings, m_program->ps.cbuffer.Settings);
  //  command_list->SetViewport(0, 0, (float)m_pWindow->GetClientAreaSize().width, (float)m_pWindow->GetClientAreaSize().height);
  //  command_list->IASetIndexBuffer(index, ezRHIResourceFormat::R32_UINT);
  //  command_list->IASetVertexBuffer(m_program->vs.ia.POSITION, pos);
  //  command_list->BeginRenderPass(render_pass_desc);
  //  command_list->DrawIndexed(3, 1, 0, 0, 0);
  //  command_list->EndRenderPass();
  //  command_list->Close();
  //  command_lists.emplace_back(command_list);
  //}
}

void ezRHISampleApp::BeforeHighLevelSystemsShutdown()
{
  // tell the engine that we are about to destroy window and graphics device,
  // and that it therefore needs to cleanup anything that depends on that
  ezStartup::ShutdownHighLevelSystems();

  // destroy device

  // finally destroy the window
  m_pWindow->Destroy().IgnoreResult();
  EZ_DEFAULT_DELETE(m_pWindow);
}

void ezRHISampleApp::OnResize(ezUInt32 width, ezUInt32 height)
{
  device->Resize(width, height);
}

ezApplication::Execution ezRHISampleApp::Run()
{
  m_pWindow->ProcessWindowMessages();

  if (m_pWindow->m_bCloseRequested || ezInputManager::GetInputActionState("Main", "CloseApp") == ezKeyState::Pressed)
    return Execution::Quit;

  // make sure time goes on
  ezClock::GetGlobalClock()->Update();

  // update all input state
  ezInputManager::Update(ezClock::GetGlobalClock()->GetTimeDiff());


  // do the rendering
  {
    auto frame_index = device->GetFrameIndex();
    RenderPassBeginDesc render_pass_desc = {};
    render_pass_desc.colors[m_program->ps.om.rtv0].texture = device->GetBackBuffer(frame_index);
    render_pass_desc.colors[m_program->ps.om.rtv0].clear_color = {0.0f, 0.2f, 0.4f, 1.0f};

    decltype(auto) command_list = device->CreateRenderCommandList();
    command_list->UseProgram(*m_program);
    command_list->Attach(m_program->ps.cbv.Settings, m_program->ps.cbuffer.Settings);
    command_list->SetViewport(0, 0, (float)m_pWindow->GetClientAreaSize().width, (float)m_pWindow->GetClientAreaSize().height);
    command_list->IASetIndexBuffer(index, ezRHIResourceFormat::R32_UINT);
    command_list->IASetVertexBuffer(m_program->vs.ia.POSITION, pos);
    command_list->BeginRenderPass(render_pass_desc);
    command_list->DrawIndexed(3, 1, 0, 0, 0);
    command_list->EndRenderPass();
    command_list->Close();


    device->ExecuteCommandLists({command_list});
    device->Present();
    device->WaitForIdle();
  }

  // needs to be called once per frame
  ezResourceManager::PerFrameUpdate();

  // tell the task system to finish its work for this frame
  // this has to be done at the very end, so that the task system will only use up the time that is left in this frame for
  // uploading GPU data etc.
  ezTaskSystem::FinishFrameTasks();

  return ezApplication::Execution::Continue;
}

EZ_CONSOLEAPP_ENTRY_POINT(ezRHISampleApp);
