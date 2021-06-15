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
#include <RendererCore/Material/MaterialResource.h>
#include <RendererCore/Meshes/MeshBufferResource.h>
#include <RendererCore/RenderContext/RenderContext.h>
#include <RendererCore/Shader/ShaderResource.h>
#include <RendererCore/ShaderCompiler/ShaderCompiler.h>
#include <RendererCore/ShaderCompiler/ShaderManager.h>
#include <RendererCore/Textures/Texture2DResource.h>
#include <RendererCore/Textures/TextureLoader.h>
#include <RendererFoundation/CommandEncoder/CommandEncoder.h>
#include <RendererFoundation/Device/DeviceFactory.h>
#include <RendererFoundation/Device/SwapChain.h>
#include <Texture/Image/ImageConversion.h>

// Constant buffer definition is shared between shader code and C++
#include <RendererCore/../../../Data/Samples/VkSample/Shaders/SampleConstantBuffer.h>

class VkSampleWindow : public ezWindow
{
public:
  VkSampleWindow()
    : ezWindow()
  {
    m_bCloseRequested = false;
  }

  virtual void OnClickClose() override { m_bCloseRequested = true; }

  bool m_bCloseRequested;
};

static ezUInt32 g_uiWindowWidth = 1280;
static ezUInt32 g_uiWindowHeight = 720;

class VkSample : public ezApplication
{

public:
  typedef ezApplication SUPER;

  VkSample()
    : ezApplication("Vulkan Sample")
    , m_pDevice{nullptr}
    , m_pWindow{nullptr}
  {
  }

  void AfterCoreSystemsStartup() override
  {
    ezStringBuilder sProjectDir = ">sdk/Data/Samples/VkSample";
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

    ezFileSystem::AddDataDirectory("", "", ":", ezFileSystem::AllowWrites).IgnoreResult();
    ezFileSystem::AddDataDirectory(">appdir/", "AppBin", "bin", ezFileSystem::AllowWrites).IgnoreResult();              // writing to the binary directory
    ezFileSystem::AddDataDirectory(">appdir/", "ShaderCache", "shadercache", ezFileSystem::AllowWrites).IgnoreResult(); // for shader files
    ezFileSystem::AddDataDirectory(">user/ezEngine Project/VkSample", "AppData", "appdata",
      ezFileSystem::AllowWrites)
      .IgnoreResult(); // app user data

    ezFileSystem::AddDataDirectory(">sdk/Data/Base", "Base", "base").IgnoreResult();
    ezFileSystem::AddDataDirectory(">sdk/Data/FreeContent", "Shared", "shared").IgnoreResult();
    ezFileSystem::AddDataDirectory(">project/", "Project", "project", ezFileSystem::AllowWrites).IgnoreResult();

    ezGlobalLog::AddLogWriter(ezLogWriter::Console::LogMessageHandler);
    ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);

    ezTelemetry::CreateServer();
    ezPlugin::LoadPlugin("ezInspectorPlugin").IgnoreResult();

    constexpr const char* szDefaultRenderer = "Vk";

    const char* szRendererName = ezCommandLineUtils::GetGlobalInstance()->GetStringOption("-renderer", 0, szDefaultRenderer);
    const char* szShaderModel = "";
    const char* szShaderCompiler = "";
    ezGALDeviceFactory::GetShaderModelAndCompiler(szRendererName, szShaderModel, szShaderCompiler);

    ezShaderManager::Configure(szShaderModel, true);
    EZ_VERIFY(ezPlugin::LoadPlugin(szShaderCompiler).Succeeded(), "Shader compiler '{}' plugin not found", szShaderCompiler);

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
      m_pWindow = EZ_DEFAULT_NEW(VkSampleWindow);
      m_pWindow->Initialize(WindowCreationDesc).IgnoreResult();
    }

    // Create a device
    {
      ezGALDeviceCreationDescription DeviceInit;
      DeviceInit.m_bCreatePrimarySwapChain = true;
      DeviceInit.m_bDebugDevice = true;
      DeviceInit.m_PrimarySwapChainDescription.m_pWindow = m_pWindow;
      DeviceInit.m_PrimarySwapChainDescription.m_SampleCount = ezGALMSAASampleCount::None;
      DeviceInit.m_PrimarySwapChainDescription.m_bAllowScreenshots = true;

      m_pDevice = ezGALDeviceFactory::CreateDevice(szRendererName, ezFoundation::GetDefaultAllocator(), DeviceInit);
      EZ_ASSERT_DEV(m_pDevice != nullptr, "Device implemention for '{}' not found", szRendererName);
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

      //ezGALTextureCreationDescription texDesc;
      //texDesc.m_uiWidth = g_uiWindowWidth;
      //texDesc.m_uiHeight = g_uiWindowHeight;
      //texDesc.m_Format = ezGALResourceFormat::D24S8;
      //texDesc.m_bCreateRenderTarget = true;

      //m_hDepthStencilTexture = m_pDevice->CreateTexture(texDesc);

      //m_hBBRTV = m_pDevice->GetDefaultRenderTargetView(pPrimarySwapChain->GetBackBufferTexture());
      //m_hBBDSV = m_pDevice->GetDefaultRenderTargetView(m_hDepthStencilTexture);
    }
  }


  Execution Run() override
  {
    m_pWindow->ProcessWindowMessages();

    if (m_pWindow->m_bCloseRequested || ezInputManager::GetInputActionState("Main", "CloseApp") == ezKeyState::Pressed)
      return Execution::Quit;

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

      ezGALPass* pGALPass = m_pDevice->BeginPass("ezVkSampleMainPass");

      ezGALRenderingSetup renderingSetup;
      //renderingSetup.m_RenderTargetSetup.SetRenderTarget(0, m_hBBRTV).SetDepthStencilTarget(m_hBBDSV);
      renderingSetup.m_uiRenderTargetClearMask = 0xFFFFFFFF;

      ezGALRenderCommandEncoder* pCommandEncoder = ezRenderContext::GetDefaultInstance()->BeginRendering(pGALPass, renderingSetup, ezRectFloat(0.0f, 0.0f, (float)g_uiWindowWidth, (float)g_uiWindowHeight));

      ezRenderContext::GetDefaultInstance()->EndRendering();
      m_pDevice->EndPass(pGALPass);

      m_pDevice->Present(m_pDevice->GetPrimarySwapChain(), true);

      m_pDevice->EndFrame();
    }

    // needs to be called once per frame
    ezResourceManager::PerFrameUpdate();

    // tell the task system to finish its work for this frame
    // this has to be done at the very end, so that the task system will only use up the time that is left in this frame for
    // uploading GPU data etc.
    ezTaskSystem::FinishFrameTasks();

    return ezApplication::Execution::Continue;
  }

  void BeforeCoreSystemsShutdown() override
  {
    // make sure that no textures are continue to be streamed in while the engine shuts down
    ezResourceManager::EngineAboutToShutdown();

    // tell the engine that we are about to destroy window and graphics device,
    // and that it therefore needs to cleanup anything that depends on that
    ezStartup::ShutdownHighLevelSystems();

    ezResourceManager::FreeAllUnusedResources();

    // now we can destroy the graphics device
    m_pDevice->Shutdown().IgnoreResult();

    EZ_DEFAULT_DELETE(m_pDevice);

    // finally destroy the window
    m_pWindow->Destroy().IgnoreResult();
    EZ_DEFAULT_DELETE(m_pWindow);
  }

private:
  VkSampleWindow* m_pWindow;
  ezGALDevice* m_pDevice;
};

EZ_CONSOLEAPP_ENTRY_POINT(VkSample);
