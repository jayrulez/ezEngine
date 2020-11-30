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
    ezDynamicArray<ezUInt8> result = CompileCode(description);
    ByteCode.SetCountUninitialized(result.GetCount());
    ByteCode.GetArrayPtr().CopyFrom(result.GetArrayPtr());
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

  ezUInt32 flags = D3DCOMPILE_ENABLE_STRICTNESS;
  flags |= (description.Debug ? D3DCOMPILE_DEBUG : D3DCOMPILE_OPTIMIZATION_LEVEL3);

  ID3DBlob* shaderBlob = nullptr;
  ID3DBlob* errorBlob = nullptr;

  HRESULT hr = D3DCompile(description.ShaderBytes.GetData(), description.ShaderBytes.GetCount(), nullptr, nullptr, nullptr, description.EntryPoint.GetData(), profile.GetData(), flags, 0, &shaderBlob, &errorBlob);

  if (FAILED(hr))
  {
    const char* error = static_cast<const char*>(errorBlob->GetBufferPointer());
    errorBlob->Release();
    EZ_REPORT_FAILURE("Failed to compile HLSL code: {}", error);

    return ezDynamicArray<ezUInt8>();
  }

  ezDynamicArray<ezUInt8> shaderBytes;
  shaderBytes.SetCountUninitialized((ezUInt32)shaderBlob->GetBufferSize());
  ezMemoryUtils::Copy(shaderBytes.GetData(), static_cast<ezUInt8*>(shaderBlob->GetBufferPointer()), shaderBytes.GetCount());
  shaderBlob->Release();

  return shaderBytes;
}



EZ_STATICLINK_FILE(RHI, RHI_Backends_D3D11_Implementation_D3D11Shader);
