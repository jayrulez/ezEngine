#include <RHI/Backends/D3D11/D3D11Shader.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

 void D3D11Shader::SetName(const ezString& name)
{

  Name = name;
  if (DeviceShader != nullptr)
    DeviceShader->SetPrivateData(WKPDID_D3DDebugObjectName, name.GetCharacterCount(), name.GetData());
}

bool D3D11Shader::IsDisposed() const
{
  return Disposed;
}

void D3D11Shader::Dispose()
{
  if (!Disposed)
  {
    if (DeviceShader != nullptr)
    {
      DeviceShader->Release();
      DeviceShader = nullptr;
    }
    Disposed = true;
  }
}

D3D11Shader::D3D11Shader(ID3D11Device* device, const RHIShaderDescription& description)
  : RHIShader(description.Stage, description.EntryPoint)
{
  if (description.ShaderBytes.GetCount() > 4 && description.ShaderBytes[0] == 0x44 && description.ShaderBytes[1] == 0x58 && description.ShaderBytes[2] == 0x42 && description.ShaderBytes[3] == 0x43)
  {
    ByteCode.SetCountUninitialized(description.ShaderBytes.GetCount());
    ByteCode.GetArrayPtr().CopyFrom(description.ShaderBytes.GetArrayPtr());
  }
  else
  {
    ByteCode = CompileCode(description);
  }

  HRESULT hr = 0;
  switch (description.Stage)
  {

    case RHIShaderStages::Vertex:
      hr = device->CreateVertexShader(ByteCode.GetData(), ByteCode.GetCount(), nullptr, reinterpret_cast<ID3D11VertexShader**>(&DeviceShader));
      break;
    case RHIShaderStages::Geometry:
      hr = device->CreateGeometryShader(ByteCode.GetData(), ByteCode.GetCount(), nullptr, reinterpret_cast<ID3D11GeometryShader**>(&DeviceShader));
      break;
    case RHIShaderStages::TessellationControl:
      hr = device->CreateHullShader(ByteCode.GetData(), ByteCode.GetCount(), nullptr, reinterpret_cast<ID3D11HullShader**>(&DeviceShader));
      break;
    case RHIShaderStages::TessellationEvaluation:
      hr = device->CreateDomainShader(ByteCode.GetData(), ByteCode.GetCount(), nullptr, reinterpret_cast<ID3D11DomainShader**>(&DeviceShader));
      break;
    case RHIShaderStages::Fragment:
      hr = device->CreatePixelShader(ByteCode.GetData(), ByteCode.GetCount(), nullptr, reinterpret_cast<ID3D11PixelShader**>(&DeviceShader));
      break;
    case RHIShaderStages::Compute:
      hr = device->CreateComputeShader(ByteCode.GetData(), ByteCode.GetCount(), nullptr, reinterpret_cast<ID3D11ComputeShader**>(&DeviceShader));
      break;
    default:
      EZ_REPORT_FAILURE("Invalid RHIShaderStages");
  }
}

ezDynamicArray<ezUInt8> D3D11Shader::CompileCode(const RHIShaderDescription& description)
{
  ezString profile;
  switch (description.Stage)
  {
    case RHIShaderStages::Vertex:
      profile = "vs_5_0";
      break;
    case RHIShaderStages::Geometry:
      profile = "gs_5_0";
      break;
    case RHIShaderStages::TessellationControl:
      profile = "hs_5_0";
      break;
    case RHIShaderStages::TessellationEvaluation:
      profile = "ds_5_0";
      break;
    case RHIShaderStages::Fragment:
      profile = "ps_5_0";
      break;
    case RHIShaderStages::Compute:
      profile = "cs_5_0";
      break;
    default:
      EZ_REPORT_FAILURE("Invalid RHIShaderStages");
  }

  // TODO

  ezUInt32 flags = description.Debug ? D3D10_SHADER_DEBUG : D3D10_SHADER_OPTIMIZATION_LEVEL3;
  //Compiler.Compile(description.ShaderBytes,
  //  description.EntryPoint, null,
  //  profile, out Blob result, out Blob error);

  //if (result == null)
  //{
  //  EZ_REPORT_FAILURE("Failed to compile HLSL code: {Encoding.ASCII.GetString(error.GetBytes())}");
  //}

  //return result.GetBytes();

  return ezDynamicArray<ezUInt8>();
}

