#include <RHISample/RHISample.h>

#include <Core/Input/InputManager.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Core/System/Window.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/Time/Clock.h>
#include <Foundation/Utilities/GraphicsUtils.h>

#include <RHIDX11/Device/GraphicsDeviceDX11.h>

static ezUInt32 g_uiWindowWidth = 640;
static ezUInt32 g_uiWindowHeight = 480;

struct Vertex
{
  ezVec3 position;
  ezColor color;
};


class ezRHISampleWindow : public ezWindow
{
public:
  ezRHISampleWindow()
    : ezWindow()
  {
    m_bCloseRequested = false;
  }

  virtual void OnClickClose() override { m_bCloseRequested = true; }

  bool m_bCloseRequested;
};

ezRHISampleApp::ezRHISampleApp()
  : ezApplication("RHI Sample")
{
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
    CommandList commandList = m_pDevice->BeginCommandList();
    m_pDevice->RenderPassBegin(m_pSwapChain, commandList);
    Viewport viewport;
    viewport.Width = (float)m_pSwapChain->desc.m_pWindow->GetClientAreaSize().width;
    viewport.Height = (float)m_pSwapChain->desc.m_pWindow->GetClientAreaSize().height;
    m_pDevice->BindViewports(1, &viewport, commandList);

    {
      static float time = 0.0f;

      ezMat4 rotX;
      rotX.SetRotationMatrixX(ezAngle::Radian(time));

      ezMat4 rotY;
      rotY.SetRotationMatrixX(ezAngle::Radian(time * 2));

      ezMat4 rotZ;
      rotZ.SetRotationMatrixX(ezAngle::Radian(time * .7f));

      ezMat4 world = rotZ * rotY * rotX;

      float aspect = (float)(m_pWindow->GetClientAreaSize().width / m_pWindow->GetClientAreaSize().height);

      ezMat4 view = ezGraphicsUtils::CreateLookAtViewMatrix(ezVec3(0, 0, 5), ezVec3::ZeroVector(), ezVec3::UnitYAxis(), ezHandedness::Default);
      ezMat4 proj = ezGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovX(ezAngle::Radian(ezMath::Pi<float>() / 4.0f), aspect, .1f, 100.f, ezClipSpaceDepthRange::Default, ezClipSpaceYMode::Regular, ezHandedness::Default);

      ezMat4 viewProj = proj * view * world;


      ezMat4 worldViewProjection = viewProj;

      m_pDevice->UpdateBuffer(&m_ConstantBuffer, &worldViewProjection, commandList);

      const GPUBuffer* vbs[] = {
        &m_VertexBuffer,
      };

      uint32_t stride = sizeof(Vertex);
      m_pDevice->BindVertexBuffers(vbs, 0, 1, &stride, nullptr, commandList);
      m_pDevice->BindIndexBuffer(&m_IndexBuffer, INDEXBUFFER_FORMAT::INDEXFORMAT_16BIT, 0, commandList);
      m_pDevice->BindPipelineState(&m_Pipeline, commandList);
      m_pDevice->BindConstantBuffer(SHADERSTAGE::VS, &m_ConstantBuffer, 0, commandList);
      m_pDevice->DrawIndexed(36, 0, 0, commandList);

      time += 0.01f;
    }

    m_pDevice->RenderPassEnd(commandList);
    m_pDevice->SubmitCommandLists();
  }

  // needs to be called once per frame
  ezResourceManager::PerFrameUpdate();

  // tell the task system to finish its work for this frame
  // this has to be done at the very end, so that the task system will only use up the time that is left in this frame for
  // uploading GPU data etc.
  ezTaskSystem::FinishFrameTasks();

  return ezApplication::Execution::Continue;
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
    WindowCreationDesc.m_Title = "RHI Sample";
    WindowCreationDesc.m_bShowMouseCursor = true;
    WindowCreationDesc.m_bClipMouseCursor = false;
    WindowCreationDesc.m_WindowMode = ezWindowMode::WindowResizable;
    m_pWindow = EZ_DEFAULT_NEW(ezRHISampleWindow);
    m_pWindow->Initialize(WindowCreationDesc).IgnoreResult();
  }

  // Create a device
  {
    m_pDevice = EZ_DEFAULT_NEW(ezRHIGraphicsDeviceDX11, true);

    SwapChainDesc desc;
    desc.buffercount = 3;
    desc.format = FORMAT_R10G10B10A2_UNORM;
    desc.m_pWindow = m_pWindow;

    m_pSwapChain = EZ_DEFAULT_NEW(SwapChain);
    m_pDevice->CreateSwapChain(&desc, m_pSwapChain);
  }

  // now that we have a window and device, tell the engine to initialize the rendering infrastructure
  ezStartup::StartupHighLevelSystems();

  // Shaders
  {
    m_pVertexShader = EZ_DEFAULT_NEW(Shader);
    m_pPixelShader = EZ_DEFAULT_NEW(Shader);

    ezFileReader fReader;
    ezDynamicArray<ezUInt8> dataBuffer;

    fReader.Open("ps5_0.o").IgnoreResult();

    if (fReader.IsOpen())
    {
      dataBuffer.SetCountUninitialized((ezUInt32)fReader.GetFileSize());

      ezUInt64 psDataSize = fReader.ReadBytes(dataBuffer.GetData(), fReader.GetFileSize());
      m_pDevice->CreateShader(SHADERSTAGE::PS, dataBuffer.GetData(), psDataSize, m_pPixelShader);

      dataBuffer.Clear();
      fReader.Close();
    }

    fReader.Open("vs5_0.o").IgnoreResult();

    if (fReader.IsOpen())
    {
      dataBuffer.SetCountUninitialized((ezUInt32)fReader.GetFileSize());

      ezUInt64 vsDataSize = fReader.ReadBytes(dataBuffer.GetData(), fReader.GetFileSize());
      m_pDevice->CreateShader(SHADERSTAGE::VS, dataBuffer.GetData(), vsDataSize, m_pVertexShader);

      dataBuffer.Clear();
      fReader.Close();
    }
  }

  // Pipeline
  {

    PipelineStateDesc psDesc = {};
    psDesc.vs = m_pVertexShader;
    psDesc.ps = m_pPixelShader;
    psDesc.pt = PRIMITIVETOPOLOGY::TRIANGLELIST;


    InputLayout inputLayout;
    inputLayout.elements.PushBack({"ATTRIBUTE", 0, FORMAT::FORMAT_R32G32B32_FLOAT, 0, InputLayout::APPEND_ALIGNED_ELEMENT, INPUT_CLASSIFICATION::INPUT_PER_VERTEX_DATA});
    inputLayout.elements.PushBack({"ATTRIBUTE", 1, FORMAT::FORMAT_R32G32B32A32_FLOAT, 0, InputLayout::APPEND_ALIGNED_ELEMENT, INPUT_CLASSIFICATION::INPUT_PER_VERTEX_DATA});
    psDesc.il = &inputLayout;

    m_BlendState.IndependentBlendEnable = true;
    psDesc.bs = &m_BlendState;

    psDesc.rs = &m_RasterizerState;

    m_pDevice->CreatePipelineState(&psDesc, &m_Pipeline);
  }

  // Buffers
  {
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

    GPUBufferDesc bufferDesc{};
    bufferDesc.Usage = USAGE::USAGE_IMMUTABLE;
    bufferDesc.BindFlags = BIND_FLAG::BIND_VERTEX_BUFFER;
    bufferDesc.ByteWidth = sizeof(cubeVertices);
    //bufferDesc.StructureByteStride = sizeof(Vertex);
    SubresourceData cubeData;
    cubeData.pSysMem = cubeVertices;
    m_pDevice->CreateBuffer(&bufferDesc, &cubeData, &m_VertexBuffer);

    // Index buffer
    const ezUInt16 indices[] = {
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

    bufferDesc.Usage = USAGE::USAGE_IMMUTABLE;
    bufferDesc.BindFlags = BIND_FLAG::BIND_INDEX_BUFFER;
    bufferDesc.ByteWidth = sizeof(indices);
    SubresourceData indexData;
    indexData.pSysMem = indices;
    m_pDevice->CreateBuffer(&bufferDesc, &indexData, &m_IndexBuffer);

    GPUBufferDesc bd;
    bd.Usage = USAGE::USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(ezMat4);
    bd.BindFlags = BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = CPU_ACCESS_WRITE;

    m_pDevice->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
  }
}

void ezRHISampleApp::BeforeHighLevelSystemsShutdown()
{
  EZ_DEFAULT_DELETE(m_pVertexShader);
  EZ_DEFAULT_DELETE(m_pPixelShader);
  //EZ_DEFAULT_DELETE(m_pTexture);

  EZ_DEFAULT_DELETE(m_pSwapChain);
  EZ_DEFAULT_DELETE(m_pDevice);
  EZ_DEFAULT_DELETE(m_pWindow);
}

EZ_CONSOLEAPP_ENTRY_POINT(ezRHISampleApp);
