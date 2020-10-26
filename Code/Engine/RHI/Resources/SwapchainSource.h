#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

/// <summary>
/// A platform-specific object representing a renderable surface.
/// A SwapchainSource can be created with one of several static factory methods.
/// A SwapchainSource is used to describe a Swapchain (see <see cref="SwapchainDescription"/>).
/// </summary>
class EZ_RHI_DLL RHISwapchainSource
{
public:
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