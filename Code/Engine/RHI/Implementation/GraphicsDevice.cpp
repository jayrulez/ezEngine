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

float GraphicsDevice::GetScreenWidth() const
{
  return (float)GetResolutionWidth() / 1.f /*wiPlatform::GetDPIScaling()*/;
}
float GraphicsDevice::GetScreenHeight() const
{
  return (float)GetResolutionHeight() / 1.f /*wiPlatform::GetDPIScaling()*/;
}
