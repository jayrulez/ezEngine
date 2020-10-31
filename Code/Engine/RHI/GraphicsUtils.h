#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Device/GraphicsDevice.h>
#include <RHI/Resources/SwapchainSource.h>

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS_DESKTOP)
#  include <Foundation/Basics/Platform/Win/MinWindows.h>

#  ifdef EZ_RHI_D3D11_SUPPORTED
#include <RHI/Backends/D3D11/D3D11DeviceOptions.h>
#include <RHI/Backends/D3D11/D3D11GraphicsDevice.h>
#  endif

#endif

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS_UWP)
#endif

#if EZ_ENABLED(EZ_PLATFORM_OSX)
#endif

#if EZ_ENABLED(EZ_PLATFORM_LINUX)
#endif

#if EZ_ENABLED(EZ_PLATFORM_IOS)
#endif

#if EZ_ENABLED(EZ_PLATFORM_ANDROID)
#endif

class GraphicsUtils
{
public:
  /// <summary>
  /// Checks whether the given <see cref="GraphicsBackend"/> is supported on this system.
  /// </summary>
  /// <param name="backend">The GraphicsBackend to check.</param>
  /// <returns>True if the GraphicsBackend is supported; false otherwise.</returns>
  static bool IsBackendSupported(ezEnum<RHIGraphicsBackend> backend)
  {
    switch (backend)
    {
#ifdef EZ_RHI_D3D11_SUPPORTED
      case RHIGraphicsBackend::Direct3D11:
        return true;
#endif

#ifdef EZ_RHI_VULKAN_SUPPORTED
      case RHIGraphicsBackend::Vulkan:
        return true;
#endif

#ifdef EZ_RHI_METAL_SUPPORTED
      case RHIGraphicsBackend::Metal:
        return true;
#endif

#ifdef EZ_RHI_OPENGL_SUPPORTED
      case RHIGraphicsBackend::OpenGL:
        return true;
#endif

#ifdef EZ_RHI_OPENGLGLES_SUPPORTED
      case RHIGraphicsBackend::OpenGLES:
        return true;
#endif

      default:
        return false;
    }
    return false;
  }

#ifdef EZ_RHI_D3D11_SUPPORTED
  /// <summary>
  /// Creates a new <see cref="GraphicsDevice"/> using Direct3D 11.
  /// </summary>
  /// <param name="options">Describes several common properties of the GraphicsDevice.</param>
  /// <returns>A new <see cref="GraphicsDevice"/> using the Direct3D 11 API.</returns>
  static RHIGraphicsDevice* CreateD3D11(const RHIGraphicsDeviceOptions& options)
  {
    //return new D3D11GraphicsDevice(options, new D3D11DeviceOptions(), null);
    return nullptr;
  }

  /// <summary>
  /// Creates a new <see cref="GraphicsDevice"/> using Direct3D 11, with a main Swapchain.
  /// </summary>
  /// <param name="options">Describes several common properties of the GraphicsDevice.</param>
  /// <param name="swapchainDescription">A description of the main Swapchain to create.</param>
  /// <returns>A new <see cref="GraphicsDevice"/> using the Direct3D 11 API.</returns>
  static RHIGraphicsDevice* CreateD3D11(const RHIGraphicsDeviceOptions& options, const RHISwapchainDescription& swapchainDescription)
  {
    //return new D3D11GraphicsDevice(options, D3D11DeviceOptions(), swapchainDescription);
    return nullptr;
  }

  /// <summary>
  /// Creates a new <see cref="GraphicsDevice"/> using Direct3D 11.
  /// </summary>
  /// <param name="options">Describes several common properties of the GraphicsDevice.</param>
  /// <param name="d3d11Options">The Direct3D11-specific options used to create the device.</param>
  /// <returns>A new <see cref="GraphicsDevice"/> using the Direct3D 11 API.</returns>
  static RHIGraphicsDevice* CreateD3D11(const RHIGraphicsDeviceOptions& options, const RHID3D11DeviceOptions& d3d11Options)
  {
    //return new D3D11.D3D11GraphicsDevice(options, d3d11Options, null);
    return nullptr;
  }

  /// <summary>
  /// Creates a new <see cref="GraphicsDevice"/> using Direct3D 11, with a main Swapchain.
  /// </summary>
  /// <param name="options">Describes several common properties of the GraphicsDevice.</param>
  /// <param name="d3d11Options">The Direct3D11-specific options used to create the device.</param>
  /// <param name="swapchainDescription">A description of the main Swapchain to create.</param>
  /// <returns>A new <see cref="GraphicsDevice"/> using the Direct3D 11 API.</returns>
  static RHIGraphicsDevice* CreateD3D11(const RHIGraphicsDeviceOptions& options, const RHID3D11DeviceOptions& d3d11Options, const RHISwapchainDescription& swapchainDescription)
  {
    //return new D3D11GraphicsDevice(options, d3d11Options, swapchainDescription);
    return nullptr;
  }

  /// <summary>
  /// Creates a new <see cref="GraphicsDevice"/> using Direct3D 11, with a main Swapchain.
  /// </summary>
  /// <param name="options">Describes several common properties of the GraphicsDevice.</param>
  /// <param name="hwnd">The Win32 window handle to render into.</param>
  /// <param name="width">The initial width of the window.</param>
  /// <param name="height">The initial height of the window.</param>
  /// <returns>A new <see cref="GraphicsDevice"/> using the Direct3D 11 API.</returns>
  static RHIGraphicsDevice* CreateD3D11(const RHIGraphicsDeviceOptions& options, ezMinWindows::HWND hwnd, ezUInt32 width, ezUInt32 height)
  {
    RHISwapchainDescription swapchainDescription(
      new RHIWin32SwapchainSource(hwnd, nullptr),
      width, height,
      options.SwapchainDepthFormat,
      options.SyncToVerticalBlank,
      options.SwapchainSrgbFormat);

    //return new D3D11GraphicsDevice(options, D3D11DeviceOptions(), swapchainDescription);
    return nullptr;
  }
#endif
};
