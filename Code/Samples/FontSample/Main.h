#pragma once

#include <Foundation/Application/Application.h>
#include <Foundation/IO/DirectoryWatcher.h>
#include <Foundation/Types/UniquePtr.h>
#include <RendererCore/Material/MaterialResource.h>
#include <RendererCore/Meshes/MeshResource.h>
#include <RendererFoundation/RendererFoundationDLL.h>
#include <System/Window/Window.h>
#include <RendererCore/Font/FontResource.h>


class ezCamera;
class ezGALDevice;
class ezDirectoryWatcher;

class ezFontRenderingWindow : public ezWindow
{
public:
  ezFontRenderingWindow()
    : ezWindow()
  {
    m_bCloseRequested = false;
  }

  virtual void OnClickClose() override { m_bCloseRequested = true; }

  bool m_bCloseRequested;
};

// A simple application that renders a full screen quad with a single shader and does live reloading of that shader
// Can be used for singed distance rendering experiments or other single shader experiments.
class ezFontRenderingApp : public ezApplication
{
public:
  typedef ezApplication SUPER;

  ezFontRenderingApp();

  virtual ApplicationExecution Run() override;

  virtual void AfterCoreSystemsStartup() override;

  virtual void BeforeHighLevelSystemsShutdown() override;

private:
  void CreateScreenQuad();
  void OnFileChanged(const char* filename, ezDirectoryWatcherAction action);

  ezFontRenderingWindow* m_pWindow = nullptr;
  ezGALDevice* m_pDevice = nullptr;

  ezGALRenderTargetViewHandle m_hBBRTV;
  ezGALRenderTargetViewHandle m_hBBDSV;
  ezGALTextureHandle m_hDepthStencilTexture;

  ezMaterialResourceHandle m_hMaterial;
  ezMeshBufferResourceHandle m_hQuadMeshBuffer;

  ezUniquePtr<ezCamera> m_camera;
  ezUniquePtr<ezDirectoryWatcher> m_directoryWatcher;

  bool m_stuffChanged;
  ezFontResourceHandle m_Font;

  void RenderText();
};
