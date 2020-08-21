#pragma once

#include <Foundation/Application/Application.h>
<<<<<<< HEAD
#include <Foundation/Containers/ArrayMap.h>
#include <Foundation/IO/DirectoryWatcher.h>
#include <Foundation/Types/UniquePtr.h>
#include <RendererCore/Font/FontResource.h>
#include <RendererCore/Font/TextSprite.h>
=======
#include <Foundation/IO/DirectoryWatcher.h>
#include <Foundation/Types/UniquePtr.h>
>>>>>>> b79fbf4b8 (Addressed some review comments)
#include <RendererCore/Material/MaterialResource.h>
#include <RendererCore/Meshes/MeshResource.h>
#include <RendererFoundation/RendererFoundationDLL.h>
#include <System/Window/Window.h>
<<<<<<< HEAD

struct ezFontSampleConstants
{
  ezMat4 ModelMatrix;
  ezMat4 ViewProjectionMatrix;
};

struct EZ_ALIGN_16(Vertex)
{
  EZ_DECLARE_POD_TYPE();

  ezVec3 Position;
  ezVec2 TexCoord;
};

class ezGALDevice;
=======
#include <RendererCore/Font/FontResource.h>
#include <RendererCore/Font/TextSprite.h>


class ezCamera;
class ezGALDevice;
class ezDirectoryWatcher;
>>>>>>> b79fbf4b8 (Addressed some review comments)

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

<<<<<<< HEAD
  void BeforeCoreSystemsShutdown() override;

private:
=======
  virtual void BeforeHighLevelSystemsShutdown() override;

private:
  void CreateScreenQuad();
  void OnFileChanged(const char* filename, ezDirectoryWatcherAction action);

>>>>>>> b79fbf4b8 (Addressed some review comments)
  ezFontRenderingWindow* m_pWindow = nullptr;
  ezGALDevice* m_pDevice = nullptr;

  ezGALRenderTargetViewHandle m_hBBRTV;
  ezGALRenderTargetViewHandle m_hBBDSV;
  ezGALTextureHandle m_hDepthStencilTexture;

<<<<<<< HEAD
  ezUniquePtr<ezCamera> m_camera;

  bool m_stuffChanged;
  ezFontResourceHandle m_Font;
  ezShaderResourceHandle m_hFontShader;
  ezTextSpriteDescriptor m_TextSpriteDesc;

  ezConstantBufferStorageHandle m_hSampleConstants;
  ezConstantBufferStorage<ezFontSampleConstants>* m_pSampleConstantBuffer;
  ezVec2 m_vCameraPosition;


  static const ezUInt32 VertexBufferSize = 10000;
  static const ezUInt32 IndexBufferSize = VertexBufferSize * 2;
  ezVertexDeclarationInfo m_VertexDeclarationInfo;
  ezGALBufferHandle m_hVertexBuffer;
  ezGALBufferHandle m_hIndexBuffer;


  ezGALRasterizerStateHandle m_hRasterizerState;
  ezGALDepthStencilStateHandle m_hDepthStencilState;

=======
  ezMaterialResourceHandle m_hMaterial;
  ezMeshBufferResourceHandle m_hQuadMeshBuffer;

  ezUniquePtr<ezCamera> m_camera;
  ezUniquePtr<ezDirectoryWatcher> m_directoryWatcher;

  bool m_stuffChanged;
  ezFontResourceHandle m_Font;
  ezTextSpriteDescriptor m_TextSpriteDesc;

>>>>>>> b79fbf4b8 (Addressed some review comments)
  void RenderText();
};
