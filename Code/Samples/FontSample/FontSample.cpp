#include <Core/Graphics/Camera.h>
#include <Core/Graphics/Geometry.h>
#include <Core/Input/InputManager.h>
#include <Core/ResourceManager/ResourceManager.h>
<<<<<<< HEAD
#include <FontSample.h>
#include <Foundation/Communication/Telemetry.h>
=======
>>>>>>> b79fbf4b8 (Addressed some review comments)
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/Time/Clock.h>
<<<<<<< HEAD
#include <Foundation/Utilities/GraphicsUtils.h>
=======
#include <FontSample.h>
>>>>>>> b79fbf4b8 (Addressed some review comments)
#include <RendererCore/Font/TextData.h>
#include <RendererCore/Meshes/MeshBufferResource.h>
#include <RendererCore/RenderContext/RenderContext.h>
#include <RendererCore/ShaderCompiler/ShaderManager.h>
#include <RendererCore/Textures/Texture2DResource.h>
#include <RendererDX11/Device/DeviceDX11.h>
#include <RendererFoundation/Context/Context.h>
#include <RendererFoundation/Descriptors/Descriptors.h>
#include <RendererFoundation/Device/Device.h>
#include <RendererFoundation/Device/SwapChain.h>
#include <RendererFoundation/Resources/RenderTargetSetup.h>

<<<<<<< HEAD
static ezUInt32 g_uiWindowWidth = 1280;
static ezUInt32 g_uiWindowHeight = 720;
=======
static ezUInt32 g_uiWindowWidth = 640;
static ezUInt32 g_uiWindowHeight = 480;
>>>>>>> b79fbf4b8 (Addressed some review comments)

ezFontRenderingApp::ezFontRenderingApp()
  : ezApplication("Font Rendering")
{
<<<<<<< HEAD
  m_vCameraPosition.SetZero();

  m_TextSpriteDesc.Anchor = ezTextSpriteAnchor::TopLeft;
  m_TextSpriteDesc.HorizontalAlignment = ezTextHorizontalAlignment::Left;
  m_TextSpriteDesc.VerticalAlignment = ezTextVerticalAlignment::Top;
  m_TextSpriteDesc.FontSize = 30;
  m_TextSpriteDesc.Width = 200;
  m_TextSpriteDesc.Height = 480;
  m_TextSpriteDesc.Color = ezColor::Red;
  m_TextSpriteDesc.Text = "Hello World. This is a test";
  m_TextSpriteDesc.WrapText = true;
  m_TextSpriteDesc.BreakTextWhenWrapped = true;
=======
>>>>>>> b79fbf4b8 (Addressed some review comments)
}

ezApplication::ApplicationExecution ezFontRenderingApp::Run()
{
  m_pWindow->ProcessWindowMessages();

  if (m_pWindow->m_bCloseRequested || ezInputManager::GetInputActionState("Main", "CloseApp") == ezKeyState::Pressed)
    return ApplicationExecution::Quit;

  // make sure time goes on
  ezClock::GetGlobalClock()->Update();

  // update all input state
  ezInputManager::Update(ezClock::GetGlobalClock()->GetTimeDiff());

<<<<<<< HEAD
  // make sure telemetry is sent out regularly
  ezTelemetry::PerFrameUpdate();

=======
>>>>>>> b79fbf4b8 (Addressed some review comments)
  // do the rendering
  {
    // Before starting to render in a frame call this function
    m_pDevice->BeginFrame();

    // The ezGALContext class is the main interaction point for draw / compute operations
    ezGALContext* pContext = m_pDevice->GetPrimaryContext();

<<<<<<< HEAD
=======
    auto& gc = ezRenderContext::GetDefaultInstance()->WriteGlobalConstants();
    ezMemoryUtils::ZeroFill(&gc, 1);

    gc.WorldToCameraMatrix[0] = m_camera->GetViewMatrix(ezCameraEye::Left);
    gc.WorldToCameraMatrix[1] = m_camera->GetViewMatrix(ezCameraEye::Right);
    gc.CameraToWorldMatrix[0] = gc.WorldToCameraMatrix[0].GetInverse();
    gc.CameraToWorldMatrix[1] = gc.WorldToCameraMatrix[1].GetInverse();
    gc.ViewportSize = ezVec4((float)g_uiWindowWidth, (float)g_uiWindowHeight, 1.0f / (float)g_uiWindowWidth, 1.0f / (float)g_uiWindowHeight);
    // Wrap around to prevent floating point issues. Wrap around is dividable by all whole numbers up to 11.
    gc.GlobalTime = (float)ezMath::Mod(ezClock::GetGlobalClock()->GetAccumulatedTime().GetSeconds(), 20790.0);
    gc.WorldTime = gc.GlobalTime;

>>>>>>> b79fbf4b8 (Addressed some review comments)

    ezGALRenderTargetSetup RTS;
    RTS.SetRenderTarget(0, m_hBBRTV).SetDepthStencilTarget(m_hBBDSV);

    pContext->SetRenderTargetSetup(RTS);
    pContext->SetViewport(ezRectFloat(0.0f, 0.0f, (float)g_uiWindowWidth, (float)g_uiWindowHeight), 0.0f, 1.0f);
    pContext->Clear(ezColor::Black);

<<<<<<< HEAD
    pContext->SetRasterizerState(m_hRasterizerState);
    pContext->SetDepthStencilState(m_hDepthStencilState);

    RenderText();
=======
    ezRenderContext::GetDefaultInstance()->BindMaterial(m_hMaterial);
    ezRenderContext::GetDefaultInstance()->BindMeshBuffer(m_hQuadMeshBuffer);
    ezRenderContext::GetDefaultInstance()->DrawMeshBuffer();


    //RenderText();
    TestMap();

    ezTextSprite textSprite;
    textSprite.Update(m_TextSpriteDesc);

    ezUInt32 numRenderElements = textSprite.GetNumRenderElements();

    for (ezUInt32 i = 0; i < numRenderElements; i++)
    {
      const ezTextSpriteRenderElementData& renderData = textSprite.GetRenderElementData(i);
    }
>>>>>>> b79fbf4b8 (Addressed some review comments)

    m_pDevice->Present(m_pDevice->GetPrimarySwapChain(), true);

    m_pDevice->EndFrame();
<<<<<<< HEAD
=======
    ezRenderContext::GetDefaultInstance()->ResetContextState();
>>>>>>> b79fbf4b8 (Addressed some review comments)
  }

  // needs to be called once per frame
  ezResourceManager::PerFrameUpdate();

  // tell the task system to finish its work for this frame
  // this has to be done at the very end, so that the task system will only use up the time that is left in this frame for
  // uploading GPU data etc.
  ezTaskSystem::FinishFrameTasks();

  return ezApplication::Continue;
}

void ezFontRenderingApp::AfterCoreSystemsStartup()
{
<<<<<<< HEAD
=======
  m_camera = EZ_DEFAULT_NEW(ezCamera);
  m_camera->LookAt(ezVec3(3, 3, 1.5), ezVec3(0, 0, 0), ezVec3(0, 1, 0));
  m_directoryWatcher = EZ_DEFAULT_NEW(ezDirectoryWatcher);

>>>>>>> b79fbf4b8 (Addressed some review comments)
  ezStringBuilder sProjectDir = ">sdk/Data/Samples/FontSample";
  ezStringBuilder sProjectDirResolved;
  ezFileSystem::ResolveSpecialDirectory(sProjectDir, sProjectDirResolved);

  ezFileSystem::SetSpecialDirectory("project", sProjectDirResolved);

<<<<<<< HEAD
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
  ezFileSystem::AddDataDirectory(">user/ezEngine Project/FontSample", "AppData", "appdata",
    ezFileSystem::AllowWrites); // app user data

  ezFileSystem::AddDataDirectory(">sdk/Data/Base", "Base", "base");
  ezFileSystem::AddDataDirectory(">sdk/Data/FreeContent", "Shared", "shared");
=======
  EZ_VERIFY(
    m_directoryWatcher->OpenDirectory(sProjectDirResolved, ezDirectoryWatcher::Watch::Writes | ezDirectoryWatcher::Watch::Subdirectories).Succeeded(),
    "Failed to watch project directory");

  ezFileSystem::AddDataDirectory("", "", ":", ezFileSystem::AllowWrites);
  ezFileSystem::AddDataDirectory(">appdir/", "AppBin", "bin", ezFileSystem::AllowWrites);                                     // writing to the binary directory
  ezFileSystem::AddDataDirectory(">appdir/", "ShaderCache", "shadercache", ezFileSystem::AllowWrites);                        // for shader files
  ezFileSystem::AddDataDirectory(">user/ezEngine Project/FontRenderingApp", "AppData", "appdata", ezFileSystem::AllowWrites); // app user data

  ezFileSystem::AddDataDirectory(">sdk/Data/Base", "Base", "base");
>>>>>>> b79fbf4b8 (Addressed some review comments)
  ezFileSystem::AddDataDirectory(">project/", "Project", "project", ezFileSystem::AllowWrites);

  ezGlobalLog::AddLogWriter(ezLogWriter::Console::LogMessageHandler);
  ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);

<<<<<<< HEAD
  ezTelemetry::CreateServer();
=======
>>>>>>> b79fbf4b8 (Addressed some review comments)
  ezPlugin::LoadPlugin("ezInspectorPlugin");

  EZ_VERIFY(ezPlugin::LoadPlugin("ezShaderCompilerHLSL").Succeeded(), "Compiler Plugin not found");

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
<<<<<<< HEAD
=======
    WindowCreationDesc.m_Title = "Font Rendering";
    WindowCreationDesc.m_bShowMouseCursor = true;
    WindowCreationDesc.m_bClipMouseCursor = false;
    WindowCreationDesc.m_WindowMode = ezWindowMode::WindowResizable;
>>>>>>> b79fbf4b8 (Addressed some review comments)
    m_pWindow = EZ_DEFAULT_NEW(ezFontRenderingWindow);
    m_pWindow->Initialize(WindowCreationDesc);
  }

  // Create a device
  {
    ezGALDeviceCreationDescription DeviceInit;
    DeviceInit.m_bCreatePrimarySwapChain = true;
<<<<<<< HEAD
    DeviceInit.m_bDebugDevice = true;
=======
    DeviceInit.m_bDebugDevice = false; // On Windows 10 this makes device creation fail :-(
>>>>>>> b79fbf4b8 (Addressed some review comments)
    DeviceInit.m_PrimarySwapChainDescription.m_pWindow = m_pWindow;
    DeviceInit.m_PrimarySwapChainDescription.m_SampleCount = ezGALMSAASampleCount::None;
    DeviceInit.m_PrimarySwapChainDescription.m_bAllowScreenshots = true;

    m_pDevice = EZ_DEFAULT_NEW(ezGALDeviceDX11, DeviceInit);
    EZ_VERIFY(m_pDevice->Init() == EZ_SUCCESS, "Device init failed!");

    ezGALDevice::SetDefaultDevice(m_pDevice);
  }

  // now that we have a window and device, tell the engine to initialize the rendering infrastructure
  ezStartup::StartupHighLevelSystems();

<<<<<<< HEAD

=======
>>>>>>> b79fbf4b8 (Addressed some review comments)
  // Get the primary swapchain (this one will always be created by device init except if the user instructs no swap chain creation
  // explicitly)
  {
    ezGALSwapChainHandle hPrimarySwapChain = m_pDevice->GetPrimarySwapChain();
    const ezGALSwapChain* pPrimarySwapChain = m_pDevice->GetSwapChain(hPrimarySwapChain);

    ezGALTextureCreationDescription texDesc;
    texDesc.m_uiWidth = g_uiWindowWidth;
    texDesc.m_uiHeight = g_uiWindowHeight;
    texDesc.m_Format = ezGALResourceFormat::D24S8;
    texDesc.m_bCreateRenderTarget = true;

    m_hDepthStencilTexture = m_pDevice->CreateTexture(texDesc);

    m_hBBRTV = m_pDevice->GetDefaultRenderTargetView(pPrimarySwapChain->GetBackBufferTexture());
    m_hBBDSV = m_pDevice->GetDefaultRenderTargetView(m_hDepthStencilTexture);
  }

<<<<<<< HEAD
  // Create Rasterizer State
  {
    ezGALRasterizerStateCreationDescription RasterStateDesc;
    RasterStateDesc.m_CullMode = ezGALCullMode::Back;
    RasterStateDesc.m_bFrontCounterClockwise = false;
    m_hRasterizerState = m_pDevice->CreateRasterizerState(RasterStateDesc);
    EZ_ASSERT_DEV(!m_hRasterizerState.IsInvalidated(), "Couldn't create rasterizer state!");
  }

  // Create Depth Stencil state
  {
    ezGALDepthStencilStateCreationDescription DepthStencilStateDesc;
    DepthStencilStateDesc.m_bDepthTest = false;
    DepthStencilStateDesc.m_bDepthWrite = false;
    m_hDepthStencilState = m_pDevice->CreateDepthStencilState(DepthStencilStateDesc);
    EZ_ASSERT_DEV(!m_hDepthStencilState.IsInvalidated(), "Couldn't create depth-stencil state!");
  }

  // Setup Shaders and Materials
  {
    ezShaderManager::Configure("DX11_SM50", true);

    m_hFontShader = ezResourceManager::LoadResource<ezShaderResource>("Shaders/Font.ezShader");
  }

  {
    m_hSampleConstants = ezRenderContext::CreateConstantBufferStorage(m_pSampleConstantBuffer);
=======
  // Setup Shaders and Materials
  {
    ezShaderManager::Configure("DX11_SM40", true);

    m_hMaterial = ezResourceManager::LoadResource<ezMaterialResource>("Materials/screen.ezMaterial");

    // Create the mesh that we use for rendering
    CreateScreenQuad();
>>>>>>> b79fbf4b8 (Addressed some review comments)
  }

  m_Font = ezResourceManager::LoadResource<ezFontResource>(":/Fonts/Roboto-Black.ezFont");

<<<<<<< HEAD
  m_TextSpriteDesc.Font = m_Font;

  // Create the vertex buffer
  {
    ezGALBufferCreationDescription desc;
    desc.m_uiStructSize = sizeof(Vertex);
    desc.m_uiTotalSize = VertexBufferSize * desc.m_uiStructSize;
    desc.m_BufferType = ezGALBufferType::VertexBuffer;
    desc.m_ResourceAccess.m_bImmutable = false;

    m_hVertexBuffer = ezGALDevice::GetDefaultDevice()->CreateBuffer(desc);
  }

  // Create the index buffer
  {
    ezGALBufferCreationDescription desc;
    desc.m_uiStructSize = sizeof(ezUInt32);
    desc.m_uiTotalSize = IndexBufferSize * desc.m_uiStructSize;
    desc.m_BufferType = ezGALBufferType::IndexBuffer;
    desc.m_ResourceAccess.m_bImmutable = false;

    m_hIndexBuffer = ezGALDevice::GetDefaultDevice()->CreateBuffer(desc);
  }

  {
    ezVertexStreamInfo& si = m_VertexDeclarationInfo.m_VertexStreams.ExpandAndGetRef();
    si.m_Semantic = ezGALVertexAttributeSemantic::Position;
    si.m_Format = ezGALResourceFormat::XYZFloat;
    si.m_uiOffset = 0;
    si.m_uiElementSize = 12;
  }

  {
    ezVertexStreamInfo& si = m_VertexDeclarationInfo.m_VertexStreams.ExpandAndGetRef();
    si.m_Semantic = ezGALVertexAttributeSemantic::TexCoord0;
    si.m_Format = ezGALResourceFormat::UVFloat;
    si.m_uiOffset = 12;
    si.m_uiElementSize = 8;
  }
}

void ezFontRenderingApp::BeforeCoreSystemsShutdown()
{
  // make sure that no textures are continue to be streamed in while the engine shuts down
  ezResourceManager::EngineAboutToShutdown();

  ezRenderContext::DeleteConstantBufferStorage(m_hSampleConstants);
  m_hSampleConstants.Invalidate();

  if (!m_hVertexBuffer.IsInvalidated())
  {
    ezGALDevice::GetDefaultDevice()->DestroyBuffer(m_hVertexBuffer);
    m_hVertexBuffer.Invalidate();
  }

  if (!m_hIndexBuffer.IsInvalidated())
  {
    ezGALDevice::GetDefaultDevice()->DestroyBuffer(m_hIndexBuffer);
    m_hIndexBuffer.Invalidate();
  }

  m_hFontShader.Invalidate();
  m_Font.Invalidate();
=======
  m_TextSpriteDesc.Anchor = ezTextSpriteAnchor::TopLeft;
  m_TextSpriteDesc.HorizontalAlignment = ezTextHorizontalAlignment::Left;
  m_TextSpriteDesc.VerticalAlignment = ezTextVerticalAlignment::Center;
  m_TextSpriteDesc.Font = m_Font;
  m_TextSpriteDesc.FontSize = 30;
  m_TextSpriteDesc.Width = 100;
  m_TextSpriteDesc.Height = 100;
  m_TextSpriteDesc.Color = ezColor::Red;
  m_TextSpriteDesc.Text = "Hello World";
  m_TextSpriteDesc.WrapText = true;
  m_TextSpriteDesc.BreakTextWhenWrapped = true;
}

void ezFontRenderingApp::BeforeHighLevelSystemsShutdown()
{
  m_directoryWatcher->CloseDirectory();
>>>>>>> b79fbf4b8 (Addressed some review comments)

  m_pDevice->DestroyTexture(m_hDepthStencilTexture);
  m_hDepthStencilTexture.Invalidate();

<<<<<<< HEAD
=======
  m_hMaterial.Invalidate();
  m_hQuadMeshBuffer.Invalidate();

>>>>>>> b79fbf4b8 (Addressed some review comments)
  // tell the engine that we are about to destroy window and graphics device,
  // and that it therefore needs to cleanup anything that depends on that
  ezStartup::ShutdownHighLevelSystems();

<<<<<<< HEAD
  ezResourceManager::FreeAllUnusedResources();

  m_pDevice->DestroyRasterizerState(m_hRasterizerState);
  m_pDevice->DestroyDepthStencilState(m_hDepthStencilState);

=======
>>>>>>> b79fbf4b8 (Addressed some review comments)
  // now we can destroy the graphics device
  m_pDevice->Shutdown();

  EZ_DEFAULT_DELETE(m_pDevice);

  // finally destroy the window
  m_pWindow->Destroy();
  EZ_DEFAULT_DELETE(m_pWindow);
<<<<<<< HEAD
}

void ezFontRenderingApp::RenderText()
{
  ezRenderContext::GetDefaultInstance()->BindConstantBuffer("ezTextureSampleConstants", m_hSampleConstants);

  ezMat4 Proj = ezGraphicsUtils::CreateOrthographicProjectionMatrix(m_vCameraPosition.x + -(float)g_uiWindowWidth * 0.5f,
    m_vCameraPosition.x + (float)g_uiWindowWidth * 0.5f, m_vCameraPosition.y + (float)g_uiWindowHeight * 0.5f,
    m_vCameraPosition.y + -(float)g_uiWindowHeight * 0.5f, -1.0f, 1.0f);

  ezMat4 mTransform;
  mTransform.SetIdentity();

  ezTextSprite textSprite;
  textSprite.Update(m_TextSpriteDesc);
  ezUInt32 numRenderElements = textSprite.GetNumRenderElements();

  ezRenderContext::GetDefaultInstance()->BindShader(m_hFontShader);

  ezDynamicArray<ezVec2> vertexData;
  ezDynamicArray<ezVec2> uvData;
  ezDynamicArray<ezUInt32> indexData;
=======

  m_camera.Clear();
  m_directoryWatcher.Clear();
}

void ezFontRenderingApp::CreateScreenQuad()
{
  ezGeometry geom;
  geom.AddRectXY(ezVec2(2, 2), ezColor::Black);

  ezMeshBufferResourceDescriptor desc;
  desc.AddStream(ezGALVertexAttributeSemantic::Position, ezGALResourceFormat::XYZFloat);

  desc.AllocateStreams(geom.GetVertices().GetCount(), ezGALPrimitiveTopology::Triangles, geom.GetPolygons().GetCount() * 2);

  for (ezUInt32 v = 0; v < geom.GetVertices().GetCount(); ++v)
  {
    desc.SetVertexData<ezVec3>(0, v, geom.GetVertices()[v].m_vPosition);
  }

  ezUInt32 t = 0;
  for (ezUInt32 p = 0; p < geom.GetPolygons().GetCount(); ++p)
  {
    for (ezUInt32 v = 0; v < geom.GetPolygons()[p].m_Vertices.GetCount() - 2; ++v)
    {
      desc.SetTriangleIndices(
        t, geom.GetPolygons()[p].m_Vertices[0], geom.GetPolygons()[p].m_Vertices[v + 1], geom.GetPolygons()[p].m_Vertices[v + 2]);

      ++t;
    }
  }

  m_hQuadMeshBuffer = ezResourceManager::GetExistingResource<ezMeshBufferResource>("{E692442B-9E15-46C5-8A00-1B07C02BF8F7}");

  if (!m_hQuadMeshBuffer.IsValid())
    m_hQuadMeshBuffer = ezResourceManager::CreateResource<ezMeshBufferResource>("{E692442B-9E15-46C5-8A00-1B07C02BF8F7}", std::move(desc));
}

void ezFontRenderingApp::TestMap()
{
  m_TestMap.Insert(1, std::move(TestMapElement(1)));
  m_TestMap.Insert(5, std::move(TestMapElement(5)));
  m_TestMap.Insert(9, std::move(TestMapElement(9)));
  m_TestMap.Insert(12, std::move(TestMapElement(12)));
  m_TestMap.Insert(14, std::move(TestMapElement(14)));
  m_TestMap.Insert(20, std::move(TestMapElement(20)));
  m_TestMap.Insert(30, std::move(TestMapElement(30)));

  EZ_ASSERT_ALWAYS(GetClosestSize(0) == 1, "Fail");
  EZ_ASSERT_ALWAYS(GetClosestSize(1) == 1, "Fail");
  EZ_ASSERT_ALWAYS(GetClosestSize(2) == 1, "Fail");
  EZ_ASSERT_ALWAYS(GetClosestSize(3) == 5, "Fail");
  EZ_ASSERT_ALWAYS(GetClosestSize(10) == 9, "Fail");
  EZ_ASSERT_ALWAYS(GetClosestSize(11) == 12, "Fail");
  EZ_ASSERT_ALWAYS(GetClosestSize(25) == 30, "Fail");
  EZ_ASSERT_ALWAYS(GetClosestSize(40) == 0, "Fail");
}

ezInt32 ezFontRenderingApp::GetClosestSize(ezUInt32 size)
{
  ezUInt32 lowerBoundIndex = m_TestMap.LowerBound(size);

  if (lowerBoundIndex == ezInvalidIndex)
    return 0;

  ezUInt32 lowerBoundSize = m_TestMap.GetValue(lowerBoundIndex).m_Size;

  if (lowerBoundSize == size || lowerBoundIndex == 0)
    return lowerBoundSize;

  ezUInt32 previousSize = m_TestMap.GetValue(lowerBoundIndex - 1).m_Size;

  if (size - previousSize < lowerBoundSize - size)
  {
    return previousSize;
  }

  return lowerBoundSize;
}

void ezFontRenderingApp::RenderText()
{
  ezTextSprite textSprite;
  textSprite.Update(m_TextSpriteDesc);

  ezUInt32 numRenderElements = textSprite.GetNumRenderElements();
>>>>>>> b79fbf4b8 (Addressed some review comments)

  for (ezUInt32 i = 0; i < numRenderElements; i++)
  {
    const ezTextSpriteRenderElementData& renderData = textSprite.GetRenderElementData(i);
<<<<<<< HEAD

    vertexData.SetCount(renderData.m_Vertices.GetCount());
    uvData.SetCount(renderData.m_UVs.GetCount());
    indexData.SetCount(renderData.m_Indices.GetCount());

    textSprite.FillBuffer(vertexData, uvData, indexData, 4, 6, i, ezVec2I32(-30,-25), ezRectI32(), false);

    ezArrayPtr<Vertex> vertices = EZ_NEW_ARRAY(ezFrameAllocator::GetCurrentAllocator(), Vertex, vertexData.GetCount());
    ezArrayPtr<ezUInt32> indices = EZ_NEW_ARRAY(ezFrameAllocator::GetCurrentAllocator(), ezUInt32, indexData.GetCount());

    for (ezUInt32 vertexIndex = 0; vertexIndex < vertexData.GetCount(); vertexIndex++)
    {
      auto& vertex = vertexData[vertexIndex];
      auto& uv = uvData[vertexIndex];

      vertices[vertexIndex] = {{vertex.x, vertex.y, 1.0f}, {uv.x, uv.y}};
    };

    indices.CopyFrom(indexData.GetArrayPtr());

    ezFontSampleConstants& cb = m_pSampleConstantBuffer->GetDataForWriting();

    cb.ModelMatrix = mTransform;
    cb.ViewProjectionMatrix = Proj;

    ezRenderContext::GetDefaultInstance()->BindTexture2D("FontAtlasTexture", renderData.m_hTexture);

    ezRenderContext::GetDefaultInstance()->GetGALContext()->UpdateBuffer(m_hVertexBuffer, 0, ezMakeArrayPtr(vertices.GetPtr(), vertices.GetCount()).ToByteArray());
    ezRenderContext::GetDefaultInstance()->GetGALContext()->UpdateBuffer(m_hIndexBuffer, 0, ezMakeArrayPtr(indices.GetPtr(), indices.GetCount()).ToByteArray());

    ezRenderContext::GetDefaultInstance()->BindMeshBuffer(m_hVertexBuffer, m_hIndexBuffer, &m_VertexDeclarationInfo, ezGALPrimitiveTopology::Triangles, indices.GetCount() / 3);
    ezRenderContext::GetDefaultInstance()->DrawMeshBuffer();
  }
=======
  }

  //if (m_Font.IsValid())
  //{
  //  ezTextSpriteDescriptor desc;

  //  ezTextData textData("Hello World\nWhat is going on?\nTesting 13", m_Font, 30);

  //  ezUInt32 numPages = textData.GetNumPages();

  //  ezUInt32 texturePage = 0;

  //  for (ezUInt32 i = 0; i < numPages; i++)
  //  {
  //    ezUInt32 numQuads = textData.GetNumQuadsForPage(texturePage);
  //    const ezTexture2DResourceHandle& texture = textData.GetTextureForPage(texturePage);
  //  }

  //  ezLog::Info("TextInfo - Width:{0} Height:{1} Line Height:{2} Num Lines:{3} Num Pages:{4}", textData.GetWidth(), textData.GetHeight(), textData.GetLineHeight(), textData.GetNumLines(), textData.GetNumPages());

  //  for (ezUInt32 lineNum = 0; lineNum < textData.GetNumLines(); lineNum++)
  //  {
  //    const ezTextData::ezTextLine& line = textData.GetLine(lineNum);

  //    ezLog::Info("Line Info - Width:{0} Height:{1} YOffset:{2} HasNLC:{3}", line.GetWidth(), line.GetHeight(), line.GetYOffset(), line.HasNewlineCharacter());
  //  }

  //  //ezLog::Info("{} {}", font->GetName(), font->GetFamilyName());
  //}
>>>>>>> b79fbf4b8 (Addressed some review comments)
}

EZ_CONSOLEAPP_ENTRY_POINT(ezFontRenderingApp);
