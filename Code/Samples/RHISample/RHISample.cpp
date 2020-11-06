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


struct float4
{
  float x;
  float y;
  float z;
  float w;
};

struct Color4
{
  float r;
  float g;
  float b;
  float a;
};

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

  ezTelemetry::CreateServer();
  ezPlugin::LoadPlugin("ezInspectorPlugin");

  // Register Input
  {
    ezInputActionConfig cfg;

    cfg = ezInputManager::GetInputActionConfig("Main", "CloseApp");
    cfg.m_sInputSlotTrigger[0] = ezInputSlot_KeyEscape;
    ezInputManager::SetInputActionConfig("Main", "CloseApp", cfg, true);

    cfg = ezInputManager::GetInputActionConfig("Main", "MovePosX");
    cfg.m_sInputSlotTrigger[0] = ezInputSlot_MouseMovePosX;
    cfg.m_bApplyTimeScaling = false;
    ezInputManager::SetInputActionConfig("Main", "MovePosX", cfg, true);

    cfg = ezInputManager::GetInputActionConfig("Main", "MoveNegX");
    cfg.m_sInputSlotTrigger[0] = ezInputSlot_MouseMoveNegX;
    cfg.m_bApplyTimeScaling = false;
    ezInputManager::SetInputActionConfig("Main", "MoveNegX", cfg, true);

    cfg = ezInputManager::GetInputActionConfig("Main", "MovePosY");
    cfg.m_sInputSlotTrigger[0] = ezInputSlot_MouseMovePosY;
    cfg.m_bApplyTimeScaling = false;
    ezInputManager::SetInputActionConfig("Main", "MovePosY", cfg, true);

    cfg = ezInputManager::GetInputActionConfig("Main", "MoveNegY");
    cfg.m_sInputSlotTrigger[0] = ezInputSlot_MouseMoveNegY;
    cfg.m_bApplyTimeScaling = false;
    ezInputManager::SetInputActionConfig("Main", "MoveNegY", cfg, true);

    cfg = ezInputManager::GetInputActionConfig("Main", "MouseDown");
    cfg.m_sInputSlotTrigger[0] = ezInputSlot_MouseButton0;
    cfg.m_bApplyTimeScaling = false;
    ezInputManager::SetInputActionConfig("Main", "MouseDown", cfg, true);
  }

  // Create a window for rendering
  {
    ezWindowCreationDesc WindowCreationDesc;
    WindowCreationDesc.m_Resolution.width = g_uiWindowWidth;
    WindowCreationDesc.m_Resolution.height = g_uiWindowHeight;
    m_pWindow = EZ_DEFAULT_NEW(RHISampleWindow);
    m_pWindow->Initialize(WindowCreationDesc);
  }

  // Create a device
  {
    RHIGraphicsDeviceOptions options(true);
    RHISwapchainDescription swapchainDesc(new RHIWin32SwapchainSource(m_pWindow->GetNativeWindowHandle(), nullptr), 640, 480, std::nullopt, true);
    m_pDevice = GraphicsUtils::CreateD3D11(options, swapchainDesc);
  }

  // now that we have a window and device, tell the engine to initialize the rendering infrastructure
  ezStartup::StartupHighLevelSystems();

  WorldProjectionMatrix.SetIdentity();
  CameraPosition = ezVec3(1, 1, 0);

  RHIResourceFactory* factory = m_pDevice->GetResourceFactory();
  {
    Vertex quadVertices[] = {
      Vertex(ezVec3(-0.75f, 0.75f, 1.f), ezColor::Red),
      Vertex(ezVec3(0.75f, 0.75f, 1.f), ezColor::Green),
      Vertex(ezVec3(-0.75f, -0.75f, 1.f), ezColor::Blue),
      Vertex(ezVec3(0.75f, -0.75f, 1.f), ezColor::Yellow),
    };

    ezUInt16 quadIndices[] = {0, 1, 2, 3};

    Vertex cubeVertices[] = {
      Vertex(ezVec3(-0.5, -0.5, -0.5), ezColor(1, 0, 0, 1)),
      Vertex(ezVec3(-0.5, +0.5, -0.5), ezColor(0, 1, 0, 1)),
      Vertex(ezVec3(+0.5, +0.5, -0.5), ezColor(0, 0, 1, 1)),
      Vertex(ezVec3(+0.5, -0.5, -0.5), ezColor(1, 1, 1, 1)),


      Vertex(ezVec3(-0.5, -0.5, +0.5), ezColor(1, 1, 0, 1)),
      Vertex(ezVec3(-0.5, +0.5, +0.5), ezColor(0, 1, 1, 1)),
      Vertex(ezVec3(+0.5, +0.5, +0.5), ezColor(1, 0, 1, 1)),
      Vertex(ezVec3(+0.5, -0.5, +0.5), ezColor(0.2f, 0.2f, 0.2f, 1.f)),
    };

    ezUInt16 cubeIndices[] = {
      2, 0, 1, 2, 3, 0,
      4, 6, 5, 4, 7, 6,
      0, 7, 4, 0, 3, 7,
      1, 0, 4, 1, 4, 5,
      1, 5, 2, 5, 6, 2,
      3, 6, 7, 3, 2, 6};

    VertexBuffer = factory->CreateBuffer(RHIBufferDescription{sizeof(cubeVertices), RHIBufferUsage::VertexBuffer});
    IndexBuffer = factory->CreateBuffer(RHIBufferDescription{sizeof(cubeIndices), RHIBufferUsage::IndexBuffer});
    ConstantBuffer = factory->CreateBuffer(RHIBufferDescription{sizeof(ezMat4), RHIBufferUsage::UniformBuffer});

    m_pDevice->UpdateBuffer(VertexBuffer, 0, reinterpret_cast<ezUInt8*>(cubeVertices), sizeof(cubeVertices));
    m_pDevice->UpdateBuffer(IndexBuffer, 0, reinterpret_cast<ezUInt8*>(cubeIndices), sizeof(cubeIndices));

    ezDynamicArray<RHIVertexElementDescription> layoutElements;
    layoutElements.PushBack(RHIVertexElementDescription{"Position", RHIVertexElementSemantic::Position, RHIVertexElementFormat::Float3});
    layoutElements.PushBack(RHIVertexElementDescription{"Color", RHIVertexElementSemantic::Color, RHIVertexElementFormat::Float4});
    RHIVertexLayoutDescription vertexLayout(layoutElements);

    ezFileReader fReader;

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


      VertexShader = factory->CreateShader(vertexShaderDesc);

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

      FragmentShader = factory->CreateShader(fragmentShaderDesc);

      fReader.Close();
    }

    RHIGraphicsPipelineDescription pipelineDesc;
    pipelineDesc.BlendState = RHIBlendStateDescription::SingleOverrideBlend();
    pipelineDesc.DepthStencilState = RHIDepthStencilStateDescription(true, true, RHIComparisonKind::LessEqual);
    pipelineDesc.RasterizerState = RHIRasterizerStateDescription(RHIFaceCullMode::None, RHIPolygonFillMode::Solid, RHIFrontFace::Clockwise, true, false);
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


    RHIResourceLayout* resourceLayout = factory->CreateResourceLayout(resourceLayoutDesc);

    pipelineDesc.ResourceLayouts.PushBack(resourceLayout);


    ezDynamicArray<RHIResource*> boundResources;

    boundResources.PushBack(ConstantBuffer);


    RHIResourceSetDescription resourceSetDesc = RHIResourceSetDescription(resourceLayout, boundResources);

    ResourceSet = factory->CreateResourceSet(resourceSetDesc);

    pipelineDesc.Outputs = m_pDevice->GetSwapchainFramebuffer()->GetOutputDescription();
    Pipeline = factory->CreateGraphicsPipeline(pipelineDesc);
    CommandList = factory->CreateCommandList();
  }
}

ezApplication::ApplicationExecution RHISample::Run()
{
  m_pWindow->ProcessWindowMessages();

  if (m_pWindow->m_bCloseRequested || ezInputManager::GetInputActionState("Main", "CloseApp") == ezKeyState::Pressed)
    return ApplicationExecution::Quit;

  // make sure time goes on
  ezClock::GetGlobalClock()->Update();

  if (ezInputManager::GetInputActionState("Main", "MouseDown") == ezKeyState::Down)
  {
    float fInputValue = 0.0f;
    const float fMouseSpeed = 0.5f;
  }

  // update all input state
  ezInputManager::Update(ezClock::GetGlobalClock()->GetTimeDiff());

  // make sure telemetry is sent out regularly
  ezTelemetry::PerFrameUpdate();

  // do the rendering
  {
    // Before starting to render in a frame call this function
    //CommandList commandList = m_pDevice->BeginCommandList();
    //m_pDevice->PresentBegin(commandList);

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

void RHISample::OnDraw()
{
  static float time = 0.f;
  WorldProjectionMatrix.Transpose();



  ezMat4 yRot;
  yRot.SetRotationMatrixY(ezAngle::Degree(time * 1.f));

  ezMat4 xRot;
  xRot.SetRotationMatrixY(ezAngle::Degree(-ezMath::Pi<float>() * 0.1f));

  ezMat4 cubeTransform = yRot * xRot;

  ezMat4 view;
  view.SetIdentity();
  view.SetTranslationVector(ezVec3(20.f, 20.f, 20.f));

  constexpr float fov = ezMath::Pi<float>() / 4.f;
  float aspect = (float)(m_pDevice->GetMainSwapchain()->GetFramebuffer()->GetWidth() / m_pDevice->GetMainSwapchain()->GetFramebuffer()->GetHeight());
  float nearClip = 1.f;
  float farClip = 1000.f;

  ezMat4 proj = ezGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovY(ezAngle::Degree(fov), aspect, nearClip, farClip);

  WorldProjectionMatrix = cubeTransform * view * proj;



  //ezCamera camera;
  //camera.SetCameraMode(ezCameraMode::PerspectiveFixedFovX, ezMath::Pi<float>() / 4 *aspect, 1, 10000);
  ////camera.LookAt(ezVec3(-10,-10,5), ezVec3(0,0,0), ezVec3::UnitYAxis());
  //ezMat4 view = camera.GetViewMatrix(ezCameraEye::Right);
  //ezMat4 proj;
  //camera.GetProjectionMatrix(aspect, proj);

  //ezMat4 t = view * proj;

  //ezMat4 viewProj = world * t;

  //bool isPerspective = camera.IsPerspective();

  //WorldProjectionMatrix = viewProj;

  CommandList->Begin();
  CommandList->SetFramebuffer(m_pDevice->GetSwapchainFramebuffer());
  CommandList->ClearColorTarget(0, ezColor::Black);

  CommandList->SetVertexBuffer(0, VertexBuffer);
  CommandList->SetIndexBuffer(IndexBuffer, RHIIndexFormat::UInt16);
  CommandList->UpdateBuffer(ConstantBuffer, 0, reinterpret_cast<ezUInt8*>(&WorldProjectionMatrix), sizeof(ezMat4));
  CommandList->SetPipeline(Pipeline);
  CommandList->SetGraphicsResourceSet(0, ResourceSet);
  CommandList->DrawIndexed(36, 1, 0, 0, 0);
  CommandList->End();

  m_pDevice->SubmitCommands(CommandList);
  m_pDevice->SwapBuffers();

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


  m_pDevice->Dispose();
  delete m_pDevice;

  // finally destroy the window
  m_pWindow->Destroy();
  EZ_DEFAULT_DELETE(m_pWindow);
}
