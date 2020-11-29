#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/TextureView.h>

struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;

class D3D11TextureView : public RHITextureView
{
private:
  ID3D11ShaderResourceView* ShaderResourceView = nullptr;
  ID3D11UnorderedAccessView* UnorderedAccessView = nullptr;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override;
  virtual void SetName(const ezString& name) override;
  virtual bool IsDisposed() const override;
  virtual void Dispose() override;

public:
  D3D11TextureView(D3D11GraphicsDevice* graphicsDevice, const RHITextureViewDescription& description);

  ID3D11ShaderResourceView* GetShaderResourceView() const;

  ID3D11UnorderedAccessView* GetUnorderedAccessView() const;

  virtual ezUInt32 GetHash() const override
  {
    ezUInt32 hash = 0;

    // TODO: hash

    return hash;
  }
};
