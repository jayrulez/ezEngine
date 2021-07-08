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
#include <RHI/Instance/InstanceFactory.h>
#include <RHI/Adapter/Adapter.h>
#include <RHI/Resource/Resource.h>
#include <RHI/BindingSet/BindingSet.h>
#include <RHI/CommandList/CommandList.h>
#include <RHI/CommandQueue/CommandQueue.h>
#include <RHI/View/View.h>
#include <RHI/Framebuffer/Framebuffer.h>
#include <RHI/Shader/ShaderBase.h>
#include <RHI/Program/ProgramBase.h>
#include <RHI/BindingSetLayout/BindingSetLayout.h>

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

  
  ezRHIShaderBlobType shaderBlobType = ezRHIShaderBlobType::SPIRV;
  ezRHIApiType apiType = ezRHIApiType::Vulkan;

  const char* szRendererName = ezCommandLineUtils::GetGlobalInstance()->GetStringOption("-rhi", 0, "vk");
  {
    if (ezStringUtils::Compare(szRendererName, "dx") == 0)
    {
      shaderBlobType = ezRHIShaderBlobType::DXIL;
      apiType = ezRHIApiType::DX12;
    }

    if (ezStringUtils::Compare(szRendererName, "vk") == 0)
    {
      shaderBlobType = ezRHIShaderBlobType::SPIRV;
      apiType = ezRHIApiType::Vulkan;
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

  instance = ezRHIInstanceFactory::CreateInstance(renderDeviceDesc.api_type);
  adapter = std::move(instance->EnumerateAdapters()[renderDeviceDesc.required_gpu_index]);
  device = adapter->CreateDevice();

  // now that we have a window and device, tell the engine to initialize the rendering infrastructure
  ezStartup::StartupHighLevelSystems();

  command_queue = device->GetCommandQueue(ezRHICommandListType::Graphics);

  ezRHISwapChainCreationDescription swapChainDesc;
  swapChainDesc.m_Window = m_pWindow;
  swapChainDesc.m_FrameCount = renderDeviceDesc.frame_count;
  swapChainDesc.m_Vsync = false;


  swapchain = device->CreateSwapchain(swapChainDesc);
  fence = device->CreateFence(fence_value);

  ezUInt32 index_data[] = {0, 1, 2};

  ezRHIBufferCreationDescription indexbufferDesc{(ezRHIBindFlag::IndexBuffer | ezRHIBindFlag::CopyDest), EZ_ARRAY_SIZE(index_data) * (ezUInt32)sizeof(index_data[0])};

  index_buffer = device->CreateBuffer(indexbufferDesc);
  index_buffer->CommitMemory(ezRHIMemoryType::Upload);
  index_buffer->UpdateUploadBuffer(0, &index_data, indexbufferDesc.m_BufferSize);

  ezDynamicArray<ezVec3> vertex_data;
  vertex_data.PushBack(ezVec3(-0.5, -0.5, 0.0));
  vertex_data.PushBack(ezVec3(0.0, 0.5, 0.0));
  vertex_data.PushBack(ezVec3(0.5, -0.5, 0.0));

  ezRHIBufferCreationDescription vertexbufferDesc;
  vertexbufferDesc.m_BindFlag = (ezRHIBindFlag::VertexBuffer | ezRHIBindFlag::CopyDest);
  vertexbufferDesc.m_BufferSize = vertex_data.GetCount() * (ezUInt32)sizeof(ezVec3);

  vertex_buffer = device->CreateBuffer(vertexbufferDesc);
  vertex_buffer->CommitMemory(ezRHIMemoryType::Upload);
  vertex_buffer->UpdateUploadBuffer(0, &vertex_data, vertexbufferDesc.m_BufferSize);

  ezVec4 constant_data = ezVec4(1, 0, 0, 1);
  ezRHIBufferCreationDescription constantBufferDesc{
    (ezRHIBindFlag::ConstantBuffer | ezRHIBindFlag::CopyDest),
    sizeof(constant_data)};

  constant_buffer = device->CreateBuffer(constantBufferDesc);
  constant_buffer->CommitMemory(ezRHIMemoryType::Upload);
  constant_buffer->UpdateUploadBuffer(0, &constant_data, sizeof(constant_data));

  ezStringBuilder projectDirAbsolutePath;
  if (!ezFileSystem::ResolveSpecialDirectory(">project", projectDirAbsolutePath).Succeeded())
  {
    EZ_REPORT_FAILURE("Project directory could not be resolved.");
  }

  ezStringBuilder vsShaderPath(projectDirAbsolutePath, "/shaders/TriangleVertexShader.hlsl");
  vsShaderPath.MakeCleanPath();
  ezRHIShaderCreationDescription vsDesc{"main", ezRHIShaderType::Vertex, "6_0"};
  vsDesc.m_Blob = Compile(vsShaderPath, vsDesc, shaderBlobType);
  vsDesc.m_Reflection = CreateShaderReflection(shaderBlobType, vsDesc.m_Blob.GetData(), vsDesc.m_Blob.GetCount());
  vertex_shader = device->CreateShader(vsDesc);


  ezStringBuilder psShaderPath(projectDirAbsolutePath, "/shaders/TrianglePixelShader.hlsl");
  psShaderPath.MakeCleanPath();
  ezRHIShaderCreationDescription psDesc{"main", ezRHIShaderType::Pixel, "6_0"};
  psDesc.m_Blob = Compile(psShaderPath, psDesc, shaderBlobType);
  psDesc.m_Reflection = CreateShaderReflection(shaderBlobType, psDesc.m_Blob.GetData(), psDesc.m_Blob.GetCount());
  pixel_shader = device->CreateShader(psDesc);

  ezDynamicArray<ezRHIShader*> shaders;
  shaders.PushBack(vertex_shader);
  shaders.PushBack(pixel_shader);
  program = device->CreateProgram(shaders);

  ezRHIViewDesc constant_view_desc = {};
  constant_view_desc.m_ViewType = ezRHIViewType::ConstantBuffer;
  constant_view_desc.m_Dimension = ezRHIViewDimension::Buffer;
  constant_view = device->CreateView(constant_buffer, constant_view_desc);

  ezDynamicArray<ezRHIBindKeyDescription> settings_keys;
  ezRHIBindKeyDescription settings_key{ezRHIShaderType::Pixel, ezRHIViewType::ConstantBuffer, 0, 0, 1};
  settings_keys.PushBack(settings_key);

  layout = device->CreateBindingSetLayout(settings_keys);
  binding_set = device->CreateBindingSet(layout);

  ezDynamicArray<ezRHIBindingDescription> bindingDescriptions;
  bindingDescriptions.PushBack({settings_key, constant_view});

  binding_set->WriteBindings(bindingDescriptions);

  ezRHIRenderPassCreationDescription render_pass_desc;
  render_pass_desc.m_Colors.PushBack({swapchain->GetFormat(), ezRHIRenderPassLoadOp::Clear, ezRHIRenderPassStoreOp::Store});
  render_pass = device->CreateRenderPass(render_pass_desc);

  ezDynamicArray<ezRHIInputLayoutDescription> inputs;
  inputs.PushBack({0, "POSITION", ezRHIResourceFormat::R32G32B32_FLOAT, sizeof(vertex_data[0])});

  ezRHIGraphicsPipelineCreationDescription pipeline_desc = {
    program,
    layout,
    inputs,
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

  EZ_DEFAULT_DELETE(swapchain);


  
  ezRHISwapChainCreationDescription swapChainDesc;
  swapChainDesc.m_Window = m_pWindow; // was this resized? any way, need to use the one from params
  swapChainDesc.m_FrameCount = renderDeviceDesc.frame_count;
  swapChainDesc.m_Vsync = false;

  swapchain = device->CreateSwapchain(swapChainDesc);
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

    ezRHIClearDescription clear_desc = {};
    clear_desc.m_Colors.PushBack({0.0, 0.2, 0.4, 1.0});

    ezRHIViewDesc back_buffer_view_desc = {};
    back_buffer_view_desc.m_ViewType = ezRHIViewType::RenderTarget;
    back_buffer_view_desc.m_Dimension = ezRHIViewDimension::Texture2D;
    ezRHIResource* back_buffer = swapchain->GetBackBuffer(frame_index);
    ezRHIView* back_buffer_view = device->CreateView(back_buffer, back_buffer_view_desc);
    ezRHIFramebufferCreationDescription framebuffer_desc = {};
    framebuffer_desc.m_RenderPass = render_pass;
    framebuffer_desc.m_Width = m_pWindow->GetClientAreaSize().width;
    framebuffer_desc.m_Height = m_pWindow->GetClientAreaSize().height;
    framebuffer_desc.m_Colors.PushBack(back_buffer_view);
    ezRHIFramebuffer* framebuffer = device->CreateFramebuffer(framebuffer_desc);
    ezRHICommandList* command_list = device->CreateCommandList(ezRHICommandListType::Graphics);
    command_list->BindPipeline(pipeline);
    command_list->BindBindingSet(binding_set);
    command_list->SetViewport(0, 0, (float)m_pWindow->GetClientAreaSize().width, (float)m_pWindow->GetClientAreaSize().height);
    command_list->SetScissorRect(0, 0, m_pWindow->GetClientAreaSize().width, m_pWindow->GetClientAreaSize().height);
    command_list->IASetIndexBuffer(index_buffer, ezRHIResourceFormat::R32_UINT);
    command_list->IASetVertexBuffer(0, vertex_buffer);

    ezDynamicArray<ezRHIResourceBarrierDescription> barriers1;
    barriers1.PushBack({back_buffer, ezRHIResourceState::Present, ezRHIResourceState::RenderTarget});
    command_list->ResourceBarrier(barriers1);
    command_list->BeginRenderPass(render_pass, framebuffer, clear_desc);
    command_list->DrawIndexed(3, 1, 0, 0, 0);
    command_list->EndRenderPass();

    ezDynamicArray<ezRHIResourceBarrierDescription> barriers2;
    barriers2.PushBack({back_buffer, ezRHIResourceState::RenderTarget, ezRHIResourceState::Present});
    command_list->ResourceBarrier(barriers2);
    command_list->Close();



    command_queue->Wait(fence, fence_value);
    fence->Wait(fence_values[frame_index]);

    ezDynamicArray<ezRHICommandList*> commandLists;
    commandLists.PushBack(command_list);

    command_queue->ExecuteCommandLists(commandLists);
    command_queue->Signal(fence, fence_values[frame_index] = ++fence_value);
    swapchain->Present(fence, fence_values[frame_index]);


    command_queue->Signal(fence, ++fence_value);
    fence->Wait(fence_value);

    EZ_DEFAULT_DELETE(command_list);
    EZ_DEFAULT_DELETE(framebuffer);
    EZ_DEFAULT_DELETE(back_buffer_view);
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
