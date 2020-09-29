#include <Core/Graphics/Geometry.h>
#include <Core/Input/InputManager.h>
#include <Core/ResourceManager/ResourceManager.h>
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
#include <RendererCore/Material/MaterialResource.h>
#include <RendererCore/Meshes/MeshBufferResource.h>
#include <RendererCore/RenderContext/RenderContext.h>
#include <RendererCore/Shader/ShaderResource.h>
#include <RendererCore/ShaderCompiler/ShaderCompiler.h>
#include <RendererCore/ShaderCompiler/ShaderManager.h>
#include <RendererCore/Textures/Texture2DResource.h>
#include <RendererCore/Textures/TextureLoader.h>
#include <RendererDX11/Device/DeviceDX11.h>
#include <RendererFoundation/Context/Context.h>
#include <RendererFoundation/Device/SwapChain.h>
#include <System/Window/Window.h>
#include <Texture/Image/ImageConversion.h>
#include <TGUIPlugin/UISystem.h>

class UISampleWindow : public ezWindow
{
public:
  UISampleWindow()
    : ezWindow()
  {
    m_bCloseRequested = false;
  }

  virtual void OnClickClose() override { m_bCloseRequested = true; }

  bool m_bCloseRequested;
};

static ezUInt32 g_uiWindowWidth = 1280;
static ezUInt32 g_uiWindowHeight = 720;

class UISample : public ezApplication
{
public:
  typedef ezApplication SUPER;

  UISample()
    : ezApplication("UI Sample")
  {
    m_vCameraPosition.SetZero();
  }

  void AfterCoreSystemsStartup() override
  {
    ezStringBuilder sProjectDir = ">sdk/Data/Samples/UISample";
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
    ezFileSystem::AddDataDirectory(">user/ezEngine Project/UISample", "AppData", "appdata",
      ezFileSystem::AllowWrites); // app user data

    ezFileSystem::AddDataDirectory(">sdk/Data/Base", "Base", "base");
    ezFileSystem::AddDataDirectory(">sdk/Data/FreeContent", "Shared", "shared");
    ezFileSystem::AddDataDirectory(">project/", "Project", "project", ezFileSystem::AllowWrites);

    ezGlobalLog::AddLogWriter(ezLogWriter::Console::LogMessageHandler);
    ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);

    ezTelemetry::CreateServer();
    ezPlugin::LoadPlugin("ezInspectorPlugin");
    ezPlugin::LoadPlugin("ezUIPlugin");

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
      m_pWindow = EZ_DEFAULT_NEW(UISampleWindow);
      m_pWindow->Initialize(WindowCreationDesc);
    }

    // Create a device
    {
      ezGALDeviceCreationDescription DeviceInit;
      DeviceInit.m_bCreatePrimarySwapChain = true;
      DeviceInit.m_bDebugDevice = true;
      DeviceInit.m_PrimarySwapChainDescription.m_pWindow = m_pWindow;
      DeviceInit.m_PrimarySwapChainDescription.m_SampleCount = ezGALMSAASampleCount::None;
      DeviceInit.m_PrimarySwapChainDescription.m_bAllowScreenshots = true;

      m_pDevice = EZ_DEFAULT_NEW(ezGALDeviceDX11, DeviceInit);
      EZ_VERIFY(m_pDevice->Init() == EZ_SUCCESS, "Device init failed!");

      ezGALDevice::SetDefaultDevice(m_pDevice);
    }

    // now that we have a window and device, tell the engine to initialize the rendering infrastructure
    ezStartup::StartupHighLevelSystems();


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

    // Create Rasterizer State
    {
      ezGALRasterizerStateCreationDescription RasterStateDesc;
      RasterStateDesc.m_CullMode = ezGALCullMode::Back;
      RasterStateDesc.m_bFrontCounterClockwise = true;
      m_hRasterizerState = m_pDevice->CreateRasterizerState(RasterStateDesc);
      EZ_ASSERT_DEV(!m_hRasterizerState.IsInvalidated(), "Couldn't create rasterizer state!");
    }

    // Create Depth Stencil state
    {
      ezGALDepthStencilStateCreationDescription DepthStencilStateDesc;
      DepthStencilStateDesc.m_bDepthTest = false;
      DepthStencilStateDesc.m_bDepthWrite = true;
      m_hDepthStencilState = m_pDevice->CreateDepthStencilState(DepthStencilStateDesc);
      EZ_ASSERT_DEV(!m_hDepthStencilState.IsInvalidated(), "Couldn't create depth-stencil state!");
    }

    SetupUI();
  }

  static void Login(tgui::EditBox::Ptr username, tgui::EditBox::Ptr password)
  {
    ezLog::Debug("Username: {}", username->getText().c_str());
    ezLog::Debug("Password: {}", password->getText().c_str());
  }

  void SetupUI()
  {
    std::shared_ptr<tgui::Gui> gui = ezUISystem::GetSingleton()->GetGui();

    // Create the username edit box
    // We set a relative position and size
    // In case it isn't obvious, the default text is the text that is displayed when the edit box is empty
    auto editBoxUsername = tgui::EditBox::create();
    editBoxUsername->setSize({"66.67%", "12.5%"});
    editBoxUsername->setPosition({"16.67%", "16.67%"});
    editBoxUsername->setDefaultText("Username");
    gui->add(editBoxUsername);

    // Create the password edit box
    // We copy the previous edit box here and keep the same size
    auto editBoxPassword = tgui::EditBox::copy(editBoxUsername);
    editBoxPassword->setPosition({"16.67%", "41.6%"});
    editBoxPassword->setPasswordCharacter('*');
    editBoxPassword->setDefaultText("Password");
    gui->add(editBoxPassword);

    // Create the login button
    auto button = tgui::Button::create("Login");
    button->setSize({"50%", "16.67%"});
    button->setPosition({"25%", "70%"});
    gui->add(button);

    // Call the login function when the button is pressed and pass the edit boxes that we created as parameters
    // The "&" in front of "login" can be removed on newer compilers, but is kept here for compatibility with GCC < 8.
    button->onPress(&UISample::Login, editBoxUsername, editBoxPassword);
  }

  ApplicationExecution Run() override
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
      m_pDevice->BeginFrame();

      // The ezGALContext class is the main interaction point for draw / compute operations
      ezGALContext* pContext = m_pDevice->GetPrimaryContext();


      ezGALRenderTargetSetup RTS;
      RTS.SetRenderTarget(0, m_hBBRTV).SetDepthStencilTarget(m_hBBDSV);

      pContext->SetRenderTargetSetup(RTS);
      pContext->SetViewport(ezRectFloat(0.0f, 0.0f, (float)g_uiWindowWidth, (float)g_uiWindowHeight), 0.0f, 1.0f);
      pContext->Clear(ezColor::Blue);

      pContext->SetRasterizerState(m_hRasterizerState);
      pContext->SetDepthStencilState(m_hDepthStencilState);

      m_pDevice->Present(m_pDevice->GetPrimarySwapChain(), true);

      m_pDevice->EndFrame();
    }

    // needs to be called once per frame
    ezResourceManager::PerFrameUpdate();

    // tell the task system to finish its work for this frame
    // this has to be done at the very end, so that the task system will only use up the time that is left in this frame for
    // uploading GPU data etc.
    ezTaskSystem::FinishFrameTasks();

    return ezApplication::Continue;
  }

  void BeforeCoreSystemsShutdown() override
  {
    // make sure that no textures are continue to be streamed in while the engine shuts down
    ezResourceManager::EngineAboutToShutdown();

    m_pDevice->DestroyTexture(m_hDepthStencilTexture);
    m_hDepthStencilTexture.Invalidate();

    // tell the engine that we are about to destroy window and graphics device,
    // and that it therefore needs to cleanup anything that depends on that
    ezStartup::ShutdownHighLevelSystems();

    ezResourceManager::FreeAllUnusedResources();

    m_pDevice->DestroyRasterizerState(m_hRasterizerState);
    m_pDevice->DestroyDepthStencilState(m_hDepthStencilState);

    // now we can destroy the graphics device
    m_pDevice->Shutdown();

    EZ_DEFAULT_DELETE(m_pDevice);

    // finally destroy the window
    m_pWindow->Destroy();
    EZ_DEFAULT_DELETE(m_pWindow);
  }

private:
  UISampleWindow* m_pWindow = nullptr;
  ezGALDevice* m_pDevice;

  ezGALRenderTargetViewHandle m_hBBRTV;
  ezGALRenderTargetViewHandle m_hBBDSV;
  ezGALTextureHandle m_hDepthStencilTexture;

  ezGALRasterizerStateHandle m_hRasterizerState;
  ezGALDepthStencilStateHandle m_hDepthStencilState;

  ezVec2 m_vCameraPosition;
};

EZ_CONSOLEAPP_ENTRY_POINT(UISample);
