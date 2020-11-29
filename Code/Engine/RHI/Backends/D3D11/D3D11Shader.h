#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Shader.h>

struct ID3D11Device;
struct ID3D11DeviceChild;

class D3D11Shader : public RHIShader
{
private:
  ID3D11DeviceChild* DeviceShader = nullptr;
  ezDynamicArray<ezUInt8> ByteCode;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override
  {
    return Name;
  }

  virtual void SetName(const ezString& name) override;
  virtual bool IsDisposed() const override;
  virtual void Dispose() override;

public:
  D3D11Shader(ID3D11Device* device, const RHIShaderDescription& description);


  ezDynamicArray<ezUInt8> CompileCode(const RHIShaderDescription& description);

  ID3D11DeviceChild* GetDeviceShader() const
  {
    return DeviceShader;
  }
  ezDynamicArray<ezUInt8> GetByteCode() const
  {
    return ByteCode;
  }

  virtual ezUInt32 GetHash() const override
  {
    ezUInt32 hash = 0;

    // TODO: hash

    return hash;
  }

private:
  void SetByteCode(ezDynamicArray<ezUInt8> byteCode)
  {
    ByteCode = byteCode;
  }
};
