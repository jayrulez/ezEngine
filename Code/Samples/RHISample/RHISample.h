#pragma once

#include <Foundation/Application/Application.h>
#include <Foundation/IO/DirectoryWatcher.h>
#include <Foundation/Types/UniquePtr.h>
#include <RHI/Graphics.h>

class ezRHISampleWindow;
class ezRHIGraphicsDevice;

class ezRHISampleApp : public ezApplication
{
public:
  typedef ezApplication SUPER;

  ezRHISampleApp();

  virtual Execution Run() override;

  virtual void AfterCoreSystemsStartup() override;

  virtual void BeforeHighLevelSystemsShutdown() override;

  private:
  ezRHISampleWindow* m_pWindow = nullptr;
  ezRHIGraphicsDevice* m_pDevice = nullptr;
  SwapChain* m_pSwapChain = nullptr;


  BlendState m_BlendState;
  GPUBuffer m_VertexBuffer;
  GPUBuffer m_IndexBuffer;
  GPUBuffer m_ConstantBuffer;
  PipelineState m_Pipeline;
  InputLayout m_InputLayout;
  RasterizerState m_RasterizerState;
  DepthStencilState m_DepthStencilState;
  Sampler m_Sampler;

  Shader* m_pVertexShader = nullptr;
  Shader* m_pPixelShader = nullptr;
  Texture* m_pTexture = nullptr;
};
