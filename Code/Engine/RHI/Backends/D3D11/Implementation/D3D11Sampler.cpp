#include <RHI/Backends/D3D11/D3D11Sampler.h>

 ezString D3D11Sampler::GetName() const
{
  return Name;
}

void D3D11Sampler::SetName(const ezString& name)
{
  Name = name;

  if (DeviceSampler != nullptr)
    DeviceSampler->SetPrivateData(WKPDID_D3DDebugObjectName, name.GetCharacterCount(), name.GetData());
}

 bool D3D11Sampler::IsDisposed() const
{
  return Disposed;
}

 void D3D11Sampler::Dispose()
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

 ID3D11SamplerState* D3D11Sampler::GetDeviceSampler() const
{
  return DeviceSampler;
}

 D3D11Sampler::D3D11Sampler(ID3D11Device* device, const RHISamplerDescription& description)
{
  D3D11_COMPARISON_FUNC comparision = !description.ComparisonKind.has_value() ? D3D11_COMPARISON_NEVER : D3D11Formats::RHIToD3D11ComparisonFunc(description.ComparisonKind.value());
  D3D11_SAMPLER_DESC samplerStateDesc;

  samplerStateDesc.AddressU = D3D11Formats::RHIToD3D11AddressMode(description.AddressModeU);
  samplerStateDesc.AddressV = D3D11Formats::RHIToD3D11AddressMode(description.AddressModeV);
  samplerStateDesc.AddressW = D3D11Formats::RHIToD3D11AddressMode(description.AddressModeW);
  samplerStateDesc.Filter = D3D11Formats::ToD3D11Filter(description.Filter, description.ComparisonKind.has_value());
  samplerStateDesc.MinLOD = (float)description.MinimumLod;
  samplerStateDesc.MaxLOD = (float)description.MaximumLod;
  samplerStateDesc.MaxAnisotropy = description.MaximumAnisotropy;
  samplerStateDesc.ComparisonFunc = comparision;
  samplerStateDesc.MipLODBias = (float)description.LodBias;
  ezColor color = ToRawColor4(description.BorderColor);
  samplerStateDesc.BorderColor[0] = color.r;
  samplerStateDesc.BorderColor[1] = color.g;
  samplerStateDesc.BorderColor[2] = color.b;
  samplerStateDesc.BorderColor[3] = color.a;

  HRESULT hr = device->CreateSamplerState(&samplerStateDesc, &DeviceSampler);
}

 ezColor D3D11Sampler::ToRawColor4(ezEnum<RHISamplerBorderColor> borderColor)
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
      return ezColor::Red;
  }
}


EZ_STATICLINK_FILE(RHI, RHI_Backends_D3D11_Implementation_D3D11Sampler);

