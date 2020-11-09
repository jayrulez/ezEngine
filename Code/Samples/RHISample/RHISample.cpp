#include <Core/Graphics/Camera.h>
#include <Core/Graphics/Geometry.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/Communication/Telemetry.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/Time/Clock.h>
#include <Foundation/Utilities/GraphicsUtils.h>
#include <RHI/Device/GraphicsDevice.h>
#include <RHI/Device/GraphicsDeviceOptions.h>
#include <RHI/GraphicsUtils.h>
#include <RHI/Resources/Buffer.h>
#include <RHI/Resources/ResourceFactory.h>
#include <RHI/Resources/Shader.h>
#include <RHISample/RHISample.h>

struct Vertex
{
  ezVec3 Position;
  ezColor Color;

  Vertex(ezVec3 position, ezColor color)
  {
    Position = position;
    Color = color;
  }
};



static ezUInt32 g_uiWindowWidth = 1280;
static ezUInt32 g_uiWindowHeight = 720;

const ezInt32 g_iMaxHalfExtent = 20;
const bool g_bForceImmediateLoading = false;
const bool g_bPreloadAllTextures = false;
const float g_nearClip = 0.01f;
const float g_farClip = 100.f;
constexpr float g_fov = ezMath::Pi<float>() / 4.f;


EZ_CONSOLEAPP_ENTRY_POINT(RHISample);

void RHISample::AfterCoreSystemsStartup()
{
  ezStringBuilder sProjectDir = ">sdk/Data/Samples/RHISample";
  ezStringBuilder sProjectDirResolved;
  ezFileSystem::ResolveSpecialDirectory(sProjectDir, sProjectDirResolved);

  ezFileSystem::SetSpecialDirectory("project", sProjectDirResolved);

  // setup the 'asset management system'
  {
    // which redirection table to search
    ezDataDirectory::FolderType::s_sRedirectionFile = "AssetCache/LookupTable.ezAsset";
    // which platform assets to use
    ezDataDirectory::FolderType::s_sRedirectionPrefix = "AssetCache/PC/";
  }

  {

    ezFileSystem::AddDataDirectory("", "", ":", ezFileSystem::AllowWrites);
    ezFileSystem::AddDataDirectory(">appdir/", "AppBin", "bin", ezFileSystem::AllowWrites);              // writing to the binary directory
    ezFileSystem::AddDataDirectory(">appdir/", "ShaderCache", "shadercache", ezFileSystem::AllowWrites); // for shader files
    ezFileSystem::AddDataDirectory(">user/ezEngine Project/TextureSample", "AppData", "appdata",
      ezFileSystem::AllowWrites); // app user data

    ezFileSystem::AddDataDirectory(">sdk/Data/Base", "Base", "base");
    ezFileSystem::AddDataDirectory(">sdk/Data/FreeContent", "Shared", "shared");
    ezFileSystem::AddDataDirectory(">project/", "Project", "project", ezFileSystem::AllowWrites);

    ezGlobalLog::AddLogWriter(ezLogWriter::Console::LogMessageHandler);
    ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);
  }

  ezTelemetry::CreateServer();
  ezPlugin::LoadPlugin("ezInspectorPlugin");

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
    WindowCreationDesc.m_WindowMode = ezWindowMode::WindowResizable;
    m_pWindow = EZ_DEFAULT_NEW(RHISampleWindow);
    m_pWindow->Initialize(WindowCreationDesc);
  }

  // Create device and resource factory
  {
    RHIGraphicsDeviceOptions options(true);
    RHISwapchainDescription swapchainDesc(new RHIWin32SwapchainSource(m_pWindow->GetNativeWindowHandle(), nullptr), g_uiWindowWidth, g_uiWindowHeight, RHIPixelFormat::D32_Float_S8_UInt, true);
    m_pDevice = GraphicsUtils::CreateD3D11(options, swapchainDesc);
    m_pWindow->SetGraphicsDevice(m_pDevice);
    ResourceFactory = m_pDevice->GetResourceFactory();
  }

  // now that we have a window and device, tell the engine to initialize the rendering infrastructure
  ezStartup::StartupHighLevelSystems();

  CreatePipelineState();
}

ezApplication::ApplicationExecution RHISample::Run()
{
  m_pWindow->ProcessWindowMessages();

  if (m_pWindow->m_bCloseRequested || ezInputManager::GetInputActionState("Main", "CloseApp") == ezKeyState::Pressed)
    return ApplicationExecution::Quit;

  // make sure time goes on
  ezClock::GetGlobalClock()->Update();

  // make sure telemetry is sent out regularly
  ezTelemetry::PerFrameUpdate();

  // do the rendering
  {
    // Before starting to render in a frame call this function
    //CommandList commandList = m_pDevice->BeginCommandList();
    //m_pDevice->PresentBegin(commandList);

    Update();
    OnDraw();

    //m_pDevice->PresentEnd(commandList);
  }

  // needs to be called once per frame
  ezResourceManager::PerFrameUpdate();

  // tell the task system to finish its work for this frame
  // this has to be done at the very end, so that the task system will only use up the time that is left in this frame for
  // uploading GPU data etc.
  ezTaskSystem::FinishFrameTasks();

  return ezApplication::Continue;
}

void RHISample::CreatePipelineState()
{
  // Cube

  Vertex cubeVertices[] = {
    {ezVec3(-.5f, -.5f, -.5f), ezColor::Red},
    {ezVec3(-.5f, +.5f, -.5f), ezColor::Green},
    {ezVec3(+.5f, +.5f, -.5f), ezColor::Blue},
    {ezVec3(+.5f, -.5f, -.5f), ezColor::Violet},

    {ezVec3(-.5f, -.5f, +.5f), ezColor::Yellow},
    {ezVec3(-.5f, +.5f, +.5f), ezColor::Aqua},
    {ezVec3(+.5f, +.5f, +.5f), ezColor::Fuchsia},
    {ezVec3(+.5f, -.5f, +.5f), ezColor::Brown},
  };

  ezUInt16 cubeIndices[] = {
    2,
    0,
    1,
    2,
    3,
    0,
    4,
    6,
    5,
    4,
    7,
    6,
    0,
    7,
    4,
    0,
    3,
    7,
    1,
    0,
    4,
    1,
    4,
    5,
    1,
    5,
    2,
    5,
    6,
    2,
    3,
    6,
    7,
    3,
    2,
    6,
  };

  // Buffers
  {
    VertexBuffer = ResourceFactory->CreateBuffer(RHIBufferDescription{sizeof(cubeVertices), RHIBufferUsage::VertexBuffer});
    IndexBuffer = ResourceFactory->CreateBuffer(RHIBufferDescription{sizeof(cubeIndices), RHIBufferUsage::IndexBuffer});
    ConstantBuffer = ResourceFactory->CreateBuffer(RHIBufferDescription{sizeof(ezMat4), RHIBufferUsage::UniformBuffer});

    m_pDevice->UpdateBuffer(VertexBuffer, 0, reinterpret_cast<ezUInt8*>(cubeVertices), sizeof(cubeVertices));
    m_pDevice->UpdateBuffer(IndexBuffer, 0, reinterpret_cast<ezUInt8*>(cubeIndices), sizeof(cubeIndices));

    ezDynamicArray<RHIVertexElementDescription> layoutElements;
    layoutElements.PushBack(RHIVertexElementDescription{"Position", RHIVertexElementSemantic::Position, RHIVertexElementFormat::Float3});
    layoutElements.PushBack(RHIVertexElementDescription{"Color", RHIVertexElementSemantic::Color, RHIVertexElementFormat::Float4});
    RHIVertexLayoutDescription vertexLayout(layoutElements);

    // Shaders
    {
      ezFileReader fReader;

      //fReader.Open("vs_no_rot.o");
      fReader.Open("vs.o");
      if (fReader.IsOpen())
      {
        RHIShaderDescription vertexShaderDesc = RHIShaderDescription{
          RHIShaderStages::Vertex,
          ezDynamicArray<ezUInt8>(),
          "VSMain",
          false,
        };

        vertexShaderDesc.ShaderBytes.SetCountUninitialized((ezUInt32)fReader.GetFileSize());
        fReader.ReadBytes(vertexShaderDesc.ShaderBytes.GetData(), vertexShaderDesc.ShaderBytes.GetCount());

        VertexShader = ResourceFactory->CreateShader(vertexShaderDesc);

        fReader.Close();
      }

      fReader.Open("ps.o");
      if (fReader.IsOpen())
      {
        RHIShaderDescription fragmentShaderDesc = RHIShaderDescription{
          RHIShaderStages::Fragment,
          ezDynamicArray<ezUInt8>(),
          "PSMain",
          false,
        };

        fragmentShaderDesc.ShaderBytes.SetCountUninitialized((ezUInt32)fReader.GetFileSize());
        fReader.ReadBytes(fragmentShaderDesc.ShaderBytes.GetData(), fragmentShaderDesc.ShaderBytes.GetCount());

        FragmentShader = ResourceFactory->CreateShader(fragmentShaderDesc);

        fReader.Close();
      }
    }

    RHIGraphicsPipelineDescription pipelineDesc;
    pipelineDesc.BlendState = RHIBlendStateDescription::SingleOverrideBlend();
    pipelineDesc.DepthStencilState = RHIDepthStencilStateDescription::DepthOnlyLessEqual; // RHIDepthStencilStateDescription(true, true, RHIComparisonKind::LessEqual);
    pipelineDesc.RasterizerState = RHIRasterizerStateDescription::Default; //RHIRasterizerStateDescription(RHIFaceCullMode::Back, RHIPolygonFillMode::Solid, RHIFrontFace::Clockwise, true, false);
    pipelineDesc.PrimitiveTopology = RHIPrimitiveTopology::TriangleStrip;


    ezDynamicArray<RHIVertexLayoutDescription> vertexLayouts;
    vertexLayouts.PushBack(vertexLayout);
    ezDynamicArray<RHIShader*> shaders;
    shaders.PushBack(VertexShader);
    shaders.PushBack(FragmentShader);

    //pipelineDesc.ShaderSet = RHIShaderSetDescription(layouts, shaders);
    pipelineDesc.ShaderSet.VertexLayouts = vertexLayouts;
    pipelineDesc.ShaderSet.Shaders = shaders;



    RHIResourceLayoutDescription resourceLayoutDesc;

    RHIResourceLayoutElementDescription projectionElement("Projection", RHIResourceKind::UniformBuffer, RHIShaderStages::Vertex);

    resourceLayoutDesc.Elements.PushBack(projectionElement);


    RHIResourceLayout* resourceLayout = ResourceFactory->CreateResourceLayout(resourceLayoutDesc);

    pipelineDesc.ResourceLayouts.PushBack(resourceLayout);


    ezDynamicArray<RHIResource*> boundResources;

    boundResources.PushBack(ConstantBuffer);


    RHIResourceSetDescription resourceSetDesc = RHIResourceSetDescription(resourceLayout, boundResources);

    ResourceSet = ResourceFactory->CreateResourceSet(resourceSetDesc);

    pipelineDesc.Outputs = m_pDevice->GetSwapchainFramebuffer()->GetOutputDescription();
    Pipeline = ResourceFactory->CreateGraphicsPipeline(pipelineDesc);
    CommandList = ResourceFactory->CreateCommandList();


  }
}

void RHISample::OnDraw()
{
  CommandList->Begin();
  CommandList->UpdateBuffer(ConstantBuffer, 0, reinterpret_cast<ezUInt8*>(&WorldViewProjectionMatrix), sizeof(ezMat4));
  CommandList->SetFramebuffer(m_pDevice->GetSwapchainFramebuffer());
  CommandList->ClearColorTarget(0, ezColor::Black);
  CommandList->ClearDepthStencil(1.f);
  CommandList->SetVertexBuffer(0, VertexBuffer);
  CommandList->SetIndexBuffer(IndexBuffer, RHIIndexFormat::UInt16);
  CommandList->SetPipeline(Pipeline);
  CommandList->SetGraphicsResourceSet(0, ResourceSet);
  CommandList->DrawIndexed(36, 1, 0, 0, 0);
  CommandList->End();

  m_pDevice->SubmitCommands(CommandList);
  m_pDevice->SwapBuffers();
  m_pDevice->WaitForIdle();
}

void RHISample::Update()
{
  static float time = 0.f;
  float aspect = (float)(m_pDevice->GetMainSwapchain()->GetFramebuffer()->GetWidth() / m_pDevice->GetMainSwapchain()->GetFramebuffer()->GetHeight());

  ezMat4 rotY;
  rotY.SetRotationMatrixY(ezAngle::Radian(time * 1.0f));

  ezMat4 rotX;
  rotX.SetRotationMatrixX(ezAngle::Radian(-ezMath::Pi<float>() * 0.4f));

  ezMat4 rotZ;
  rotZ.SetRotationMatrixZ(ezAngle::Radian(ezMath::Pi<float>() * 0.4f));


  ezMat4 transform = rotY * rotZ * rotX;

  ezMat4 view;
  view.SetIdentity();
  view.SetTranslationVector(ezVec3(0.f, -0.f, 5.f));

  ezMat4 proj = ezGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovX(ezAngle::Radian(g_fov), aspect, g_nearClip, g_farClip);

  WorldViewProjectionMatrix = proj * view * transform;
  //WorldViewProjectionMatrix.Transpose();

  time += 0.001f;
}

void RHISample::BeforeCoreSystemsShutdown()
{
  // make sure that no textures are continue to be streamed in while the engine shuts down
  ezResourceManager::EngineAboutToShutdown();

  // now we can destroy the graphics device
  Pipeline->Dispose();
  VertexShader->Dispose();
  FragmentShader->Dispose();
  CommandList->Dispose();
  VertexBuffer->Dispose();
  IndexBuffer->Dispose();
  ConstantBuffer->Dispose();

  m_pDevice->Dispose();
  delete m_pDevice;

  // finally destroy the window
  m_pWindow->Destroy();
  EZ_DEFAULT_DELETE(m_pWindow);
}

void RHISampleWindow::SetGraphicsDevice(RHIGraphicsDevice* pDevice)
{
  m_pDevice = pDevice;
}

void RHISampleWindow::OnResize(const ezSizeU32& newWindowSize)
{
  if (m_pDevice != nullptr)
  {
    //m_pDevice->ResizeMainWindow(newWindowSize.width, newWindowSize.height);
  }
}
