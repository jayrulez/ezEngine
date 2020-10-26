#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

/// <summary>
/// A platform-specific object representing a renderable surface.
/// A SwapchainSource can be created with one of several static factory methods.
/// A SwapchainSource is used to describe a Swapchain (see <see cref="SwapchainDescription"/>).
/// </summary>
struct EZ_RHI_DLL RHISwapchainSource
{
  RHISwapchainSource() = default;

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  virtual bool operator==(const RHISwapchainSource& other) const
  {
    return true;
  }
};

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS_DESKTOP)
#  include <Foundation/Basics/Platform/Win/MinWindows.h>

struct RHIWin32SwapchainSource : public RHISwapchainSource
{
  ezMinWindows::HWND Hwnd = nullptr;
  ezMinWindows::HINSTANCE Hinstance = nullptr;

  RHIWin32SwapchainSource() {}

  RHIWin32SwapchainSource(ezMinWindows::HWND hwnd, ezMinWindows::HINSTANCE hinstance)
  {
    Hwnd = hwnd;
    Hinstance = hinstance;
  }
};
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
