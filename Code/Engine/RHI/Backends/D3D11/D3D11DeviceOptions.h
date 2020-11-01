#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <dxgi.h>

/// <summary>
/// A structure describing Direct3D11-specific device creation options.
/// </summary>
struct RHID3D11DeviceOptions
{
  /// <summary>
  /// Native pointer to an adapter.
  /// </summary>
  IDXGIAdapter* AdapterPtr = nullptr;

  /// <summary>
  /// Set of device specific flags.
  /// See <see cref="Vortice.Direct3D11.DeviceCreationFlags"/> for details.
  /// </summary>
  ezUInt32 DeviceCreationFlags = 0;
};
