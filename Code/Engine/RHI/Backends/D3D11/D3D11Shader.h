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
  virtual ezString GetName() const override;

  virtual void SetName(const ezString& name) override;
  virtual bool IsDisposed() const override;
  virtual void Dispose() override;

public:
  D3D11Shader(ID3D11Device* device, const RHIShaderDescription& description);


  ezDynamicArray<ezUInt8> CompileCode(const RHIShaderDescription& description);

  ID3D11DeviceChild* GetDeviceShader() const;
  ezDynamicArray<ezUInt8> GetByteCode() const;
};
