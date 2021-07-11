#include <Core/Input/InputManager.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/Time/Clock.h>
#include <RHISampleLowLevel/RHISampleLowLevel.h>

#include <Core/System/Window.h>

#include <RHIShaderCompilerHLSL/Compiler.h>
#include <RHIShaderCompilerHLSL/ShaderReflection.h>

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

std::shared_ptr<Shader> CreateShader()
{
  return nullptr;
}

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
    if (ezStringUtils::Compare(szRendererName, "dx") == 0)
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

  renderDeviceDesc.api_type = apiType;
  renderDeviceDesc.frame_count = frame_count;
  renderDeviceDesc.round_fps = true;
  renderDeviceDesc.vsync = true;

  instance = InstanceFactory::CreateInstance(renderDeviceDesc.api_type);
  adapter = std::move(instance->EnumerateAdapters()[renderDeviceDesc.required_gpu_index]);
  device = adapter->CreateDevice();

  // now that we have a window and device, tell the engine to initialize the rendering infrastructure
  ezStartup::StartupHighLevelSystems();

  command_queue = device->GetCommandQueue(CommandListType::kGraphics);
  swapchain = device->CreateSwapchain(m_pWindow->GetNativeWindowHandle(),
    m_pWindow->GetClientAreaSize().width,
    m_pWindow->GetClientAreaSize().height,
    frame_count, renderDeviceDesc.vsync);
  fence = device->CreateFence(fence_value);

  std::vector<uint32_t> index_data = {0, 1, 2};
  index_buffer = device->CreateBuffer(BindFlag::kIndexBuffer | BindFlag::kCopyDest, (ezUInt32)(sizeof(uint32_t) * index_data.size()));
  index_buffer->CommitMemory(MemoryType::kUpload);
  index_buffer->UpdateUploadBuffer(0, index_data.data(), sizeof(index_data.front()) * index_data.size());

  std::vector<ezVec3> vertex_data = {ezVec3(-0.5, -0.5, 0.0), ezVec3(0.0, 0.5, 0.0), ezVec3(0.5, -0.5, 0.0)};
  vertex_buffer = device->CreateBuffer(BindFlag::kVertexBuffer | BindFlag::kCopyDest, (ezUInt32)(sizeof(vertex_data.front()) * vertex_data.size()));
  vertex_buffer->CommitMemory(MemoryType::kUpload);
  vertex_buffer->UpdateUploadBuffer(0, vertex_data.data(), sizeof(vertex_data.front()) * vertex_data.size());

  ezVec4 constant_data = ezVec4(1, 0, 0, 1);
  constant_buffer = device->CreateBuffer(BindFlag::kConstantBuffer | BindFlag::kCopyDest, sizeof(constant_data));
  constant_buffer->CommitMemory(MemoryType::kUpload);
  constant_buffer->UpdateUploadBuffer(0, &constant_data, sizeof(constant_data));

  ezStringBuilder projectDirAbsolutePath;
  if (!ezFileSystem::ResolveSpecialDirectory(">project", projectDirAbsolutePath).Succeeded())
  {
    EZ_REPORT_FAILURE("Project directory could not be resolved.");
  }

  ezStringBuilder vsShaderPath(projectDirAbsolutePath, "/shaders/Triangle/VertexShader.hlsl");
  vsShaderPath.MakeCleanPath();
  ShaderDesc vsDesc{vsShaderPath.GetData(), "main", ShaderType::kVertex, "6_0"};
  auto vsBlob = Compile(vsDesc, shaderBlobType);
  auto vsReflection = CreateShaderReflection(shaderBlobType, vsBlob.data(), vsBlob.size());
  vertex_shader = device->CreateShader(vsDesc, vsBlob, vsReflection);


  ezStringBuilder psShaderPath(projectDirAbsolutePath, "/shaders/Triangle/PixelShader.hlsl");
  psShaderPath.MakeCleanPath();
  ShaderDesc psDesc{psShaderPath.GetData(), "main", ShaderType::kPixel, "6_0"};
  auto psBlob = Compile(psDesc, shaderBlobType);
  auto psReflection = CreateShaderReflection(shaderBlobType, psBlob.data(), psBlob.size());
  pixel_shader = device->CreateShader(psDesc, psBlob, psReflection);

  program = device->CreateProgram({vertex_shader, pixel_shader});

  ViewDesc constant_view_desc = {};
  constant_view_desc.view_type = ViewType::kConstantBuffer;
  constant_view_desc.dimension = ViewDimension::kBuffer;
  constant_view = device->CreateView(constant_buffer, constant_view_desc);

  BindKey settings_key = {ShaderType::kPixel, ViewType::kConstantBuffer, 0, 0, 1};
  layout = device->CreateBindingSetLayout({settings_key});
  binding_set = device->CreateBindingSet(layout);
  binding_set->WriteBindings({{settings_key, constant_view}});

  RenderPassDesc render_pass_desc = {
    {{swapchain->GetFormat(), RenderPassLoadOp::kClear, RenderPassStoreOp::kStore}},
  };
  render_pass = device->CreateRenderPass(render_pass_desc);


  GraphicsPipelineDesc pipeline_desc = {
    program,
    layout,
    {{0, "POSITION", ezRHIResourceFormat::R32G32B32_FLOAT, sizeof(vertex_data.front())}},
    render_pass};
  pipeline = device->CreateGraphicsPipeline(pipeline_desc);

  //for (ezUInt32 i = 0; i < frame_count; ++i)
  //{
  //  ViewDesc back_buffer_view_desc = {};
  //  back_buffer_view_desc.view_type = ViewType::kRenderTarget;
  //  back_buffer_view_desc.dimension = ViewDimension::kTexture2D;
  //  std::shared_ptr<Resource> back_buffer = swapchain->GetBackBuffer(i);
  //  std::shared_ptr<View> back_buffer_view = device->CreateView(back_buffer, back_buffer_view_desc);
  //  FramebufferDesc framebuffer_desc = {};
  //  framebuffer_desc.render_pass = render_pass;
  //  framebuffer_desc.width = m_pWindow->GetClientAreaSize().width;
  //  framebuffer_desc.height = m_pWindow->GetClientAreaSize().height;
  //  framebuffer_desc.colors = {back_buffer_view};
  //  std::shared_ptr<Framebuffer> framebuffer = framebuffers.emplace_back(device->CreateFramebuffer(framebuffer_desc));
  //  std::shared_ptr<CommandList> command_list = command_lists.emplace_back(device->CreateCommandList(CommandListType::kGraphics));
  //  command_list->BindPipeline(pipeline);
  //  command_list->BindBindingSet(binding_set);
  //  command_list->SetViewport(0, 0, (float)m_pWindow->GetClientAreaSize().width, (float)m_pWindow->GetClientAreaSize().height);
  //  command_list->SetScissorRect(0, 0, m_pWindow->GetClientAreaSize().width, m_pWindow->GetClientAreaSize().height);
  //  command_list->IASetIndexBuffer(index_buffer, ezRHIResourceFormat::R32_UINT);
  //  command_list->IASetVertexBuffer(0, vertex_buffer);
  //  command_list->ResourceBarrier({{back_buffer, ResourceState::kPresent, ResourceState::kRenderTarget}});
  //  command_list->BeginRenderPass(render_pass, framebuffer, clear_desc);
  //  command_list->DrawIndexed(3, 1, 0, 0, 0);
  //  command_list->EndRenderPass();
  //  command_list->ResourceBarrier({{back_buffer, ResourceState::kRenderTarget, ResourceState::kPresent}});
  //  command_list->Close();
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
  //command_queue->Signal(fence, ++fence_value);
  //fence->Wait(fence_value);

  swapchain.reset();
  swapchain = device->CreateSwapchain(m_pWindow->GetNativeWindowHandle(), width, height, frame_count, renderDeviceDesc.vsync);
  frame_index = 0;
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
    frame_index = swapchain->NextImage(fence, ++fence_value);

    ClearDesc clear_desc = {{{0.0, 0.2, 0.4, 1.0}}};

    ViewDesc back_buffer_view_desc = {};
    back_buffer_view_desc.view_type = ViewType::kRenderTarget;
    back_buffer_view_desc.dimension = ViewDimension::kTexture2D;
    std::shared_ptr<Resource> back_buffer = swapchain->GetBackBuffer(frame_index);
    std::shared_ptr<View> back_buffer_view = device->CreateView(back_buffer, back_buffer_view_desc);
    FramebufferDesc framebuffer_desc = {};
    framebuffer_desc.render_pass = render_pass;
    framebuffer_desc.width = m_pWindow->GetClientAreaSize().width;
    framebuffer_desc.height = m_pWindow->GetClientAreaSize().height;
    framebuffer_desc.colors = {back_buffer_view};
    std::shared_ptr<Framebuffer> framebuffer = device->CreateFramebuffer(framebuffer_desc);
    std::shared_ptr<CommandList> command_list = device->CreateCommandList(CommandListType::kGraphics);
    command_list->BindPipeline(pipeline);
    command_list->BindBindingSet(binding_set);
    command_list->SetViewport(0, 0, (float)m_pWindow->GetClientAreaSize().width, (float)m_pWindow->GetClientAreaSize().height);
    command_list->SetScissorRect(0, 0, m_pWindow->GetClientAreaSize().width, m_pWindow->GetClientAreaSize().height);
    command_list->IASetIndexBuffer(index_buffer, ezRHIResourceFormat::R32_UINT);
    command_list->IASetVertexBuffer(0, vertex_buffer);
    command_list->ResourceBarrier({{back_buffer, ResourceState::kPresent, ResourceState::kRenderTarget}});
    command_list->BeginRenderPass(render_pass, framebuffer, clear_desc);
    command_list->DrawIndexed(3, 1, 0, 0, 0);
    command_list->EndRenderPass();
    command_list->ResourceBarrier({{back_buffer, ResourceState::kRenderTarget, ResourceState::kPresent}});
    command_list->Close();



    command_queue->Wait(fence, fence_value);
    fence->Wait(fence_values[frame_index]);
    command_queue->ExecuteCommandLists({command_list});
    command_queue->Signal(fence, fence_values[frame_index] = ++fence_value);
    swapchain->Present(fence, fence_values[frame_index]);


    command_queue->Signal(fence, ++fence_value);
    fence->Wait(fence_value);
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
