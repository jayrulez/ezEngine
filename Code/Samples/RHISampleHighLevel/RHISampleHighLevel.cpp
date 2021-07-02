#include <Core/Input/InputManager.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/Time/Clock.h>
#include <RHISampleHighLevel/RHISampleHighLevel.h>

#include <Core/System/Window.h>

#include <RHIShaderCompilerHLSL/Compiler.h>
#include <RHIShaderCompilerHLSL/ShaderReflection.h>

static ezUInt32 g_uiWindowWidth = 640;
static ezUInt32 g_uiWindowHeight = 480;

std::string GetShaderPath(const std::string& shaderFile)
{
  ezStringBuilder projectDirAbsolutePath;
  if (!ezFileSystem::ResolveSpecialDirectory(">project", projectDirAbsolutePath).Succeeded())
  {
    EZ_REPORT_FAILURE("Project directory could not be resolved.");
    return {};
  }

  ezStringBuilder shaderPath(projectDirAbsolutePath, shaderFile.data());
  shaderPath.MakeCleanPath();

  return shaderPath.GetData();
}

struct VertexShaderDesc
{
  static constexpr ShaderType type = ShaderType::kVertex;
  ShaderDesc desc = {GetShaderPath("/shaders/TriangleVertexShader.hlsl"), "main", type, "6_0"};

  struct IA
  {
    static constexpr const uint32_t POSITION = 0;
  } ia;
};

class VertexShader : public VertexShaderDesc
{
public:
  VertexShader(RenderDevice& device, ShaderBlobType shaderBlobType)
    : m_ShaderBlobType{shaderBlobType}
  {
  }

  void CompileShader(RenderDevice& device)
  {
    auto full_desc = desc;
    std::vector<uint8_t> byteCode = Compile(full_desc, m_ShaderBlobType);
    std::shared_ptr<ShaderReflection> reflection = CreateShaderReflection(m_ShaderBlobType, byteCode.data(), byteCode.size());
    shader = device.CreateShader(full_desc, byteCode, reflection);
  }

  std::shared_ptr<Shader> shader;

private:
  ShaderBlobType m_ShaderBlobType;
};

struct PixelShaderDesc
{
  static constexpr ShaderType type = ShaderType::kPixel;
  ShaderDesc desc = {GetShaderPath("/shaders/TrianglePixelShader.hlsl"), "main", type, "6_0"};

  struct CBV
  {
    BindKey Settings;
  } cbv;

  struct OM
  {
    static constexpr const uint32_t rtv0 = 0;
  } om;
};

class PixelShader : public PixelShaderDesc
{
public:
  struct Settings
  {
    ezColor color;
  };
  BufferLayout Settings_layout = { 16, { 16, }, { offsetof(Settings, color), }, { 0, } };
  struct Cbuffer
  {
    Cbuffer(PixelShader& shader, RenderDevice& device)
      : Settings(device, shader.Settings_layout)
    {
    }
    ConstantBuffer<Settings> Settings;
  } cbuffer;

  PixelShader(RenderDevice& device, ShaderBlobType shaderBlobType)
    : cbuffer(*this, device)
    , m_ShaderBlobType{shaderBlobType}
  {
  }

  void CompileShader(RenderDevice& device)
  {
    auto full_desc = desc;
    std::vector<uint8_t> byteCode = Compile(full_desc, m_ShaderBlobType);
    std::shared_ptr<ShaderReflection> reflection = CreateShaderReflection(m_ShaderBlobType, byteCode.data(), byteCode.size());
    shader = device.CreateShader(full_desc, byteCode, reflection);
    cbv.Settings = shader->GetBindKey("Settings");
  }

  std::shared_ptr<Shader> shader;

private:
  ShaderBlobType m_ShaderBlobType;
};


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
      m_pApp->OnResize(newWindowSize.width, newWindowSize.height);
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

  // Create a window for rendering
  {
    ezWindowCreationDesc WindowCreationDesc;
    WindowCreationDesc.m_Resolution.width = g_uiWindowWidth;
    WindowCreationDesc.m_Resolution.height = g_uiWindowHeight;
    WindowCreationDesc.m_Title = "RHISample";
    WindowCreationDesc.m_bShowMouseCursor = true;
    WindowCreationDesc.m_bClipMouseCursor = false;
    WindowCreationDesc.m_WindowMode = ezWindowMode::WindowResizable;
    m_pWindow = EZ_DEFAULT_NEW(ezRHISampleWindow, this);
    m_pWindow->Initialize(WindowCreationDesc).IgnoreResult();
  }

  // now that we have a window and device, tell the engine to initialize the rendering infrastructure
  ezStartup::StartupHighLevelSystems();

  ShaderBlobType shaderBlobType = ShaderBlobType::kDXIL;
  ApiType apiType = ApiType::kDX12;

  renderDeviceDesc.api_type = apiType;
  renderDeviceDesc.frame_count = frame_count;
  renderDeviceDesc.round_fps = true;
  renderDeviceDesc.vsync = true;

  device = CreateRenderDevice(renderDeviceDesc, m_pWindow);

  upload_command_list = device->CreateRenderCommandList();
  std::vector<uint32_t> ibuf = {0, 1, 2};
  std::shared_ptr<Resource> index = device->CreateBuffer(BindFlag::kIndexBuffer | BindFlag::kCopyDest, (ezUInt32)(sizeof(uint32_t) * ibuf.size()));
  upload_command_list->UpdateSubresource(index, 0, ibuf.data(), 0, 0);
  std::vector<ezVec3> pbuf = {
    ezVec3(-0.5, -0.5, 0.0),
    ezVec3(0.0, 0.5, 0.0),
    ezVec3(0.5, -0.5, 0.0)};
  std::shared_ptr<Resource> pos = device->CreateBuffer(BindFlag::kVertexBuffer | BindFlag::kCopyDest, (ezUInt32)(sizeof(ezVec3) * pbuf.size()));
  upload_command_list->UpdateSubresource(pos, 0, pbuf.data(), 0, 0);
  upload_command_list->Close();
  device->ExecuteCommandLists({upload_command_list});

  ProgramHolder<PixelShader, VertexShader> program(*device, shaderBlobType);
  program.ps.cbuffer.Settings.color = ezColor(1, 0, 0, 1);

  for (uint32_t i = 0; i < renderDeviceDesc.frame_count; ++i)
  {
    RenderPassBeginDesc render_pass_desc = {};
    render_pass_desc.colors[program.ps.om.rtv0].texture = device->GetBackBuffer(i);
    render_pass_desc.colors[program.ps.om.rtv0].clear_color = {0.0f, 0.2f, 0.4f, 1.0f};

    decltype(auto) command_list = device->CreateRenderCommandList();
    command_list->UseProgram(program);
    command_list->Attach(program.ps.cbv.Settings, program.ps.cbuffer.Settings);
    command_list->SetViewport(0, 0, (float)m_pWindow->GetClientAreaSize().width, (float)m_pWindow->GetClientAreaSize().height);
    command_list->IASetIndexBuffer(index, ezRHIResourceFormat::R32_UINT);
    command_list->IASetVertexBuffer(program.vs.ia.POSITION, pos);
    command_list->BeginRenderPass(render_pass_desc);
    command_list->DrawIndexed(3, 1, 0, 0, 0);
    command_list->EndRenderPass();
    command_list->Close();
    command_lists.emplace_back(command_list);
  }
}

void ezRHISampleApp::BeforeHighLevelSystemsShutdown()
{
  device->WaitForIdle();

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
    device->ExecuteCommandLists({command_lists[device->GetFrameIndex()]});
    device->Present();
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
