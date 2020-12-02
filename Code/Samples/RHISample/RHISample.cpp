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
#include <Texture/Image/Formats/ImageFileFormat.h>
#include <Texture/Image/Image.h>
#include <Texture/Image/ImageHeader.h>

RHIPixelFormat::Enum ImageFormatToRHIPixelFormat(ezImageFormat::Enum format, bool bSRGB)
{
  switch (format)
  {
    case ezImageFormat::R8G8B8A8_UNORM:
      if (bSRGB)
        return RHIPixelFormat::R8_G8_B8_A8_UNorm_SRgb;
      else
        return RHIPixelFormat::R8_G8_B8_A8_UNorm;

      // case ezImageFormat::R8G8B8A8_TYPELESS:
    case ezImageFormat::R8G8B8A8_UNORM_SRGB:
      return RHIPixelFormat::R8_G8_B8_A8_UNorm_SRgb;

    case ezImageFormat::R8G8B8A8_UINT:
      return RHIPixelFormat::R8_G8_B8_A8_UInt;

    case ezImageFormat::R8G8B8A8_SNORM:
      return RHIPixelFormat::R8_G8_B8_A8_SNorm;

    case ezImageFormat::R8G8B8A8_SINT:
      return RHIPixelFormat::R8_G8_B8_A8_SInt;

    case ezImageFormat::B8G8R8A8_UNORM:
      if (bSRGB)
        return RHIPixelFormat::B8_G8_R8_A8_UNorm_SRgb;
      else
        return RHIPixelFormat::B8_G8_R8_A8_UNorm;

    case ezImageFormat::B8G8R8X8_UNORM:
      if (bSRGB)
        return RHIPixelFormat::B8_G8_R8_A8_UNorm_SRgb;
      else
        return RHIPixelFormat::B8_G8_R8_A8_UNorm;

      // case ezImageFormat::B8G8R8A8_TYPELESS:
    case ezImageFormat::B8G8R8A8_UNORM_SRGB:
      return RHIPixelFormat::B8_G8_R8_A8_UNorm_SRgb;

      // case ezImageFormat::B8G8R8X8_TYPELESS:
    case ezImageFormat::B8G8R8X8_UNORM_SRGB:
      return RHIPixelFormat::B8_G8_R8_A8_UNorm_SRgb;

      // case ezImageFormat::B8G8R8_UNORM:

      // case ezImageFormat::BC1_TYPELESS:
    case ezImageFormat::BC1_UNORM:
      if (bSRGB)
        return RHIPixelFormat::BC1_Rgba_UNorm_SRgb;
      else
        return RHIPixelFormat::BC1_Rgba_UNorm;

    case ezImageFormat::BC1_UNORM_SRGB:
      return RHIPixelFormat::BC1_Rgba_UNorm_SRgb;

      // case ezImageFormat::BC2_TYPELESS:
    case ezImageFormat::BC2_UNORM:
      if (bSRGB)
        return RHIPixelFormat::BC2_UNorm_SRgb;
      else
        return RHIPixelFormat::BC2_UNorm;

    case ezImageFormat::BC2_UNORM_SRGB:
      return RHIPixelFormat::BC2_UNorm_SRgb;

      // case ezImageFormat::BC3_TYPELESS:
    case ezImageFormat::BC3_UNORM:
      if (bSRGB)
        return RHIPixelFormat::BC3_UNorm_SRgb;
      else
        return RHIPixelFormat::BC3_UNorm;

    case ezImageFormat::BC3_UNORM_SRGB:
      return RHIPixelFormat::BC3_UNorm_SRgb;

      // case ezImageFormat::BC4_TYPELESS:
    case ezImageFormat::BC4_UNORM:
      return RHIPixelFormat::BC4_UNorm;

    case ezImageFormat::BC4_SNORM:
      return RHIPixelFormat::BC4_SNorm;

      // case ezImageFormat::BC5_TYPELESS:
    case ezImageFormat::BC5_UNORM:
      return RHIPixelFormat::BC5_UNorm;

    case ezImageFormat::BC5_SNORM:
      return RHIPixelFormat::BC5_SNorm;

    //  // case ezImageFormat::BC6H_TYPELESS:
    case ezImageFormat::BC6H_UF16:
      return RHIPixelFormat::BC6_H_UFloat;

    case ezImageFormat::BC6H_SF16:
      return RHIPixelFormat::BC6_H_SFloat;

      // case ezImageFormat::BC7_TYPELESS:
    case ezImageFormat::BC7_UNORM:
      if (bSRGB)
        return RHIPixelFormat::BC7_UNorm_SRgb;
      else
        return RHIPixelFormat::BC7_UNorm;

    case ezImageFormat::BC7_UNORM_SRGB:
      return RHIPixelFormat::BC7_UNorm_SRgb;

    //case ezImageFormat::B5G6R5_UNORM:
    //  return RHIPixelFormat::B5_G6_R5_UNorm; /// \todo Not supported by some GPUs ?

    case ezImageFormat::R16_FLOAT:
      return RHIPixelFormat::R16_Float;

    case ezImageFormat::R32_FLOAT:
      return RHIPixelFormat::R32_Float;

    case ezImageFormat::R16G16_FLOAT:
      return RHIPixelFormat::R16_G16_Float;

    case ezImageFormat::R32G32_FLOAT:
      return RHIPixelFormat::R32_G32_Float;

    case ezImageFormat::R32G32B32_FLOAT:
      return RHIPixelFormat::R32_G32_B32_Float;

    case ezImageFormat::R16G16B16A16_FLOAT:
      return RHIPixelFormat::R16_G16_B16_A16_Float;

    case ezImageFormat::R32G32B32A32_FLOAT:
      return RHIPixelFormat::R32_G32_B32_A32_Float;

    case ezImageFormat::R16G16B16A16_UNORM:
      return RHIPixelFormat::R16_G16_B16_A16_UNorm;

    case ezImageFormat::R8_UNORM:
      return RHIPixelFormat::R8_UNorm;

    case ezImageFormat::R8G8_UNORM:
      return RHIPixelFormat::R8_G8_UNorm;

    case ezImageFormat::R16G16_UNORM:
      return RHIPixelFormat::R16_G16_UNorm;

    case ezImageFormat::R11G11B10_FLOAT:
      return RHIPixelFormat::R11_G11_B10_Float;

    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      break;
  }

  return RHIPixelFormat::Invalid;
}

struct Vertex
{
  ezVec3 Position;
  //ezColor Color;
  ezVec2 UV;

  Vertex(ezVec3 position, /*ezColor color,*/ ezVec2 uv)
  {
    Position = position;
    //Color = color;
    UV = uv;
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
  ezFileSystem::ResolveSpecialDirectory(sProjectDir, sProjectDirResolved).IgnoreResult();

  ezFileSystem::SetSpecialDirectory("project", sProjectDirResolved);

  // setup the 'asset management system'
  {
    // which redirection table to search
    ezDataDirectory::FolderType::s_sRedirectionFile = "AssetCache/LookupTable.ezAsset";
    // which platform assets to use
    ezDataDirectory::FolderType::s_sRedirectionPrefix = "AssetCache/PC/";
  }

  {

    ezFileSystem::AddDataDirectory("", "", ":", ezFileSystem::AllowWrites).IgnoreResult();
    ezFileSystem::AddDataDirectory(">appdir/", "AppBin", "bin", ezFileSystem::AllowWrites).IgnoreResult();                              // writing to the binary directory
    ezFileSystem::AddDataDirectory(">appdir/", "ShaderCache", "shadercache", ezFileSystem::AllowWrites).IgnoreResult();                 // for shader files
    ezFileSystem::AddDataDirectory(">user/ezEngine Project/RHISample", "AppData", "appdata", ezFileSystem::AllowWrites).IgnoreResult(); // app user data

    ezFileSystem::AddDataDirectory(">sdk/Data/Base", "Base", "base").IgnoreResult();
    ezFileSystem::AddDataDirectory(">sdk/Data/FreeContent", "Shared", "shared").IgnoreResult();
    ezFileSystem::AddDataDirectory(">project/", "Project", "project", ezFileSystem::AllowWrites).IgnoreResult();

    ezGlobalLog::AddLogWriter(ezLogWriter::Console::LogMessageHandler);
    ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);
  }

  ezTelemetry::CreateServer();
  ezPlugin::LoadPlugin("ezInspectorPlugin").IgnoreResult();

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
    WindowCreationDesc.m_WindowMode = ezWindowMode::WindowResizable;
    WindowCreationDesc.m_bClipMouseCursor = false;
    WindowCreationDesc.m_bShowMouseCursor = true;
    m_pWindow = EZ_DEFAULT_NEW(RHISampleWindow);
    m_pWindow->Initialize(WindowCreationDesc).IgnoreResult();
  }

  // Create device and resource factory
  {
    m_pSwapchainSource = EZ_DEFAULT_NEW(RHIWin32SwapchainSource, m_pWindow->GetNativeWindowHandle(), nullptr);
    RHIGraphicsDeviceOptions options(true);
    RHISwapchainDescription swapchainDesc(m_pSwapchainSource, g_uiWindowWidth, g_uiWindowHeight, RHIPixelFormat::D32_Float_S8_UInt, true);
    m_pDevice = GraphicsUtils::CreateD3D11(options, swapchainDesc);
    m_pWindow->SetGraphicsDevice(m_pDevice);
    ResourceFactory = m_pDevice->GetResourceFactory();
  }

  // now that we have a window and device, tell the engine to initialize the rendering infrastructure
  ezStartup::StartupHighLevelSystems();

  CreateTexturedCube();
  CreatePipelineState();
}

ezApplication::ApplicationExecution RHISample::Run()
{
  m_pWindow->ProcessWindowMessages();

  if (m_pWindow->m_bCloseRequested || ezInputManager::GetInputActionState("Main", "CloseApp") == ezKeyState::Pressed)
    return ApplicationExecution::Quit;

  // make sure time goes on
  ezClock::GetGlobalClock()->Update();

  // update all input state
  ezInputManager::Update(ezClock::GetGlobalClock()->GetTimeDiff());

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

void RHISample::CreateTexturedCube()
{
}

void RHISample::CreatePipelineState()
{
  // Cube

  //Vertex cubeVertices[] = {
  //  {ezVec3(-.5f, -.5f, -.5f), ezColor::Red},
  //  {ezVec3(-.5f, +.5f, -.5f), ezColor::Green},
  //  {ezVec3(+.5f, +.5f, -.5f), ezColor::Blue},
  //  {ezVec3(+.5f, -.5f, -.5f), ezColor::Violet},

  //  {ezVec3(-.5f, -.5f, +.5f), ezColor::Yellow},
  //  {ezVec3(-.5f, +.5f, +.5f), ezColor::Aqua},
  //  {ezVec3(+.5f, +.5f, +.5f), ezColor::Fuchsia},
  //  {ezVec3(+.5f, -.5f, +.5f), ezColor::Brown},
  //};

  Vertex cubeVertices[] =
    {
      {ezVec3(-1, -1, -1), ezVec2(0, 1)},
      {ezVec3(-1, +1, -1), ezVec2(0, 0)},
      {ezVec3(+1, +1, -1), ezVec2(1, 0)},
      {ezVec3(+1, -1, -1), ezVec2(1, 1)},

      {ezVec3(-1, -1, -1), ezVec2(0, 1)},
      {ezVec3(-1, -1, +1), ezVec2(0, 0)},
      {ezVec3(+1, -1, +1), ezVec2(1, 0)},
      {ezVec3(+1, -1, -1), ezVec2(1, 1)},

      {ezVec3(+1, -1, -1), ezVec2(0, 1)},
      {ezVec3(+1, -1, +1), ezVec2(1, 1)},
      {ezVec3(+1, +1, +1), ezVec2(1, 0)},
      {ezVec3(+1, +1, -1), ezVec2(0, 0)},

      {ezVec3(+1, +1, -1), ezVec2(0, 1)},
      {ezVec3(+1, +1, +1), ezVec2(0, 0)},
      {ezVec3(-1, +1, +1), ezVec2(1, 0)},
      {ezVec3(-1, +1, -1), ezVec2(1, 1)},

      {ezVec3(-1, +1, -1), ezVec2(1, 0)},
      {ezVec3(-1, +1, +1), ezVec2(0, 0)},
      {ezVec3(-1, -1, +1), ezVec2(0, 1)},
      {ezVec3(-1, -1, -1), ezVec2(1, 1)},

      {ezVec3(-1, -1, +1), ezVec2(1, 1)},
      {ezVec3(+1, -1, +1), ezVec2(0, 1)},
      {ezVec3(+1, +1, +1), ezVec2(0, 0)},
      {ezVec3(-1, +1, +1), ezVec2(1, 0)}};

  ezUInt16 cubeIndices[] = {
    2, 0, 1, 2, 3, 0,
    4, 6, 5, 4, 7, 6,
    8, 10, 9, 8, 11, 10,
    12, 14, 13, 12, 15, 14,
    16, 18, 17, 16, 19, 18,
    20, 21, 22, 20, 22, 23};

  {
    RHIGraphicsPipelineDescription pipelineDesc;

    // Texture
    {
      ezFileReader fReader;
      fReader.Open("ez-logo.png").IgnoreResult();

      if (fReader.IsOpen())
      {
        ezDynamicArray<ezUInt8> fileBuffer;
        fileBuffer.SetCountUninitialized((ezUInt32)fReader.GetFileSize());
        fReader.ReadBytes(fileBuffer.GetData(), fileBuffer.GetCount());
        fReader.Close();

        ezImage img;
        img.LoadFrom("ez-logo.png").IgnoreResult();

        RHITextureDescription desc(img.GetWidth(), img.GetHeight(), img.GetDepth(), img.GetNumMipLevels(), img.GetNumArrayIndices(), ImageFormatToRHIPixelFormat(img.GetImageFormat(), true), RHITextureUsage::Sampled, RHITextureType::Texture2D);
        CubeTexture = ResourceFactory->CreateTexture(desc);
        m_pDevice->UpdateTexture(CubeTexture, img.GetByteBlobPtr().GetPtr(), (ezUInt32)img.GetByteBlobPtr().GetCount(), 0, 0, 0, img.GetWidth(), img.GetHeight(), img.GetDepth(), 0, 0);
      }
    }

    // Shaders
    {
      ezFileReader fReader;
      fReader.Open("triangle.hlsl").IgnoreResult();
      if (fReader.IsOpen())
      {
        ezDynamicArray<ezUInt8> shaderBytes;
        shaderBytes.SetCountUninitialized((ezUInt32)fReader.GetFileSize());
        fReader.ReadBytes(shaderBytes.GetData(), shaderBytes.GetCount());

        RHIShaderDescription vertexShaderDesc = RHIShaderDescription{
          RHIShaderStages::Vertex,
          shaderBytes,
          "VSMain",
          true,
        };

        VertexShader = ResourceFactory->CreateShader(vertexShaderDesc);
        VertexShader->SetName("VertexShader");

        RHIShaderDescription fragmentShaderDesc = RHIShaderDescription{
          RHIShaderStages::Fragment,
          shaderBytes,
          "PSMain",
          true,
        };

        FragmentShader = ResourceFactory->CreateShader(fragmentShaderDesc);
        FragmentShader->SetName("FragmentShader");

        fReader.Close();
      }

      //fReader.Open("vs.o").IgnoreResult();
      //if (fReader.IsOpen())
      //{
      //  RHIShaderDescription vertexShaderDesc = RHIShaderDescription{
      //    RHIShaderStages::Vertex,
      //    ezDynamicArray<ezUInt8>(),
      //    "VSMain",
      //    false,
      //  };

      //  vertexShaderDesc.ShaderBytes.SetCountUninitialized((ezUInt32)fReader.GetFileSize());
      //  fReader.ReadBytes(vertexShaderDesc.ShaderBytes.GetData(), vertexShaderDesc.ShaderBytes.GetCount());

      //  VertexShader = ResourceFactory->CreateShader(vertexShaderDesc);
      //  VertexShader->SetName("VertexShader");

      //  fReader.Close();
      //}

      //fReader.Open("ps.o").IgnoreResult();
      //if (fReader.IsOpen())
      //{
      //  RHIShaderDescription fragmentShaderDesc = RHIShaderDescription{
      //    RHIShaderStages::Fragment,
      //    ezDynamicArray<ezUInt8>(),
      //    "PSMain",
      //    false,
      //  };

      //  fragmentShaderDesc.ShaderBytes.SetCountUninitialized((ezUInt32)fReader.GetFileSize());
      //  fReader.ReadBytes(fragmentShaderDesc.ShaderBytes.GetData(), fragmentShaderDesc.ShaderBytes.GetCount());

      //  FragmentShader = ResourceFactory->CreateShader(fragmentShaderDesc);
      //  FragmentShader->SetName("FragmentShader");

      //  fReader.Close();
      //}
    }

    pipelineDesc.BlendState = RHIBlendStateDescription::SingleOverrideBlend();
    pipelineDesc.DepthStencilState = RHIDepthStencilStateDescription(true, true, RHIComparisonKind::LessEqual);
    pipelineDesc.RasterizerState = RHIRasterizerStateDescription(RHIFaceCullMode::Back, RHIPolygonFillMode::Solid, RHIFrontFace::Clockwise, true, false);
    pipelineDesc.PrimitiveTopology = RHIPrimitiveTopology::TriangleList;

    //RHIShaderSetDescription shaderSet(
    //  {{RHIVertexElementDescription{"Position", RHIVertexElementSemantic::Position, RHIVertexElementFormat::Float3},
    //    RHIVertexElementDescription{"Color", RHIVertexElementSemantic::Color, RHIVertexElementFormat::Float4}}},
    //  {VertexShader,
    //    FragmentShader});
    //pipelineDesc.ShaderSet = shaderSet;

    ezDynamicArray<RHIVertexElementDescription> layoutElements;
    layoutElements.PushBack(RHIVertexElementDescription{"Position", RHIVertexElementSemantic::Position, RHIVertexElementFormat::Float3});
    //layoutElements.PushBack(RHIVertexElementDescription{"Color", RHIVertexElementSemantic::Color, RHIVertexElementFormat::Float4});
    layoutElements.PushBack(RHIVertexElementDescription{"UV", RHIVertexElementSemantic::TextureCoordinate, RHIVertexElementFormat::Float2});
    RHIVertexLayoutDescription vertexLayout(layoutElements);
    pipelineDesc.ShaderSet.VertexLayouts.PushBack(vertexLayout);

    pipelineDesc.ShaderSet.Shaders.PushBack(VertexShader);
    pipelineDesc.ShaderSet.Shaders.PushBack(FragmentShader);


    VertexBuffer = ResourceFactory->CreateBuffer(RHIBufferDescription{sizeof(cubeVertices), RHIBufferUsage::VertexBuffer});
    VertexBuffer->SetName("VertexBuffer");
    IndexBuffer = ResourceFactory->CreateBuffer(RHIBufferDescription{sizeof(cubeIndices), RHIBufferUsage::IndexBuffer});
    IndexBuffer->SetName("IndexBuffer");
    ConstantBuffer = ResourceFactory->CreateBuffer(RHIBufferDescription{sizeof(ezMat4), RHIBufferUsage::UniformBuffer});
    ConstantBuffer->SetName("ConstantBuffer");

    m_pDevice->UpdateBuffer(VertexBuffer, 0, reinterpret_cast<ezUInt8*>(cubeVertices), sizeof(cubeVertices));
    m_pDevice->UpdateBuffer(IndexBuffer, 0, reinterpret_cast<ezUInt8*>(cubeIndices), sizeof(cubeIndices));

    RHIResourceLayoutDescription resourceLayoutDesc;
    resourceLayoutDesc.Elements.PushBack(RHIResourceLayoutElementDescription("Projection", RHIResourceKind::UniformBuffer, RHIShaderStages::Vertex));
    resourceLayoutDesc.Elements.PushBack(RHIResourceLayoutElementDescription("LogoTexture", RHIResourceKind::TextureReadOnly, RHIShaderStages::Fragment));
    resourceLayoutDesc.Elements.PushBack(RHIResourceLayoutElementDescription("LogoSampler", RHIResourceKind::Sampler, RHIShaderStages::Fragment));
    ResourceLayout = ResourceFactory->CreateResourceLayout(resourceLayoutDesc);
    ResourceLayout->SetName("ResourceLayout");
    pipelineDesc.ResourceLayouts.PushBack(ResourceLayout);


    ezDynamicArray<RHIResource*> boundResources;

    boundResources.PushBack(ConstantBuffer);
    boundResources.PushBack(CubeTexture);
    boundResources.PushBack(m_pDevice->GetAniso4xSampler());


    RHIResourceSetDescription resourceSetDesc = RHIResourceSetDescription(ResourceLayout, boundResources);

    ResourceSet = ResourceFactory->CreateResourceSet(resourceSetDesc);
    ResourceSet->SetName("ResourceSet");

    pipelineDesc.Outputs = m_pDevice->GetSwapchainFramebuffer()->GetOutputDescription();
    Pipeline = ResourceFactory->CreateGraphicsPipeline(pipelineDesc);
    Pipeline->SetName("Pipeline");
    CommandList = ResourceFactory->CreateCommandList();
    CommandList->SetName("CommandList");
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
  static float time = 0.0f;

  ezMat4 rotX;
  rotX.SetRotationMatrixX(ezAngle::Radian(time));

  ezMat4 rotY;
  rotY.SetRotationMatrixX(ezAngle::Radian(time * 2));

  ezMat4 rotZ;
  rotZ.SetRotationMatrixX(ezAngle::Radian(time * .7f));

  ezMat4 world = rotZ * rotY * rotX;
  float aspect = (float)(m_pDevice->GetMainSwapchain()->GetFramebuffer()->GetWidth() / m_pDevice->GetMainSwapchain()->GetFramebuffer()->GetHeight());

  ezMat4 view = ezGraphicsUtils::CreateLookAtViewMatrix(ezVec3(0, 0, 5), ezVec3::ZeroVector(), ezVec3::UnitYAxis(), ezHandedness::Default);
  ezMat4 proj = ezGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovX(ezAngle::Radian(ezMath::Pi<float>() / 4.0f), aspect, .1f, 100.f, ezClipSpaceDepthRange::Default, ezClipSpaceYMode::Regular, ezHandedness::Default);

  ezMat4 viewProj = proj * view * world;

  WorldViewProjectionMatrix = viewProj;

  time += 0.00025f;
}

void RHISample::BeforeCoreSystemsShutdown()
{
  // make sure that no textures are continue to be streamed in while the engine shuts down
  ezResourceManager::EngineAboutToShutdown();

  // now we can destroy the graphics device

  CommandList->Dispose();
  EZ_DEFAULT_DELETE(CommandList);

  VertexBuffer->Dispose();
  EZ_DEFAULT_DELETE(VertexBuffer);

  IndexBuffer->Dispose();
  EZ_DEFAULT_DELETE(IndexBuffer);

  ConstantBuffer->Dispose();
  EZ_DEFAULT_DELETE(ConstantBuffer);

  FragmentShader->Dispose();
  EZ_DEFAULT_DELETE(FragmentShader);

  VertexShader->Dispose();
  EZ_DEFAULT_DELETE(VertexShader);

  ResourceLayout->Dispose();
  EZ_DEFAULT_DELETE(ResourceLayout);

  ResourceSet->Dispose();
  EZ_DEFAULT_DELETE(ResourceSet);

  Pipeline->Dispose();
  EZ_DEFAULT_DELETE(Pipeline);

  CubeTexture->Dispose();
  EZ_DEFAULT_DELETE(CubeTexture);

  m_pDevice->Dispose();
  EZ_DEFAULT_DELETE(m_pDevice);

  // finally destroy the window
  m_pWindow->Destroy().IgnoreResult();
  EZ_DEFAULT_DELETE(m_pWindow);

  EZ_DEFAULT_DELETE(m_pSwapchainSource);

  // TODO:
  // Device::Dispose -> ResourceFactory::Dispose -> [Clean up all resources]
}

void RHISampleWindow::SetGraphicsDevice(RHIGraphicsDevice* pDevice)
{
  m_pDevice = pDevice;
}

void RHISampleWindow::OnResize(const ezSizeU32& newWindowSize)
{
  if (m_pDevice != nullptr)
  {
    m_pDevice->ResizeMainWindow(newWindowSize.width, newWindowSize.height);
  }
}
