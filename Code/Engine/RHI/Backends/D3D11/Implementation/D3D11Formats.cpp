#include <RHI/Backends/D3D11/D3D11Formats.h>

namespace D3D11Formats
{
  DXGI_FORMAT ToDxgiFormat(ezEnum<RHIPixelFormat> format, bool depthFormat)
  {
    switch (format)
    {
      case RHIPixelFormat::R8_UNorm:
        return DXGI_FORMAT_R8_UNORM;
      case RHIPixelFormat::R8_SNorm:
        return DXGI_FORMAT_R8_SNORM;
      case RHIPixelFormat::R8_UInt:
        return DXGI_FORMAT_R8_UINT;
      case RHIPixelFormat::R8_SInt:
        return DXGI_FORMAT_R8_SINT;

      case RHIPixelFormat::R16_UNorm:
        return depthFormat ? DXGI_FORMAT_R16_TYPELESS : DXGI_FORMAT_R16_UNORM;
      case RHIPixelFormat::R16_SNorm:
        return DXGI_FORMAT_R16_SNORM;
      case RHIPixelFormat::R16_UInt:
        return DXGI_FORMAT_R16_UINT;
      case RHIPixelFormat::R16_SInt:
        return DXGI_FORMAT_R16_SINT;
      case RHIPixelFormat::R16_Float:
        return DXGI_FORMAT_R16_FLOAT;

      case RHIPixelFormat::R32_UInt:
        return DXGI_FORMAT_R32_UINT;
      case RHIPixelFormat::R32_SInt:
        return DXGI_FORMAT_R32_SINT;
      case RHIPixelFormat::R32_Float:
        return depthFormat ? DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_R32_FLOAT;

      case RHIPixelFormat::R8_G8_UNorm:
        return DXGI_FORMAT_R8G8_UNORM;
      case RHIPixelFormat::R8_G8_SNorm:
        return DXGI_FORMAT_R8G8_SNORM;
      case RHIPixelFormat::R8_G8_UInt:
        return DXGI_FORMAT_R8G8_UINT;
      case RHIPixelFormat::R8_G8_SInt:
        return DXGI_FORMAT_R8G8_SINT;

      case RHIPixelFormat::R16_G16_UNorm:
        return DXGI_FORMAT_R16G16_UNORM;
      case RHIPixelFormat::R16_G16_SNorm:
        return DXGI_FORMAT_R16G16_SNORM;
      case RHIPixelFormat::R16_G16_UInt:
        return DXGI_FORMAT_R16G16_UINT;
      case RHIPixelFormat::R16_G16_SInt:
        return DXGI_FORMAT_R16G16_SINT;
      case RHIPixelFormat::R16_G16_Float:
        return DXGI_FORMAT_R16G16_FLOAT;

      case RHIPixelFormat::R32_G32_UInt:
        return DXGI_FORMAT_R32G32_UINT;
      case RHIPixelFormat::R32_G32_SInt:
        return DXGI_FORMAT_R32G32_SINT;
      case RHIPixelFormat::R32_G32_Float:
        return DXGI_FORMAT_R32G32_FLOAT;

      case RHIPixelFormat::R8_G8_B8_A8_UNorm:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
      case RHIPixelFormat::R8_G8_B8_A8_UNorm_SRgb:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
      case RHIPixelFormat::B8_G8_R8_A8_UNorm:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
      case RHIPixelFormat::B8_G8_R8_A8_UNorm_SRgb:
        return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
      case RHIPixelFormat::R8_G8_B8_A8_SNorm:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
      case RHIPixelFormat::R8_G8_B8_A8_UInt:
        return DXGI_FORMAT_R8G8B8A8_UINT;
      case RHIPixelFormat::R8_G8_B8_A8_SInt:
        return DXGI_FORMAT_R8G8B8A8_SINT;

      case RHIPixelFormat::R16_G16_B16_A16_UNorm:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
      case RHIPixelFormat::R16_G16_B16_A16_SNorm:
        return DXGI_FORMAT_R16G16B16A16_SNORM;
      case RHIPixelFormat::R16_G16_B16_A16_UInt:
        return DXGI_FORMAT_R16G16B16A16_UINT;
      case RHIPixelFormat::R16_G16_B16_A16_SInt:
        return DXGI_FORMAT_R16G16B16A16_SINT;
      case RHIPixelFormat::R16_G16_B16_A16_Float:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;

      case RHIPixelFormat::R32_G32_B32_A32_UInt:
        return DXGI_FORMAT_R32G32B32A32_UINT;
      case RHIPixelFormat::R32_G32_B32_A32_SInt:
        return DXGI_FORMAT_R32G32B32A32_SINT;
      case RHIPixelFormat::R32_G32_B32_A32_Float:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;

      case RHIPixelFormat::BC1_Rgb_UNorm:
      case RHIPixelFormat::BC1_Rgba_UNorm:
        return DXGI_FORMAT_BC1_UNORM;
      case RHIPixelFormat::BC1_Rgb_UNorm_SRgb:
      case RHIPixelFormat::BC1_Rgba_UNorm_SRgb:
        return DXGI_FORMAT_BC1_UNORM_SRGB;
      case RHIPixelFormat::BC2_UNorm:
        return DXGI_FORMAT_BC2_UNORM;
      case RHIPixelFormat::BC2_UNorm_SRgb:
        return DXGI_FORMAT_BC2_UNORM_SRGB;
      case RHIPixelFormat::BC3_UNorm:
        return DXGI_FORMAT_BC3_UNORM;
      case RHIPixelFormat::BC3_UNorm_SRgb:
        return DXGI_FORMAT_BC3_UNORM_SRGB;
      case RHIPixelFormat::BC4_UNorm:
        return DXGI_FORMAT_BC4_UNORM;
      case RHIPixelFormat::BC4_SNorm:
        return DXGI_FORMAT_BC4_SNORM;
      case RHIPixelFormat::BC5_UNorm:
        return DXGI_FORMAT_BC5_UNORM;
      case RHIPixelFormat::BC5_SNorm:
        return DXGI_FORMAT_BC5_SNORM;
      case RHIPixelFormat::BC7_UNorm:
        return DXGI_FORMAT_BC7_UNORM;
      case RHIPixelFormat::BC7_UNorm_SRgb:
        return DXGI_FORMAT_BC7_UNORM_SRGB;

      case RHIPixelFormat::D24_UNorm_S8_UInt:
        EZ_ASSERT_ALWAYS(depthFormat, "");
        return DXGI_FORMAT_R24G8_TYPELESS;
      case RHIPixelFormat::D32_Float_S8_UInt:
        EZ_ASSERT_ALWAYS(depthFormat, "");
        return DXGI_FORMAT_R32G8X24_TYPELESS;

      case RHIPixelFormat::R10_G10_B10_A2_UNorm:
        return DXGI_FORMAT_R10G10B10A2_UNORM;
      case RHIPixelFormat::R10_G10_B10_A2_UInt:
        return DXGI_FORMAT_R10G10B10A2_UINT;
      case RHIPixelFormat::R11_G11_B10_Float:
        return DXGI_FORMAT_R11G11B10_FLOAT;

      case RHIPixelFormat::ETC2_R8_G8_B8_UNorm:
      case RHIPixelFormat::ETC2_R8_G8_B8_A1_UNorm:
      case RHIPixelFormat::ETC2_R8_G8_B8_A8_UNorm:
        EZ_REPORT_FAILURE("ETC2 formats are not supported on Direct3D 11.");
        return DXGI_FORMAT_UNKNOWN;

      default:
        EZ_REPORT_FAILURE("Invalid pixel format");
        return DXGI_FORMAT_UNKNOWN;
    }
  }

  DXGI_FORMAT GetTypelessFormat(DXGI_FORMAT format)
  {
    switch (format)
    {
      case DXGI_FORMAT_R32G32B32A32_TYPELESS:
      case DXGI_FORMAT_R32G32B32A32_FLOAT:
      case DXGI_FORMAT_R32G32B32A32_UINT:
      case DXGI_FORMAT_R32G32B32A32_SINT:
        return DXGI_FORMAT_R32G32B32A32_TYPELESS;
      case DXGI_FORMAT_R32G32B32_TYPELESS:
      case DXGI_FORMAT_R32G32B32_FLOAT:
      case DXGI_FORMAT_R32G32B32_UINT:
      case DXGI_FORMAT_R32G32B32_SINT:
        return DXGI_FORMAT_R32G32B32_TYPELESS;
      case DXGI_FORMAT_R16G16B16A16_TYPELESS:
      case DXGI_FORMAT_R16G16B16A16_FLOAT:
      case DXGI_FORMAT_R16G16B16A16_UNORM:
      case DXGI_FORMAT_R16G16B16A16_UINT:
      case DXGI_FORMAT_R16G16B16A16_SNORM:
      case DXGI_FORMAT_R16G16B16A16_SINT:
        return DXGI_FORMAT_R16G16B16A16_TYPELESS;
      case DXGI_FORMAT_R32G32_TYPELESS:
      case DXGI_FORMAT_R32G32_FLOAT:
      case DXGI_FORMAT_R32G32_UINT:
      case DXGI_FORMAT_R32G32_SINT:
        return DXGI_FORMAT_R32G32_TYPELESS;
      case DXGI_FORMAT_R10G10B10A2_TYPELESS:
      case DXGI_FORMAT_R10G10B10A2_UNORM:
      case DXGI_FORMAT_R10G10B10A2_UINT:
        return DXGI_FORMAT_R10G10B10A2_TYPELESS;
      case DXGI_FORMAT_R8G8B8A8_TYPELESS:
      case DXGI_FORMAT_R8G8B8A8_UNORM:
      case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
      case DXGI_FORMAT_R8G8B8A8_UINT:
      case DXGI_FORMAT_R8G8B8A8_SNORM:
      case DXGI_FORMAT_R8G8B8A8_SINT:
        return DXGI_FORMAT_R8G8B8A8_TYPELESS;
      case DXGI_FORMAT_R16G16_TYPELESS:
      case DXGI_FORMAT_R16G16_FLOAT:
      case DXGI_FORMAT_R16G16_UNORM:
      case DXGI_FORMAT_R16G16_UINT:
      case DXGI_FORMAT_R16G16_SNORM:
      case DXGI_FORMAT_R16G16_SINT:
        return DXGI_FORMAT_R16G16_TYPELESS;
      case DXGI_FORMAT_R32_TYPELESS:
      case DXGI_FORMAT_D32_FLOAT:
      case DXGI_FORMAT_R32_FLOAT:
      case DXGI_FORMAT_R32_UINT:
      case DXGI_FORMAT_R32_SINT:
        return DXGI_FORMAT_R32_TYPELESS;
      case DXGI_FORMAT_R24G8_TYPELESS:
      case DXGI_FORMAT_D24_UNORM_S8_UINT:
      case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
      case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return DXGI_FORMAT_R24G8_TYPELESS;
      case DXGI_FORMAT_R8G8_TYPELESS:
      case DXGI_FORMAT_R8G8_UNORM:
      case DXGI_FORMAT_R8G8_UINT:
      case DXGI_FORMAT_R8G8_SNORM:
      case DXGI_FORMAT_R8G8_SINT:
        return DXGI_FORMAT_R8G8_TYPELESS;
      case DXGI_FORMAT_R16_TYPELESS:
      case DXGI_FORMAT_R16_FLOAT:
      case DXGI_FORMAT_D16_UNORM:
      case DXGI_FORMAT_R16_UNORM:
      case DXGI_FORMAT_R16_UINT:
      case DXGI_FORMAT_R16_SNORM:
      case DXGI_FORMAT_R16_SINT:
        return DXGI_FORMAT_R16_TYPELESS;
      case DXGI_FORMAT_R8_TYPELESS:
      case DXGI_FORMAT_R8_UNORM:
      case DXGI_FORMAT_R8_UINT:
      case DXGI_FORMAT_R8_SNORM:
      case DXGI_FORMAT_R8_SINT:
      case DXGI_FORMAT_A8_UNORM:
        return DXGI_FORMAT_R8_TYPELESS;
      case DXGI_FORMAT_BC1_TYPELESS:
      case DXGI_FORMAT_BC1_UNORM:
      case DXGI_FORMAT_BC1_UNORM_SRGB:
        return DXGI_FORMAT_BC1_TYPELESS;
      case DXGI_FORMAT_BC2_TYPELESS:
      case DXGI_FORMAT_BC2_UNORM:
      case DXGI_FORMAT_BC2_UNORM_SRGB:
        return DXGI_FORMAT_BC2_TYPELESS;
      case DXGI_FORMAT_BC3_TYPELESS:
      case DXGI_FORMAT_BC3_UNORM:
      case DXGI_FORMAT_BC3_UNORM_SRGB:
        return DXGI_FORMAT_BC3_TYPELESS;
      case DXGI_FORMAT_BC4_TYPELESS:
      case DXGI_FORMAT_BC4_UNORM:
      case DXGI_FORMAT_BC4_SNORM:
        return DXGI_FORMAT_BC4_TYPELESS;
      case DXGI_FORMAT_BC5_TYPELESS:
      case DXGI_FORMAT_BC5_UNORM:
      case DXGI_FORMAT_BC5_SNORM:
        return DXGI_FORMAT_BC5_TYPELESS;
      case DXGI_FORMAT_B8G8R8A8_TYPELESS:
      case DXGI_FORMAT_B8G8R8A8_UNORM:
      case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_TYPELESS;
      case DXGI_FORMAT_BC7_TYPELESS:
      case DXGI_FORMAT_BC7_UNORM:
      case DXGI_FORMAT_BC7_UNORM_SRGB:
        return DXGI_FORMAT_BC7_TYPELESS;
      default:
        return format;
    }
  }

  D3D11_BIND_FLAG RHIToD3D11BindFlags(ezBitflags<RHIBufferUsage> usage)
  {
    ezUInt32 flags;

    if ((usage & RHIBufferUsage::VertexBuffer) == RHIBufferUsage::VertexBuffer)
    {
      flags |= D3D11_BIND_VERTEX_BUFFER;
    }
    if ((usage & RHIBufferUsage::IndexBuffer) == RHIBufferUsage::IndexBuffer)
    {
      flags |= D3D11_BIND_INDEX_BUFFER;
    }
    if ((usage & RHIBufferUsage::UniformBuffer) == RHIBufferUsage::UniformBuffer)
    {
      flags |= D3D11_BIND_CONSTANT_BUFFER;
    }
    if ((usage & RHIBufferUsage::StructuredBufferReadOnly) == RHIBufferUsage::StructuredBufferReadOnly || (usage & RHIBufferUsage::StructuredBufferReadWrite) == RHIBufferUsage::StructuredBufferReadWrite)
    {
      flags |= D3D11_BIND_SHADER_RESOURCE;
    }
    if ((usage & RHIBufferUsage::StructuredBufferReadWrite) == RHIBufferUsage::StructuredBufferReadWrite)
    {
      flags |= D3D11_BIND_UNORDERED_ACCESS;
    }

    return (D3D11_BIND_FLAG)flags;
  }

  ezBitflags<RHITextureUsage> GetRHIUsage(D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG cpuFlags, D3D11_RESOURCE_MISC_FLAG optionFlags)
  {
    ezBitflags<RHITextureUsage> usage;

    if ((bindFlags & D3D11_BIND_RENDER_TARGET) != 0)
    {
      usage |= RHITextureUsage::RenderTarget;
    }
    if ((bindFlags & D3D11_BIND_DEPTH_STENCIL) != 0)
    {
      usage |= RHITextureUsage::DepthStencil;
    }
    if ((bindFlags & D3D11_BIND_SHADER_RESOURCE) != 0)
    {
      usage |= RHITextureUsage::Sampled;
    }
    if ((bindFlags & D3D11_BIND_UNORDERED_ACCESS) != 0)
    {
      usage |= RHITextureUsage::Storage;
    }

    if ((optionFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) != 0)
    {
      usage |= RHITextureUsage::Cubemap;
    }
    if ((optionFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS) != 0)
    {
      usage |= RHITextureUsage::GenerateMipmaps;
    }

    return usage;
  }

  bool IsUnsupportedFormat(ezEnum<RHIPixelFormat> format)
  {
    return format == RHIPixelFormat::ETC2_R8_G8_B8_UNorm || format == RHIPixelFormat::ETC2_R8_G8_B8_A1_UNorm || format == RHIPixelFormat::ETC2_R8_G8_B8_A8_UNorm;
  }

  DXGI_FORMAT GetViewFormat(DXGI_FORMAT format)
  {
    switch (format)
    {
      case DXGI_FORMAT_R16_TYPELESS:
        return DXGI_FORMAT_R16_UNORM;
      case DXGI_FORMAT_R32_TYPELESS:
        return DXGI_FORMAT_R32_FLOAT;
      case DXGI_FORMAT_R32G8X24_TYPELESS:
        return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
      case DXGI_FORMAT_R24G8_TYPELESS:
        return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
      default:
        return format;
    }
  }

  D3D11_BLEND RHIToD3D11Blend(ezEnum<RHIBlendFactor> factor)
  {
    switch (factor)
    {
      case RHIBlendFactor::Zero:
        return D3D11_BLEND_ZERO;
      case RHIBlendFactor::One:
        return D3D11_BLEND_ONE;
      case RHIBlendFactor::SourceAlpha:
        return D3D11_BLEND_SRC_ALPHA;
      case RHIBlendFactor::InverseSourceAlpha:
        return D3D11_BLEND_INV_SRC_ALPHA;
      case RHIBlendFactor::DestinationAlpha:
        return D3D11_BLEND_DEST_ALPHA;
      case RHIBlendFactor::InverseDestinationAlpha:
        return D3D11_BLEND_INV_DEST_ALPHA;
      case RHIBlendFactor::SourceColor:
        return D3D11_BLEND_SRC_COLOR;
      case RHIBlendFactor::InverseSourceColor:
        return D3D11_BLEND_INV_SRC_COLOR;
      case RHIBlendFactor::DestinationColor:
        return D3D11_BLEND_DEST_COLOR;
      case RHIBlendFactor::InverseDestinationColor:
        return D3D11_BLEND_INV_DEST_COLOR;
      case RHIBlendFactor::BlendFactor:
        return D3D11_BLEND_BLEND_FACTOR;
      case RHIBlendFactor::InverseBlendFactor:
        return D3D11_BLEND_INV_BLEND_FACTOR;
      default:
        EZ_REPORT_FAILURE("Invalid blend factor");
        return D3D11_BLEND_ZERO;
    }
  }

  DXGI_FORMAT ToDxgiFormat(ezEnum<RHIIndexFormat> format)
  {
    switch (format)
    {
      case RHIIndexFormat::UInt16:
        return DXGI_FORMAT_R16_UINT;

      case RHIIndexFormat::UInt32:
        return DXGI_FORMAT_R32_UINT;

      default:
        EZ_REPORT_FAILURE("Invalid index format");
        return DXGI_FORMAT_R16_UINT;
    }
  }

  D3D11_STENCIL_OP RHIToD3D11StencilOperation(ezEnum<RHIStencilOperation> op)
  {
    switch (op)
    {
      case RHIStencilOperation::Keep:
        return D3D11_STENCIL_OP_KEEP;

      case RHIStencilOperation::Zero:
        return D3D11_STENCIL_OP_ZERO;

      case RHIStencilOperation::Replace:
        return D3D11_STENCIL_OP_REPLACE;

      case RHIStencilOperation::IncrementAndClamp:
        return D3D11_STENCIL_OP_INCR_SAT;

      case RHIStencilOperation::DecrementAndClamp:
        return D3D11_STENCIL_OP_DECR_SAT;

      case RHIStencilOperation::Invert:
        return D3D11_STENCIL_OP_INVERT;

      case RHIStencilOperation::IncrementAndWrap:

        return D3D11_STENCIL_OP_INCR;

      case RHIStencilOperation::DecrementAndWrap:
        return D3D11_STENCIL_OP_DECR;

      default:
        EZ_REPORT_FAILURE("Invalid stencil operation.");
        return D3D11_STENCIL_OP_KEEP;
    }
  }

  ezEnum<RHIPixelFormat> ToRHIFormat(DXGI_FORMAT format)
  {
    switch (format)
    {
      case DXGI_FORMAT_R8_UNORM:
        return RHIPixelFormat::R8_UNorm;
      case DXGI_FORMAT_R8_SNORM:
        return RHIPixelFormat::R8_SNorm;
      case DXGI_FORMAT_R8_UINT:
        return RHIPixelFormat::R8_UInt;
      case DXGI_FORMAT_R8_SINT:
        return RHIPixelFormat::R8_SInt;

      case DXGI_FORMAT_R16_UNORM:
      case DXGI_FORMAT_D16_UNORM:
        return RHIPixelFormat::R16_UNorm;
      case DXGI_FORMAT_R16_SNORM:
        return RHIPixelFormat::R16_SNorm;
      case DXGI_FORMAT_R16_UINT:
        return RHIPixelFormat::R16_UInt;
      case DXGI_FORMAT_R16_SINT:
        return RHIPixelFormat::R16_SInt;
      case DXGI_FORMAT_R16_FLOAT:
        return RHIPixelFormat::R16_Float;

      case DXGI_FORMAT_R32_UINT:
        return RHIPixelFormat::R32_UInt;
      case DXGI_FORMAT_R32_SINT:
        return RHIPixelFormat::R32_SInt;
      case DXGI_FORMAT_R32_FLOAT:
      case DXGI_FORMAT_D32_FLOAT:
        return RHIPixelFormat::R32_Float;

      case DXGI_FORMAT_R8G8_UNORM:
        return RHIPixelFormat::R8_G8_UNorm;
      case DXGI_FORMAT_R8G8_SNORM:
        return RHIPixelFormat::R8_G8_SNorm;
      case DXGI_FORMAT_R8G8_UINT:
        return RHIPixelFormat::R8_G8_UInt;
      case DXGI_FORMAT_R8G8_SINT:
        return RHIPixelFormat::R8_G8_SInt;

      case DXGI_FORMAT_R16G16_UNORM:
        return RHIPixelFormat::R16_G16_UNorm;
      case DXGI_FORMAT_R16G16_SNORM:
        return RHIPixelFormat::R16_G16_SNorm;
      case DXGI_FORMAT_R16G16_UINT:
        return RHIPixelFormat::R16_G16_UInt;
      case DXGI_FORMAT_R16G16_SINT:
        return RHIPixelFormat::R16_G16_SInt;
      case DXGI_FORMAT_R16G16_FLOAT:
        return RHIPixelFormat::R16_G16_Float;

      case DXGI_FORMAT_R32G32_UINT:
        return RHIPixelFormat::R32_G32_UInt;
      case DXGI_FORMAT_R32G32_SINT:
        return RHIPixelFormat::R32_G32_SInt;
      case DXGI_FORMAT_R32G32_FLOAT:
        return RHIPixelFormat::R32_G32_Float;

      case DXGI_FORMAT_R8G8B8A8_UNORM:
        return RHIPixelFormat::R8_G8_B8_A8_UNorm;
      case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return RHIPixelFormat::R8_G8_B8_A8_UNorm_SRgb;

      case DXGI_FORMAT_B8G8R8A8_UNORM:
        return RHIPixelFormat::B8_G8_R8_A8_UNorm;
      case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return RHIPixelFormat::B8_G8_R8_A8_UNorm_SRgb;
      case DXGI_FORMAT_R8G8B8A8_SNORM:
        return RHIPixelFormat::R8_G8_B8_A8_SNorm;
      case DXGI_FORMAT_R8G8B8A8_UINT:
        return RHIPixelFormat::R8_G8_B8_A8_UInt;
      case DXGI_FORMAT_R8G8B8A8_SINT:
        return RHIPixelFormat::R8_G8_B8_A8_SInt;

      case DXGI_FORMAT_R16G16B16A16_UNORM:
        return RHIPixelFormat::R16_G16_B16_A16_UNorm;
      case DXGI_FORMAT_R16G16B16A16_SNORM:
        return RHIPixelFormat::R16_G16_B16_A16_SNorm;
      case DXGI_FORMAT_R16G16B16A16_UINT:
        return RHIPixelFormat::R16_G16_B16_A16_UInt;
      case DXGI_FORMAT_R16G16B16A16_SINT:
        return RHIPixelFormat::R16_G16_B16_A16_SInt;
      case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return RHIPixelFormat::R16_G16_B16_A16_Float;

      case DXGI_FORMAT_R32G32B32A32_UINT:
        return RHIPixelFormat::R32_G32_B32_A32_UInt;
      case DXGI_FORMAT_R32G32B32A32_SINT:
        return RHIPixelFormat::R32_G32_B32_A32_SInt;
      case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return RHIPixelFormat::R32_G32_B32_A32_Float;

      case DXGI_FORMAT_BC1_UNORM:
      case DXGI_FORMAT_BC1_TYPELESS:
        return RHIPixelFormat::BC1_Rgba_UNorm;
      case DXGI_FORMAT_BC2_UNORM:
        return RHIPixelFormat::BC2_UNorm;
      case DXGI_FORMAT_BC3_UNORM:
        return RHIPixelFormat::BC3_UNorm;
      case DXGI_FORMAT_BC4_UNORM:
        return RHIPixelFormat::BC4_UNorm;
      case DXGI_FORMAT_BC4_SNORM:
        return RHIPixelFormat::BC4_SNorm;
      case DXGI_FORMAT_BC5_UNORM:
        return RHIPixelFormat::BC5_UNorm;
      case DXGI_FORMAT_BC5_SNORM:
        return RHIPixelFormat::BC5_SNorm;
      case DXGI_FORMAT_BC7_UNORM:
        return RHIPixelFormat::BC7_UNorm;

      case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return RHIPixelFormat::D24_UNorm_S8_UInt;
      case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return RHIPixelFormat::D32_Float_S8_UInt;

      case DXGI_FORMAT_R10G10B10A2_UINT:
        return RHIPixelFormat::R10_G10_B10_A2_UInt;
      case DXGI_FORMAT_R10G10B10A2_UNORM:
        return RHIPixelFormat::R10_G10_B10_A2_UNorm;
      case DXGI_FORMAT_R11G11B10_FLOAT:
        return RHIPixelFormat::R11_G11_B10_Float;
      default:
        EZ_REPORT_FAILURE("Invalid pixel format");
        return RHIPixelFormat::Default;
    }
  }

  D3D11_BLEND_OP RHIToD3D11BlendOperation(ezEnum<RHIBlendFunction> function)
  {
    switch (function)
    {
      case RHIBlendFunction::Add:
        return D3D11_BLEND_OP_ADD;

      case RHIBlendFunction::Subtract:
        return D3D11_BLEND_OP_SUBTRACT;

      case RHIBlendFunction::ReverseSubtract:
        return D3D11_BLEND_OP_REV_SUBTRACT;

      case RHIBlendFunction::Minimum:
        return D3D11_BLEND_OP_MIN;

      case RHIBlendFunction::Maximum:
        return D3D11_BLEND_OP_MAX;

      default:
        EZ_REPORT_FAILURE("Invalid blend function");
        return D3D11_BLEND_OP_ADD;
    }
  }

  D3D11_FILTER ToD3D11Filter(ezEnum<RHISamplerFilter> filter, bool isComparison)
  {
    switch (filter)
    {
      case RHISamplerFilter::MinPoint_MagPoint_MipPoint:
        return isComparison ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_POINT;

      case RHISamplerFilter::MinPoint_MagPoint_MipLinear:
        return isComparison ? D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR : D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;

      case RHISamplerFilter::MinPoint_MagLinear_MipPoint:
        return isComparison ? D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT : D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;

      case RHISamplerFilter::MinPoint_MagLinear_MipLinear:
        return isComparison ? D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR : D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;

      case RHISamplerFilter::MinLinear_MagPoint_MipPoint:
        return isComparison ? D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT : D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;

      case RHISamplerFilter::MinLinear_MagPoint_MipLinear:
        return isComparison ? D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR : D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;

      case RHISamplerFilter::MinLinear_MagLinear_MipPoint:
        return isComparison ? D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT : D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;

      case RHISamplerFilter::MinLinear_MagLinear_MipLinear:
        return isComparison ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_LINEAR;

      case RHISamplerFilter::Anisotropic:
        return isComparison ? D3D11_FILTER_COMPARISON_ANISOTROPIC : D3D11_FILTER_ANISOTROPIC;

      default:
        EZ_REPORT_FAILURE("Invalid filter");
        return D3D11_FILTER_MIN_MAG_MIP_POINT;
    }
  }

  D3D11_MAP RHIToD3D11MapMode(bool isDynamic, ezEnum<RHIMapMode> mode)
  {
    switch (mode)
    {
      case RHIMapMode::Read:
        return D3D11_MAP_READ;

      case RHIMapMode::Write:
        return isDynamic ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE;

      case RHIMapMode::ReadWrite:
        return D3D11_MAP_READ_WRITE;

      default:
        EZ_REPORT_FAILURE("Invalid map mode.");
        return D3D11_MAP_READ;
    }
  }

  D3D_PRIMITIVE_TOPOLOGY RHIToD3D11PrimitiveTopology(ezEnum<RHIPrimitiveTopology> primitiveTopology)
  {
    switch (primitiveTopology)
    {
      case RHIPrimitiveTopology::TriangleList:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

      case RHIPrimitiveTopology::TriangleStrip:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

      case RHIPrimitiveTopology::LineList:
        return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

      case RHIPrimitiveTopology::LineStrip:
        return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

      case RHIPrimitiveTopology::PointList:
        return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

      default:
        EZ_REPORT_FAILURE("Invalid primitive topology.");
        return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
  }

  D3D11_FILL_MODE RHIToD3D11FillMode(ezEnum<RHIPolygonFillMode> fillMode)
  {
    switch (fillMode)
    {
      case RHIPolygonFillMode::Solid:
        return D3D11_FILL_SOLID;

      case RHIPolygonFillMode::Wireframe:
        return D3D11_FILL_WIREFRAME;

      default:
        EZ_REPORT_FAILURE("Invalid fill mode.");
        return D3D11_FILL_SOLID;
    }
  }

  D3D11_CULL_MODE RHIToD3D11CullMode(ezEnum<RHIFaceCullMode> cullingMode)
  {
    switch (cullingMode)
    {
      case RHIFaceCullMode::Back:
        return D3D11_CULL_BACK;

      case RHIFaceCullMode::Front:
        return D3D11_CULL_FRONT;

      case RHIFaceCullMode::None:
        return D3D11_CULL_NONE;

      default:
        EZ_REPORT_FAILURE("Invalid cull mode.");
        return D3D11_CULL_NONE;
    }
  }

  DXGI_FORMAT ToDxgiFormat(ezEnum<RHIVertexElementFormat> format)
  {
    switch (format)
    {
      case RHIVertexElementFormat::Float1:
        return DXGI_FORMAT_R32_FLOAT;

      case RHIVertexElementFormat::Float2:
        return DXGI_FORMAT_R32G32_FLOAT;

      case RHIVertexElementFormat::Float3:
        return DXGI_FORMAT_R32G32B32_FLOAT;

      case RHIVertexElementFormat::Float4:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;

      case RHIVertexElementFormat::Byte2_Norm:
        return DXGI_FORMAT_R8G8_UNORM;

      case RHIVertexElementFormat::Byte2:
        return DXGI_FORMAT_R8G8_UINT;

      case RHIVertexElementFormat::Byte4_Norm:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

      case RHIVertexElementFormat::Byte4:
        return DXGI_FORMAT_R8G8B8A8_UINT;

      case RHIVertexElementFormat::SByte2_Norm:
        return DXGI_FORMAT_R8G8_SNORM;

      case RHIVertexElementFormat::SByte2:
        return DXGI_FORMAT_R8G8_SINT;

      case RHIVertexElementFormat::SByte4_Norm:
        return DXGI_FORMAT_R8G8B8A8_SNORM;

      case RHIVertexElementFormat::SByte4:
        return DXGI_FORMAT_R8G8B8A8_SINT;

      case RHIVertexElementFormat::UShort2_Norm:
        return DXGI_FORMAT_R16G16_UNORM;

      case RHIVertexElementFormat::UShort2:
        return DXGI_FORMAT_R16G16_UINT;

      case RHIVertexElementFormat::UShort4_Norm:
        return DXGI_FORMAT_R16G16B16A16_UNORM;

      case RHIVertexElementFormat::UShort4:
        return DXGI_FORMAT_R16G16B16A16_UINT;

      case RHIVertexElementFormat::Short2_Norm:
        return DXGI_FORMAT_R16G16_SNORM;

      case RHIVertexElementFormat::Short2:
        return DXGI_FORMAT_R16G16_SINT;

      case RHIVertexElementFormat::Short4_Norm:
        return DXGI_FORMAT_R16G16B16A16_SNORM;

      case RHIVertexElementFormat::Short4:
        return DXGI_FORMAT_R16G16B16A16_SINT;

      case RHIVertexElementFormat::UInt1:
        return DXGI_FORMAT_R32_UINT;

      case RHIVertexElementFormat::UInt2:
        return DXGI_FORMAT_R32G32_UINT;

      case RHIVertexElementFormat::UInt3:
        return DXGI_FORMAT_R32G32B32_UINT;

      case RHIVertexElementFormat::UInt4:
        return DXGI_FORMAT_R32G32B32A32_UINT;

      case RHIVertexElementFormat::Int1:
        return DXGI_FORMAT_R32_SINT;

      case RHIVertexElementFormat::Int2:
        return DXGI_FORMAT_R32G32_SINT;

      case RHIVertexElementFormat::Int3:
        return DXGI_FORMAT_R32G32B32_SINT;

      case RHIVertexElementFormat::Int4:
        return DXGI_FORMAT_R32G32B32A32_SINT;

      case RHIVertexElementFormat::Half1:
        return DXGI_FORMAT_R16_FLOAT;

      case RHIVertexElementFormat::Half2:
        return DXGI_FORMAT_R16G16_FLOAT;

      case RHIVertexElementFormat::Half4:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;

      default:
        EZ_REPORT_FAILURE("Invalid comparison kind.");
        return DXGI_FORMAT_UNKNOWN;
    }
  }

  D3D11_COMPARISON_FUNC RHIToD3D11ComparisonFunc(ezEnum<RHIComparisonKind> comparisonKind)
  {
    switch (comparisonKind)
    {
      case RHIComparisonKind::Never:
        return D3D11_COMPARISON_NEVER;

      case RHIComparisonKind::Less:
        return D3D11_COMPARISON_LESS;

      case RHIComparisonKind::Equal:
        return D3D11_COMPARISON_EQUAL;

      case RHIComparisonKind::LessEqual:
        return D3D11_COMPARISON_LESS_EQUAL;

      case RHIComparisonKind::Greater:
        return D3D11_COMPARISON_GREATER;

      case RHIComparisonKind::NotEqual:
        return D3D11_COMPARISON_NOT_EQUAL;

      case RHIComparisonKind::GreaterEqual:
        return D3D11_COMPARISON_GREATER_EQUAL;

      case RHIComparisonKind::Always:
        return D3D11_COMPARISON_ALWAYS;

      default:
        EZ_REPORT_FAILURE("Invalid comparison kind.");
        return D3D11_COMPARISON_NEVER;
    }
  }

  D3D11_TEXTURE_ADDRESS_MODE RHIToD3D11AddressMode(ezEnum<RHISamplerAddressMode> mode)
  {
    switch (mode)
    {
      case RHISamplerAddressMode::Wrap:
        return D3D11_TEXTURE_ADDRESS_WRAP;

      case RHISamplerAddressMode::Mirror:
        return D3D11_TEXTURE_ADDRESS_MIRROR;

      case RHISamplerAddressMode::Clamp:
        return D3D11_TEXTURE_ADDRESS_CLAMP;

      case RHISamplerAddressMode::Border:
        return D3D11_TEXTURE_ADDRESS_BORDER;

      default:
        EZ_REPORT_FAILURE("Invalid sampler mode");
        return D3D11_TEXTURE_ADDRESS_WRAP;
    }
  }

  DXGI_FORMAT GetDepthFormat(ezEnum<RHIPixelFormat> format)
  {
    switch (format)
    {
      case RHIPixelFormat::R32_Float:
        return DXGI_FORMAT_R32_FLOAT;

      case RHIPixelFormat::R16_UNorm:
        return DXGI_FORMAT_D16_UNORM;

      case RHIPixelFormat::D24_UNorm_S8_UInt:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;

      case RHIPixelFormat::D32_Float_S8_UInt:
        return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

      default:
        EZ_REPORT_FAILURE("Invalid depth texture format: {}", format);
        return DXGI_FORMAT_UNKNOWN;
    }
  }
} // namespace D3D11Formats


EZ_STATICLINK_FILE(RHI, RHI_Backends_D3D11_Implementation_D3D11Formats);

