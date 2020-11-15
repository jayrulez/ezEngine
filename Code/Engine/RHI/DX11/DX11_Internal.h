#pragma once

#if EZ_RHI_D3D11_SUPPORTED
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Resources/Resources.h>

#include <DXGI1_3.h>
#include <d3d11_3.h>
#include <wrl/client.h> // ComPtr

using namespace Microsoft::WRL;

namespace DX11_Internal
{
  // Engine -> Native converters

  constexpr ezUInt32 _ParseBindFlags(ezUInt32 value);
  constexpr ezUInt32 _ParseCPUAccessFlags(ezUInt32 value);
  constexpr ezUInt32 _ParseResourceMiscFlags(ezUInt32 value);
  inline ezUInt32 _ParseColorWriteMask(ezBitflags<ezRHIColorWriteMask> value);

  D3D11_FILTER _ConvertFilter(ezEnum<ezRHIFilter> value);
  D3D11_TEXTURE_ADDRESS_MODE _ConvertTextureAddressMode(ezEnum<ezRHITextureAddressMode> value);
  D3D11_COMPARISON_FUNC _ConvertComparisonFunc(ezEnum<ezRHIComparisonFunc> value);
  D3D11_FILL_MODE _ConvertFillMode(ezEnum<ezRHIFillMode> value);
  D3D11_CULL_MODE _ConvertCullMode(ezEnum<ezRHICullMode> value);
  D3D11_DEPTH_WRITE_MASK _ConvertDepthWriteMask(ezEnum<ezRHIDepthWriteMask> value);
  D3D11_STENCIL_OP _ConvertStencilOp(ezEnum<ezRHIStencilOp> value);
  D3D11_BLEND _ConvertBlend(ezEnum<ezRHIBlendFactor> value);
  D3D11_BLEND_OP _ConvertBlendOp(ezEnum<ezRHIBlendOp> value);
  D3D11_USAGE _ConvertUsage(ezBitflags<ezRHIUsage> value);
  D3D11_INPUT_CLASSIFICATION _ConvertInputClassification(ezEnum<ezRHIInputClassification> value);
  DXGI_FORMAT _ConvertFormat(ezEnum<ezRHIFormat> value);

  D3D11_TEXTURE1D_DESC _ConvertTextureDesc1D(const TextureDesc* pDesc);
  D3D11_TEXTURE2D_DESC _ConvertTextureDesc2D(const TextureDesc* pDesc);
  D3D11_TEXTURE3D_DESC _ConvertTextureDesc3D(const TextureDesc* pDesc);
  D3D11_SUBRESOURCE_DATA _ConvertSubresourceData(const SubresourceData& pInitialData);


  // Native -> Engine converters

  constexpr ezUInt32 _ParseBindFlags_Inv(ezUInt32 value);
  constexpr ezUInt32 _ParseCPUAccessFlags_Inv(ezUInt32 value);
  constexpr ezUInt32 _ParseResourceMiscFlags_Inv(ezUInt32 value);

  ezEnum<ezRHIFormat> _ConvertFormat_Inv(DXGI_FORMAT value);
  ezEnum<ezRHIUsage> _ConvertUsage_Inv(D3D11_USAGE value);

  TextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE1D_DESC* pDesc);
  TextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE2D_DESC* pDesc);
  TextureDesc _ConvertTextureDesc_Inv(const D3D11_TEXTURE3D_DESC* pDesc);


  // Local Helpers:
  const void* const __nullBlob[128] = {}; // this is initialized to nullptrs and used to unbind resources!


  struct Resource_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11Resource> resource;
    ComPtr<ID3D11ShaderResourceView> srv;
    ComPtr<ID3D11UnorderedAccessView> uav;
    ezDynamicArray<ComPtr<ID3D11ShaderResourceView>> subresources_srv;
    ezDynamicArray<ComPtr<ID3D11UnorderedAccessView>> subresources_uav;
  };
  struct Texture_DX11 : public Resource_DX11
  {
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<ID3D11DepthStencilView> dsv;
    ezDynamicArray<ComPtr<ID3D11RenderTargetView>> subresources_rtv;
    ezDynamicArray<ComPtr<ID3D11DepthStencilView>> subresources_dsv;
  };
  struct InputLayout_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11InputLayout> resource;
  };
  struct VertexShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11VertexShader> resource;
  };
  struct HullShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11HullShader> resource;
  };
  struct DomainShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11DomainShader> resource;
  };
  struct GeometryShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11GeometryShader> resource;
  };
  struct PixelShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11PixelShader> resource;
  };
  struct ComputeShader_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11ComputeShader> resource;
  };
  struct BlendState_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11BlendState> resource;
  };
  struct DepthStencilState_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11DepthStencilState> resource;
  };
  struct RasterizerState_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11RasterizerState> resource;
  };
  struct Sampler_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11SamplerState> resource;
  };
  struct Query_DX11 : public ezRefCounted
  {
    ComPtr<ID3D11Query> resource;
  };

  Resource_DX11* to_internal(const GPUResource* param);
  Resource_DX11* to_internal(const GPUBuffer* param);
  Texture_DX11* to_internal(const Texture* param);
  InputLayout_DX11* to_internal(const InputLayout* param);
  BlendState_DX11* to_internal(const BlendState* param);
  DepthStencilState_DX11* to_internal(const DepthStencilState* param);
  RasterizerState_DX11* to_internal(const RasterizerState* param);
  Sampler_DX11* to_internal(const Sampler* param);
  Query_DX11* to_internal(const GPUQuery* param);
} // namespace DX11_Internal

#endif
