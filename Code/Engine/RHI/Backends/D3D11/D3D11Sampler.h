#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Descriptors/SamplerDescription.h>
#include <RHI/Resources/Sampler.h>

struct ID3D11SamplerState;

class D3D11Sampler : public RHISampler
{
private:
  ID3D11SamplerState* DeviceSampler = nullptr;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override;

  virtual void SetName(const ezString& name) override;
  virtual bool IsDisposed() const override;
  virtual void Dispose() override;

  ID3D11SamplerState* GetDeviceSampler() const;

  D3D11Sampler(ID3D11Device* device, const RHISamplerDescription& description);

  virtual ezUInt32 GetHash() const override
  {
    ezUInt32 hash = 0;

    // TODO: hash

    return hash;
  }

private:
  static ezColor ToRawColor4(ezEnum<RHISamplerBorderColor> borderColor);
};
