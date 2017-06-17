﻿#include <PCH.h>
#include "Basics.h"
#include <RendererFoundation/Context/Context.h>
#include <Core/Graphics/Camera.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <RendererCore/Shader/ShaderResource.h>
#include <RendererCore/Textures/TextureCubeResource.h>

ezTestAppRun ezRendererTestBasics::SubtestTextures2D()
{
  BeginFrame();

  ezRenderContext::GetDefaultInstance()->SetDefaultTextureFilter(ezTextureFilterSetting::FixedTrilinear);

  const ezInt32 iNumFrames = 10;

  m_hShader = ezResourceManager::LoadResource<ezShaderResource>("Shaders/Textured.ezShader");

  if (m_iFrame == 0)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_ABGR_Mips_D.dds");
  }

  if (m_iFrame == 1)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_ABGR_NoMips_D.dds");
  }

  if (m_iFrame == 2)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_ARGB_Mips_D.dds");
  }

  if (m_iFrame == 3)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_ARGB_NoMips_D.dds");
  }

  if (m_iFrame == 4)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_DXT1_Mips_D.dds");
  }

  if (m_iFrame == 5)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_DXT1_NoMips_D.dds");
  }

  if (m_iFrame == 6)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_DXT3_Mips_D.dds");
  }

  if (m_iFrame == 7)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_DXT3_NoMips_D.dds");
  }

  if (m_iFrame == 8)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_DXT5_Mips_D.dds");
  }

  if (m_iFrame == 9)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_DXT5_NoMips_D.dds");
  }

  /// \todo All below are disabled, because iNumFrames == 10, don't know why

  if (m_iFrame == 10)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_RGB_Mips_D.dds");
  }

  if (m_iFrame == 11)
  {
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_RGB_NoMips_D.dds");
  }

  if (m_iFrame == 12)
  {
    // My GPU seems not to support this format in general
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_R5G6B5_NoMips_D.dds");
  }

  if (m_iFrame == 13)
  {
    // This format generally does not support mipmaps
    //m_hTexture = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/ezLogo_R5G6B5_MipsD.dds");
  }

  ezRenderContext::GetDefaultInstance()->BindTexture2D(ezGALShaderStage::PixelShader, "DiffuseTexture", m_hTexture2D);

  ClearScreen(ezColor::Black);

  RenderObjects(ezShaderBindFlags::Default);

  EZ_TEST_IMAGE(100);

  EndFrame();

  return m_iFrame < (iNumFrames-1) ? ezTestAppRun::Continue : ezTestAppRun::Quit;
}


ezTestAppRun ezRendererTestBasics::SubtestTextures3D()
{
  BeginFrame();

  ezRenderContext::GetDefaultInstance()->SetDefaultTextureFilter(ezTextureFilterSetting::FixedTrilinear);

  const ezInt32 iNumFrames = 1;

  m_hShader = ezResourceManager::LoadResource<ezShaderResource>("Shaders/TexturedVolume.ezShader");

  if (m_iFrame == 0)
  {
    /// \todo Christian: Error "The image format is neither specified as a pixel mask nor as a FourCC code"
    m_hTexture2D = ezResourceManager::LoadResource<ezTexture2DResource>("Textures/Volume/ezLogo_Volume_A8_NoMips_D.dds");
  }

  ezRenderContext::GetDefaultInstance()->BindTexture2D(ezGALShaderStage::PixelShader, "DiffuseTexture", m_hTexture2D);

  ClearScreen(ezColor::Black);

  RenderObjects(ezShaderBindFlags::Default);

  EZ_TEST_IMAGE(100);

  EndFrame();

  return m_iFrame < (iNumFrames-1) ? ezTestAppRun::Continue : ezTestAppRun::Quit;
}


ezTestAppRun ezRendererTestBasics::SubtestTexturesCube()
{
  BeginFrame();

  ezRenderContext::GetDefaultInstance()->SetDefaultTextureFilter(ezTextureFilterSetting::FixedTrilinear);

  const ezInt32 iNumFrames = 10;

  m_hShader = ezResourceManager::LoadResource<ezShaderResource>("Shaders/TexturedCube.ezShader");

  if (m_iFrame == 0)
  {
    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_XRGB_NoMips_D.dds");
  }

  if (m_iFrame == 1)
  {
    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_XRGB_Mips_D.dds");
  }

  if (m_iFrame == 2)
  {
    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_RGBA_NoMips_D.dds");
  }

  if (m_iFrame == 3)
  {
    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_RGBA_Mips_D.dds");
  }

  if (m_iFrame == 4)
  {
    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_DXT1_NoMips_D.dds");
  }

  if (m_iFrame == 5)
  {
    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_DXT1_Mips_D.dds");
  }

  if (m_iFrame == 6)
  {
    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_DXT3_NoMips_D.dds");
  }

  if (m_iFrame == 7)
  {
    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_DXT3_Mips_D.dds");
  }

  if (m_iFrame == 8)
  {
    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_DXT5_NoMips_D.dds");
  }

  if (m_iFrame == 9)
  {
    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_DXT5_Mips_D.dds");
  }

  if (m_iFrame == 10)
  {
    /// \todo Christian: These formats cannot be loaded

    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_RGB_NoMips_D.dds");
  }

  if (m_iFrame == 11)
  {
    /// \todo Christian: These formats cannot be loaded

    m_hTextureCube = ezResourceManager::LoadResource<ezTextureCubeResource>("Textures/Cubemap/ezLogo_Cube_RGB_Mips_D.dds");
  }

  ezRenderContext::GetDefaultInstance()->BindTextureCube(ezGALShaderStage::PixelShader, "DiffuseTexture", m_hTextureCube);

  ClearScreen(ezColor::Black);

  RenderObjects(ezShaderBindFlags::Default);

  EZ_TEST_IMAGE(100);

  EndFrame();

  return m_iFrame < (iNumFrames-1) ? ezTestAppRun::Continue : ezTestAppRun::Quit;
}

