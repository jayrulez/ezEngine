#pragma once
#include <Core/System/Window.h>
#include <Foundation/Application/Application.h>
#include <Core/Input/InputManager.h>

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

class RHISample : public ezApplication
{
public:
  typedef ezApplication SUPER;

  RHISample()
    : ezApplication("Texture Sample")
  {
  }

  void AfterCoreSystemsStartup() override;


  ApplicationExecution Run() override;

  void CreatePipelineState();

  void OnDraw();
  void Update();

  void BeforeCoreSystemsShutdown() override;


private:
  RHISampleWindow* m_pWindow = nullptr;
  RHIGraphicsDevice* m_pDevice = nullptr;

  RHICommandList* CommandList = nullptr;
  RHIBuffer* VertexBuffer = nullptr;
  RHIBuffer* IndexBuffer = nullptr;
  RHIBuffer* ConstantBuffer = nullptr;
  ezDynamicArray<RHIShader*> Shaders;
  RHIPipeline* Pipeline = nullptr;
  RHIShader* VertexShader = nullptr;
  RHIShader* FragmentShader = nullptr;
  ezMat4 WorldViewProjectionMatrix;
  RHIResourceSet* ResourceSet = nullptr;
  RHIResourceFactory* ResourceFactory = nullptr;
};
