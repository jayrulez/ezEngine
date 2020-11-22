#include <RHI/GraphicsDevice.h>

#ifdef _WIN32
// These will let the driver select the dedicated GPU in favour of the integrated one:
extern "C"
{
  _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
  _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif // _WIN32

float GraphicsDevice::GetScreenWidth() const
{
  return (float)GetResolutionWidth() / 1.f /*wiPlatform::GetDPIScaling()*/;
}
float GraphicsDevice::GetScreenHeight() const
{
  return (float)GetResolutionHeight() / 1.f /*wiPlatform::GetDPIScaling()*/;
}
