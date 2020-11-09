#include <RHI/GraphicsDevice.h>

#ifdef _WIN32
// These will let the driver select the dedicated GPU in favour of the integrated one:
extern "C"
{
  _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
  _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif // _WIN32

bool GraphicsDevice::CheckCapability(GRAPHICSDEVICE_CAPABILITY capability) const
{
  switch (capability)
  {
    case GRAPHICSDEVICE_CAPABILITY_TESSELLATION:
      return TESSELLATION;
    case GRAPHICSDEVICE_CAPABILITY_CONSERVATIVE_RASTERIZATION:
      return CONSERVATIVE_RASTERIZATION;
    case GRAPHICSDEVICE_CAPABILITY_RASTERIZER_ORDERED_VIEWS:
      return RASTERIZER_ORDERED_VIEWS;
    case GRAPHICSDEVICE_CAPABILITY_UAV_LOAD_FORMAT_COMMON:
      return UAV_LOAD_FORMAT_COMMON;
    case GRAPHICSDEVICE_CAPABILITY_UAV_LOAD_FORMAT_R11G11B10_FLOAT:
      return UAV_LOAD_FORMAT_R11G11B10_FLOAT;
    case GRAPHICSDEVICE_CAPABILITY_RENDERTARGET_AND_VIEWPORT_ARRAYINDEX_WITHOUT_GS:
      return RENDERTARGET_AND_VIEWPORT_ARRAYINDEX_WITHOUT_GS;
    case GRAPHICSDEVICE_CAPABILITY_RAYTRACING:
      return RAYTRACING;
    case GRAPHICSDEVICE_CAPABILITY_RAYTRACING_INLINE:
      return RAYTRACING_INLINE;
    case GRAPHICSDEVICE_CAPABILITY_DESCRIPTOR_MANAGEMENT:
      return DESCRIPTOR_MANAGEMENT;
    case GRAPHICSDEVICE_CAPABILITY_VARIABLE_RATE_SHADING:
      return VARIABLE_RATE_SHADING;
    case GRAPHICSDEVICE_CAPABILITY_VARIABLE_RATE_SHADING_TIER2:
      return VARIABLE_RATE_SHADING_TIER2;
    case GRAPHICSDEVICE_CAPABILITY_MESH_SHADER:
      return MESH_SHADER;
  }
  return false;
}

ezUInt32 GraphicsDevice::GetFormatStride(ezEnum<ezRHIFormat> value) const
{
  switch (value)
  {

    case ezRHIFormat::R32G32B32A32_FLOAT:
    case ezRHIFormat::R32G32B32A32_UINT:
    case ezRHIFormat::R32G32B32A32_SINT:
    case ezRHIFormat::BC1_UNORM:
    case ezRHIFormat::BC1_UNORM_SRGB:
    case ezRHIFormat::BC2_UNORM:
    case ezRHIFormat::BC2_UNORM_SRGB:
    case ezRHIFormat::BC3_UNORM:
    case ezRHIFormat::BC3_UNORM_SRGB:
    case ezRHIFormat::BC4_SNORM:
    case ezRHIFormat::BC4_UNORM:
    case ezRHIFormat::BC5_SNORM:
    case ezRHIFormat::BC5_UNORM:
    case ezRHIFormat::BC6H_UF16:
    case ezRHIFormat::BC6H_SF16:
    case ezRHIFormat::BC7_UNORM:
    case ezRHIFormat::BC7_UNORM_SRGB:
      return 16;

    case ezRHIFormat::R32G32B32_FLOAT:
    case ezRHIFormat::R32G32B32_UINT:
    case ezRHIFormat::R32G32B32_SINT:
      return 12;

    case ezRHIFormat::R16G16B16A16_FLOAT:
    case ezRHIFormat::R16G16B16A16_UNORM:
    case ezRHIFormat::R16G16B16A16_UINT:
    case ezRHIFormat::R16G16B16A16_SNORM:
    case ezRHIFormat::R16G16B16A16_SINT:
      return 8;

    case ezRHIFormat::R32G32_FLOAT:
    case ezRHIFormat::R32G32_UINT:
    case ezRHIFormat::R32G32_SINT:
    case ezRHIFormat::R32G8X24_TYPELESS:
    case ezRHIFormat::D32_FLOAT_S8X24_UINT:
      return 8;

    case ezRHIFormat::R10G10B10A2_UNORM:
    case ezRHIFormat::R10G10B10A2_UINT:
    case ezRHIFormat::R11G11B10_FLOAT:
    case ezRHIFormat::R8G8B8A8_UNORM:
    case ezRHIFormat::R8G8B8A8_UNORM_SRGB:
    case ezRHIFormat::R8G8B8A8_UINT:
    case ezRHIFormat::R8G8B8A8_SNORM:
    case ezRHIFormat::R8G8B8A8_SINT:
    case ezRHIFormat::B8G8R8A8_UNORM:
    case ezRHIFormat::B8G8R8A8_UNORM_SRGB:
    case ezRHIFormat::R16G16_FLOAT:
    case ezRHIFormat::R16G16_UNORM:
    case ezRHIFormat::R16G16_UINT:
    case ezRHIFormat::R16G16_SNORM:
    case ezRHIFormat::R16G16_SINT:
    case ezRHIFormat::R32_TYPELESS:
    case ezRHIFormat::D32_FLOAT:
    case ezRHIFormat::R32_FLOAT:
    case ezRHIFormat::R32_UINT:
    case ezRHIFormat::R32_SINT:
    case ezRHIFormat::R24G8_TYPELESS:
    case ezRHIFormat::D24_UNORM_S8_UINT:
      return 4;

    case ezRHIFormat::R8G8_UNORM:
    case ezRHIFormat::R8G8_UINT:
    case ezRHIFormat::R8G8_SNORM:
    case ezRHIFormat::R8G8_SINT:
    case ezRHIFormat::R16_TYPELESS:
    case ezRHIFormat::R16_FLOAT:
    case ezRHIFormat::D16_UNORM:
    case ezRHIFormat::R16_UNORM:
    case ezRHIFormat::R16_UINT:
    case ezRHIFormat::R16_SNORM:
    case ezRHIFormat::R16_SINT:
      return 2;

    case ezRHIFormat::R8_UNORM:
    case ezRHIFormat::R8_UINT:
    case ezRHIFormat::R8_SNORM:
    case ezRHIFormat::R8_SINT:
      return 1;


    default:
      EZ_REPORT_FAILURE("Unsupported format."); // didn't catch format!
      break;
  }

  return 16;
}

bool GraphicsDevice::IsFormatUnorm(ezEnum<ezRHIFormat> value) const
{
  switch (value)
  {
    case ezRHIFormat::R16G16B16A16_UNORM:
    case ezRHIFormat::R10G10B10A2_UNORM:
    case ezRHIFormat::R8G8B8A8_UNORM:
    case ezRHIFormat::R8G8B8A8_UNORM_SRGB:
    case ezRHIFormat::B8G8R8A8_UNORM:
    case ezRHIFormat::B8G8R8A8_UNORM_SRGB:
    case ezRHIFormat::R16G16_UNORM:
    case ezRHIFormat::D24_UNORM_S8_UINT:
    case ezRHIFormat::R8G8_UNORM:
    case ezRHIFormat::D16_UNORM:
    case ezRHIFormat::R16_UNORM:
    case ezRHIFormat::R8_UNORM:
      return true;
  }

  return false;
}

bool GraphicsDevice::IsFormatBlockCompressed(ezEnum<ezRHIFormat> value) const
{
  switch (value)
  {
    case ezRHIFormat::BC1_UNORM:
    case ezRHIFormat::BC1_UNORM_SRGB:
    case ezRHIFormat::BC2_UNORM:
    case ezRHIFormat::BC2_UNORM_SRGB:
    case ezRHIFormat::BC3_UNORM:
    case ezRHIFormat::BC3_UNORM_SRGB:
    case ezRHIFormat::BC4_UNORM:
    case ezRHIFormat::BC4_SNORM:
    case ezRHIFormat::BC5_UNORM:
    case ezRHIFormat::BC5_SNORM:
    case ezRHIFormat::BC6H_UF16:
    case ezRHIFormat::BC6H_SF16:
    case ezRHIFormat::BC7_UNORM:
    case ezRHIFormat::BC7_UNORM_SRGB:
      return true;
  }

  return false;
}

bool GraphicsDevice::IsFormatStencilSupport(ezEnum<ezRHIFormat> value) const
{
  switch (value)
  {
    case ezRHIFormat::R32G8X24_TYPELESS:
    case ezRHIFormat::D32_FLOAT_S8X24_UINT:
    case ezRHIFormat::R24G8_TYPELESS:
    case ezRHIFormat::D24_UNORM_S8_UINT:
      return true;
  }

  return false;
}

float GraphicsDevice::GetScreenWidth() const
{
  return (float)GetResolutionWidth() / 1.f /*wiPlatform::GetDPIScaling()*/;
}
float GraphicsDevice::GetScreenHeight() const
{
  return (float)GetResolutionHeight() / 1.f /*wiPlatform::GetDPIScaling()*/;
}
