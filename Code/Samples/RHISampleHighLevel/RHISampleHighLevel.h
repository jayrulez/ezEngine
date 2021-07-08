#pragma once

#include <memory>
#include <Foundation/Application/Application.h>
#include <Foundation/Types/UniquePtr.h>

#include <RHIHighLevelRenderer/HighLevelRenderer/RenderDevice.h>
#include <RHI/Swapchain/Swapchain.h>
#include <RHI/Fence/Fence.h>
#include <RHI/Instance/Instance.h>
#include <RHIHighLevelRenderer/HighLevelRenderer/ProgramHolder.h>
#include <RHIShaderCompilerHLSL/Compiler.h>
#include <RHIShaderCompilerHLSL/ShaderReflection.h>

class ezRHISampleWindow;

constexpr uint32_t frame_count = 3;

std::string GetShaderPath(const std::string& shaderFile)
{
  ezStringBuilder projectDirAbsolutePath;
  if (!ezFileSystem::ResolveSpecialDirectory(">project", projectDirAbsolutePath).Succeeded())
  {
    EZ_REPORT_FAILURE("Project directory could not be resolved.");
    return {};
  }

  ezStringBuilder shaderPath(projectDirAbsolutePath, shaderFile.data());
  shaderPath.MakeCleanPath();

  return shaderPath.GetData();
}

struct VertexShaderDesc
{
  static constexpr ShaderType type = ShaderType::kVertex;
  ShaderDesc desc = {GetShaderPath("/shaders/TriangleVertexShader.hlsl"), "main", type, "6_0"};

  struct IA
  {
    static constexpr const uint32_t POSITION = 0;
  } ia;
};

class VertexShader : public VertexShaderDesc
{
public:
  VertexShader(RenderDevice& device, ShaderBlobType shaderBlobType)
    : m_ShaderBlobType{shaderBlobType}
  {
  }

  void CompileShader(RenderDevice& device)
  {
    auto full_desc = desc;
    std::vector<uint8_t> byteCode = Compile(full_desc, m_ShaderBlobType);
    std::shared_ptr<ShaderReflection> reflection = CreateShaderReflection(m_ShaderBlobType, byteCode.data(), byteCode.size());
    shader = device.CreateShader(full_desc, byteCode, reflection);
  }

  std::shared_ptr<Shader> shader;

private:
  ShaderBlobType m_ShaderBlobType;
};

struct PixelShaderDesc
{
  static constexpr ShaderType type = ShaderType::kPixel;
  ShaderDesc desc = {GetShaderPath("/shaders/TrianglePixelShader.hlsl"), "main", type, "6_0"};

  struct CBV
  {
    BindKey Settings;
  } cbv;

  struct OM
  {
    static constexpr const uint32_t rtv0 = 0;
  } om;
};

class PixelShader : public PixelShaderDesc
{
public:
  struct Settings
  {
    ezColor color;
  };
  BufferLayout Settings_layout = {16, {
                                        16,
                                      },
    {
      offsetof(Settings, color),
    },
    {
      0,
    }};
  struct Cbuffer
  {
    Cbuffer(PixelShader& shader, RenderDevice& device)
      : Settings(device, shader.Settings_layout)
    {
    }
    ConstantBuffer<Settings> Settings;
  } cbuffer;

  PixelShader(RenderDevice& device, ShaderBlobType shaderBlobType)
    : cbuffer(*this, device)
    , m_ShaderBlobType{shaderBlobType}
  {
  }

  void CompileShader(RenderDevice& device)
  {
    auto full_desc = desc;
    std::vector<uint8_t> byteCode = Compile(full_desc, m_ShaderBlobType);
    std::shared_ptr<ShaderReflection> reflection = CreateShaderReflection(m_ShaderBlobType, byteCode.data(), byteCode.size());
    shader = device.CreateShader(full_desc, byteCode, reflection);
    cbv.Settings = shader->GetBindKey("Settings");
  }

  std::shared_ptr<Shader> shader;

private:
  ShaderBlobType m_ShaderBlobType;
};

class ezRHISampleApp : public ezApplication
{
public:
  typedef ezApplication SUPER;

  ezRHISampleApp();

  virtual Execution Run() override;

  virtual void AfterCoreSystemsStartup() override;

  virtual void BeforeHighLevelSystemsShutdown() override;

  void OnResize(ezUInt32 width, ezUInt32 height);

private:
  ezRHISampleWindow* m_pWindow = nullptr;
  RenderDeviceDesc renderDeviceDesc;
  std::shared_ptr<RenderDevice> device;
  std::shared_ptr<RenderCommandList> upload_command_list;
  //std::vector<std::shared_ptr<RenderCommandList>> command_lists;
  std::shared_ptr<Resource> index;
  std::shared_ptr<Resource> pos;
  std::shared_ptr<ProgramHolder<PixelShader, VertexShader>> m_program;
};
