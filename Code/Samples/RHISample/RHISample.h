#include <Core/Graphics/Geometry.h>
#include <Core/Input/InputManager.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Core/System/Window.h>
#include <Foundation/Application/Application.h>
#include <Foundation/Communication/Telemetry.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Containers/Blob.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/HTMLWriter.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/Strings/PathUtils.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Strings/StringBuilder.h>
#include <Foundation/Threading/TaskSystem.h>
#include <Foundation/Time/Clock.h>
#include <Foundation/Utilities/GraphicsUtils.h>
#include <RHI/Device/GraphicsDevice.h>
#include <RHI/Device/GraphicsDeviceOptions.h>
#include <RHI/GraphicsUtils.h>
#include <RHI/Resources/Buffer.h>
#include <RHI/Resources/ResourceFactory.h>
#include <RHI/Resources/Shader.h>


struct EZ_ALIGN_16(float4)
{
  float x;
  float y;
  float z;
  float w;
};

struct EZ_ALIGN_16(Color4)
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

class RHISampleWindow : public ezWindow
{
public:
  RHISampleWindow()
    : ezWindow()
  {
    m_bCloseRequested = false;
  }

  virtual void OnClickClose() override { m_bCloseRequested = true; }

  bool m_bCloseRequested;
};

static ezUInt32 g_uiWindowWidth = 1280;
static ezUInt32 g_uiWindowHeight = 720;

const ezInt32 g_iMaxHalfExtent = 20;
const bool g_bForceImmediateLoading = false;
const bool g_bPreloadAllTextures = false;

class RHISample : public ezApplication
{
public:
  typedef ezApplication SUPER;

  RHISample()
    : ezApplication("Texture Sample")
  {
  }

  void AfterCoreSystemsStartup() override
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

    RHIResourceFactory* factory = m_pDevice->GetResourceFactory();
    {
      Vertex quadVertices[] = {
        Vertex(ezVec3(-0.75f, 0.75f, 1.f), ezColor::Red),
        Vertex(ezVec3(0.75f, 0.75f, 1.f), ezColor::Green),
        Vertex(ezVec3(-0.75f, -0.75f, 1.f), ezColor::Blue),
        Vertex(ezVec3(0.75f, -0.75f, 1.f), ezColor::Yellow),
      };

      ezUInt16 quadIndices[] = {0, 1, 2, 0, 2, 3};

      VertexBuffer = factory->CreateBuffer(RHIBufferDescription{4 * sizeof(Vertex), RHIBufferUsage::VertexBuffer});
      IndexBuffer = factory->CreateBuffer(RHIBufferDescription{6 * sizeof(ezUInt16), RHIBufferUsage::IndexBuffer});

      m_pDevice->UpdateBuffer(VertexBuffer, 0, quadVertices);
      m_pDevice->UpdateBuffer(IndexBuffer, 0, quadIndices);

      ezDynamicArray<RHIVertexElementDescription> layoutElements;
      layoutElements.PushBack(RHIVertexElementDescription{"Position", RHIVertexElementSemantic::Position, RHIVertexElementFormat::Float3});
      layoutElements.PushBack(RHIVertexElementDescription{"Color", RHIVertexElementSemantic::Color, RHIVertexElementFormat::Float4});
      RHIVertexLayoutDescription layout(layoutElements);

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
      pipelineDesc.RasterizerState = RHIRasterizerStateDescription(RHIFaceCullMode::Back, RHIPolygonFillMode::Solid, RHIFrontFace::Clockwise, true, false);
      pipelineDesc.PrimitiveTopology = RHIPrimitiveTopology::TriangleList;

      ezDynamicArray<RHIVertexLayoutDescription> layouts;
      layouts.PushBack(layout);
      ezDynamicArray<RHIShader*> shaders;
      shaders.PushBack(VertexShader);
      shaders.PushBack(FragmentShader);

      //pipelineDesc.ShaderSet = RHIShaderSetDescription(layouts, shaders);
      pipelineDesc.ShaderSet.VertexLayouts = layouts;
      pipelineDesc.ShaderSet.Shaders = shaders;

      pipelineDesc.Outputs = m_pDevice->GetSwapchainFramebuffer()->GetOutputDescription();
      Pipeline = factory->CreateGraphicsPipeline(pipelineDesc);
      CommandList = factory->CreateCommandList();
    }
  }


  ApplicationExecution Run() override
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

  void OnDraw()
  {
    CommandList->Begin();
    CommandList->SetFramebuffer(m_pDevice->GetSwapchainFramebuffer());
    CommandList->ClearColorTarget(0, ezColor::Blue);

    CommandList->SetVertexBuffer(0, VertexBuffer);
    CommandList->SetIndexBuffer(IndexBuffer, RHIIndexFormat::UInt16);
    CommandList->SetPipeline(Pipeline);
    CommandList->DrawIndexed(4, 1, 0, 0, 0);
    CommandList->End();

    m_pDevice->SubmitCommands(CommandList);
    m_pDevice->SwapBuffers();
  }

  void BeforeCoreSystemsShutdown() override
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


private:
  RHISampleWindow* m_pWindow = nullptr;
  RHIGraphicsDevice* m_pDevice = nullptr;

  RHICommandList* CommandList = nullptr;
  RHIBuffer* VertexBuffer = nullptr;
  RHIBuffer* IndexBuffer = nullptr;
  ezDynamicArray<RHIShader*> Shaders;
  RHIPipeline* Pipeline = nullptr;
  RHIShader* VertexShader = nullptr;
  RHIShader* FragmentShader = nullptr;
};

EZ_CONSOLEAPP_ENTRY_POINT(RHISample);
