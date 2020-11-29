#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <RHI/GraphicsUtils.h>


EZ_STATICLINK_FILE(RHI, RHI_Implementation_GraphicsUtils);

/// <summary>
/// Creates a new <see cref="GraphicsDevice"/> using Direct3D 11, with a main Swapchain.
/// </summary>
/// <param name="options">Describes several common properties of the GraphicsDevice.</param>
/// <param name="swapchainDescription">A description of the main Swapchain to create.</param>
/// <returns>A new <see cref="GraphicsDevice"/> using the Direct3D 11 API.</returns>

RHIGraphicsDevice* GraphicsUtils::CreateD3D11(const RHIGraphicsDeviceOptions& options, const RHISwapchainDescription& swapchainDescription)
{
  RHID3D11DeviceOptions d3d11DeviceOptions;
  return EZ_DEFAULT_NEW(D3D11GraphicsDevice, options, d3d11DeviceOptions, std::make_optional(swapchainDescription));
}
