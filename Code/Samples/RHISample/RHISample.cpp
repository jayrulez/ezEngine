#include <Core/Graphics/Geometry.h>
#include <Core/Input/InputManager.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Core/System/Window.h>
#include <Foundation/Application/Application.h>
#include <Foundation/Communication/Telemetry.h>
#include <Foundation/Configuration/Startup.h>
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

#include <RHI/DX11/GraphicsDevice_DX11.h>
#include <RHI/DX12/GraphicsDevice_DX12.h>
#include <RHI/Vulkan/GraphicsDevice_Vulkan.h>
#include <RHI/Graphics.h>
#include <RHI/GraphicsDevice.h>
#include <Foundation/Containers/Blob.h>

using namespace wiGraphics;

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
  float4 position;
  Color4 color;
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
      m_pDevice = EZ_DEFAULT_NEW(GraphicsDevice_DX12, (HWND)m_pWindow->GetNativeWindowHandle(), false, true);
    }

    // now that we have a window and device, tell the engine to initialize the rendering infrastructure
    ezStartup::StartupHighLevelSystems();


    {
      vertexShader = EZ_DEFAULT_NEW(Shader);
      pixelShader = EZ_DEFAULT_NEW(Shader);

      ezBlob blob;
      ezFileReader fReader;

      ezDynamicArray<ezUInt8> fileBuffer;

      ezUInt8* psData = new ezUInt8[10000];
      ezUInt64 psDataSize = 0;

      //fReader.Open("ps5_0.o");
      //fReader.Open("ps_sv11.o");
      fReader.Open("ps6_2.o");
      if (fReader.IsOpen())
      {
        psDataSize = fReader.ReadBytes(psData, fReader.GetFileSize());
        m_pDevice->CreateShader(SHADERSTAGE::PS, psData, psDataSize, pixelShader);
        delete[] psData;
      }
      fReader.Close();
      blob.Clear();

      ezUInt8* vsData = new ezUInt8[10000];
      ezUInt64 vsDataSize = 0;

      //fReader.Open("vs5_0.o");
      //fReader.Open("vs_sv11.o");
      fReader.Open("vs6_2.o");
      if (fReader.IsOpen())
      {
        vsDataSize = fReader.ReadBytes(vsData, fReader.GetFileSize());
        m_pDevice->CreateShader(SHADERSTAGE::VS, vsData, vsDataSize, vertexShader);
        delete[] vsData;
      }
      fReader.Close();
      blob.Clear();
      
    }

    {
      InputLayoutDesc layout[] = {
        {"ATTRIBUTE", 0, FORMAT_R32G32B32A32_FLOAT, 0, InputLayoutDesc::APPEND_ALIGNED_ELEMENT, INPUT_PER_VERTEX_DATA, 0},
        {"ATTRIBUTE", 1, FORMAT_R32G32B32A32_FLOAT, 0, InputLayoutDesc::APPEND_ALIGNED_ELEMENT, INPUT_PER_VERTEX_DATA, 0},
      };

      m_pDevice->CreateInputLayout(layout, EZ_ARRAY_SIZE(layout), vertexShader, &inputLayout);

      PipelineStateDesc pipelineDesc = {};
      pipelineDesc.vs = vertexShader;
      pipelineDesc.ps = pixelShader;
      pipelineDesc.pt = TRIANGLELIST;
      pipelineDesc.il = &inputLayout;

      //BlendStateDesc bsDesc;
      //bsDesc.IndependentBlendEnable = true;

      //m_pDevice->CreateBlendState(&bsDesc, &blendState);

      //pipelineDesc.bs = &blendState;

      //BlendStateDesc bd;
      //bd.RenderTarget[0].BlendEnable = true;
      //bd.RenderTarget[0].SrcBlend = BLEND_SRC_ALPHA;
      //bd.RenderTarget[0].DestBlend = BLEND_INV_SRC_ALPHA;
      //bd.RenderTarget[0].BlendOp = BLEND_OP_ADD;
      //bd.RenderTarget[0].SrcBlendAlpha = BLEND_ONE;
      //bd.RenderTarget[0].DestBlendAlpha = BLEND_ONE;
      //bd.RenderTarget[0].BlendOpAlpha = BLEND_OP_ADD;
      //bd.RenderTarget[0].RenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
      //bd.IndependentBlendEnable = false;
      //m_pDevice->CreateBlendState(&bd, &blendState);

      //pipelineDesc.bs = &blendState;

      m_pDevice->CreatePipelineState(&pipelineDesc, &pipeline);
    }

    {
      Vertex quadVertices[] = {
        {{-0.5f, 0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
        {{-0.5f, -0.5, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
      };

      GPUBufferDesc bufferDesc{};
      bufferDesc.Usage = USAGE_IMMUTABLE;
      bufferDesc.BindFlags = BIND_VERTEX_BUFFER;
      bufferDesc.ByteWidth = sizeof(quadVertices);
      bufferDesc.StructureByteStride = sizeof(Vertex);
      SubresourceData quadData;
      quadData.pSysMem = quadVertices;
      m_pDevice->CreateBuffer(&bufferDesc, &quadData, &vertexBuffer);

      // Index buffer
      const ezUInt16 indices[] = {
        0, 1, 2, 0, 2, 3,
        6, 5, 4, 7, 6, 4,
        8, 9, 10, 8, 10, 11,
        14, 13, 12, 15, 14, 12,
        16, 17, 18, 16, 18, 19,
        22, 21, 20, 23, 22, 20
      };

      bufferDesc.Usage = USAGE_IMMUTABLE;
      bufferDesc.BindFlags = BIND_INDEX_BUFFER;
      bufferDesc.ByteWidth = sizeof(indices);
      SubresourceData indexData;
      indexData.pSysMem = indices;
      m_pDevice->CreateBuffer(&bufferDesc, &indexData, &indexBuffer);

      GPUBufferDesc bd;
      bd.Usage = USAGE_DYNAMIC;
      bd.ByteWidth = sizeof(XMFLOAT4X4);
      bd.BindFlags = BIND_CONSTANT_BUFFER;
      bd.CPUAccessFlags = CPU_ACCESS_WRITE;

      m_pDevice->CreateBuffer(&bd, nullptr, &constantBuffer);
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
      CommandList commandList = m_pDevice->BeginCommandList();
      m_pDevice->PresentBegin(commandList);

      OnDraw(commandList);

      m_pDevice->PresentEnd(commandList);
    }

    // needs to be called once per frame
    ezResourceManager::PerFrameUpdate();

    // tell the task system to finish its work for this frame
    // this has to be done at the very end, so that the task system will only use up the time that is left in this frame for
    // uploading GPU data etc.
    ezTaskSystem::FinishFrameTasks();

    return ezApplication::Continue;
  }

  void OnDraw(CommandList commandList)
  {
    static float time = 0.0f;
    XMMATRIX world = XMMatrixRotationX(time) * XMMatrixRotationY(time * 2) * XMMatrixRotationZ(time * .7f);

    

    float aspect = (float)(m_pWindow->GetClientAreaSize().width / m_pWindow->GetClientAreaSize().height);
    XMMATRIX view = XMMatrixLookAtRH(XMVectorSet(0, 0, 5, 1), XMVectorZero(), XMVectorSet(0, 1, 0, 1));
    XMMATRIX proj = XMMatrixPerspectiveFovRH(ezMath::Pi<float>() / 4.0f, aspect, 0.1f, 100);
    XMMATRIX viewProj = XMMatrixMultiply(world, XMMatrixMultiply(view, proj));

    //Matrix4x4 projectionMatrix;
    //Matrix4x4::CreatePerspectiveFieldOfView(Pi / 4.0f, aspect, 0.1f, 100, &projectionMatrix);

    XMFLOAT4X4 worldViewProjection;
    XMStoreFloat4x4(&worldViewProjection, viewProj);
    m_pDevice->UpdateBuffer(&constantBuffer, &worldViewProjection, commandList);

    const GPUBuffer* vbs[] = {
      &vertexBuffer,
    };

    uint32_t stride = sizeof(Vertex);
    m_pDevice->BindVertexBuffers(vbs, 0, 1, &stride, nullptr, commandList);
    m_pDevice ->BindIndexBuffer(&indexBuffer, INDEXFORMAT_16BIT, 0, commandList);
    m_pDevice->BindPipelineState(&pipeline, commandList);
    m_pDevice->BindConstantBuffer(SHADERSTAGE::VS, &constantBuffer, 0, commandList);
    //m_pDevice->Draw(36,0, commandList);
    m_pDevice->DrawIndexed(36, 0, 0, commandList);

    time += 0.01f;
  }

  void BeforeCoreSystemsShutdown() override
  {
    // make sure that no textures are continue to be streamed in while the engine shuts down
    ezResourceManager::EngineAboutToShutdown();

    EZ_DEFAULT_DELETE(vertexShader);
    EZ_DEFAULT_DELETE(pixelShader);
    EZ_DEFAULT_DELETE(texture);

    // now we can destroy the graphics device

    EZ_DEFAULT_DELETE(m_pDevice);

    // finally destroy the window
    m_pWindow->Destroy();
    EZ_DEFAULT_DELETE(m_pWindow);
  }


private:
  RHISampleWindow* m_pWindow = nullptr;
  GraphicsDevice* m_pDevice = nullptr;
  BlendState blendState;
  GPUBuffer vertexBuffer;
  GPUBuffer indexBuffer;
  GPUBuffer constantBuffer;
  PipelineState pipeline;
  InputLayout inputLayout;
  Sampler sampler;

  Shader* vertexShader = nullptr;
  Shader* pixelShader = nullptr;
  Texture* texture = nullptr;
};

EZ_CONSOLEAPP_ENTRY_POINT(RHISample);
