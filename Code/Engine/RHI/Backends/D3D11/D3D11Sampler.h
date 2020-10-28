#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/Sampler.h>
#include <d3d11_1.h>

class D3D11Sampler : public RHISampler
{
private:
  ID3D11SamplerState* DeviceSampler = nullptr;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override { return Name; }

  virtual void SetName(const ezString& name) override
  {
    Name = name;

    if (DeviceSampler != nullptr)
      DeviceSampler->SetPrivateData(WKPDID_D3DDebugObjectName, name.GetCharacterCount(), name.GetData());
  }
  virtual bool IsDisposed() const override { return Disposed; }
  virtual void Dispose() override
  {
    if (!Disposed)
    {
      if (DeviceSampler != nullptr)
      {
        DeviceSampler->Release();
        DeviceSampler = nullptr;
      }
      Disposed = true;
    }
  }

  ID3D11SamplerState* GetDeviceSampler() const
  {
    return DeviceSampler;
  }

  D3D11Sampler(ID3D11Device* device, const RHISamplerDescription& description)
  {
    D3D11_COMPARISON_FUNC comparision = !description.ComparisonKind.has_value() ? D3D11_COMPARISON_NEVER : D3D11FormatUtils::RHIToD3D11ComparisonFunc(description.ComparisonKind.value());
    D3D11_SAMPLER_DESC samplerStateDesc;

    samplerStateDesc.AddressU = D3D11FormatUtils::RHIToD3D11AddressMode(description.AddressModeU);
    samplerStateDesc.AddressV = D3D11FormatUtils::RHIToD3D11AddressMode(description.AddressModeV);
    samplerStateDesc.AddressW = D3D11FormatUtils::RHIToD3D11AddressMode(description.AddressModeW);
    samplerStateDesc.Filter = D3D11FormatUtils::ToD3D11Filter(description.Filter, description.ComparisonKind.has_value());
    samplerStateDesc.MinLOD = description.MinimumLod;
    samplerStateDesc.MaxLOD = description.MaximumLod;
    samplerStateDesc.MaxAnisotropy = (int)description.MaximumAnisotropy;
    samplerStateDesc.ComparisonFunc = comparision;
    samplerStateDesc.MipLODBias = description.LodBias;
    ezColor color = ToRawColor4(description.BorderColor);
    samplerStateDesc.BorderColor[0] = color.r;
    samplerStateDesc.BorderColor[1] = color.g;
    samplerStateDesc.BorderColor[2] = color.b;
    samplerStateDesc.BorderColor[3] = color.a;

    HRESULT hr = device->CreateSamplerState(&samplerStateDesc, &DeviceSampler);
  }

private:
  static ezColor ToRawColor4(ezEnum<RHISamplerBorderColor> borderColor)
  {
    switch (borderColor)
    {
      case RHISamplerBorderColor::TransparentBlack:
        return ezColor(0, 0, 0, 0);
      case RHISamplerBorderColor::OpaqueBlack:
        return ezColor(0, 0, 0, 1);
      case RHISamplerBorderColor::OpaqueWhite:
        return ezColor(1, 1, 1, 1);
      default:
        EZ_REPORT_FAILURE("Invalid RHISamplerBorderColor");
        return ezColor::Blue;
    }
  }
};
