#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Strings/HashedString.h>

struct EZ_RHI_DLL RHIDeviceResourceFlags // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    None,
    /// <summary>
    /// A marker designating a resource which can be bound in a <see cref="RHIResourceSet"/> and used in a shader.
    /// See <see cref="RHIDeviceBuffer"/>, <see cref="RHIDeviceBufferRange"/>, <see cref="RHITexture"/>, <see cref="RHITextureView"/>
    /// and <see cref="RHISampler"/>.
    /// </summary>
    Bindable,
    /// <summary>
    /// A marker designating a device resource which can be mapped into CPU-visible memory with
    /// <see cref="RHIGraphicsDevice.Map(RHIDeviceResource, RHIMapMode, ezUInt32)"/>
    /// </summary>
    Mappable,
  };

  struct Bits
  {
  };
};
EZ_DECLARE_FLAGS_OPERATORS(RHIDeviceResourceFlags);

/// <summary>
/// A resource owned by a <see cref="RHIGraphicsDevice"/>, which can be given a string identifier for debugging and
/// informational purposes.
/// </summary>
class EZ_RHI_DLL RHIDeviceResource : public ezRefCounted
{
public:
  virtual ~RHIDeviceResource() {}

  /// <summary>
  /// A string identifying this instance. Can be used to differentiate between objects in graphics debuggers and other
  /// tools.
  /// </summary>
  virtual ezString GetName() const = 0;

  virtual void SetName(const ezString& name) = 0;

  /// <summary>
  /// Checks if this device resource has been disposed
  /// </summary>
  /// <returns></returns>
  virtual bool IsDisposed() const = 0;

  /// <summary>
  /// Frees device resources controlled by this instance.
  /// </summary>
  virtual void Dispose() = 0;

  const ezBitflags<RHIDeviceResourceFlags> GetFlags() const
  {
    return Flags;
  }

  const bool IsBindable() const
  {
    return Flags.IsSet(RHIDeviceResourceFlags::Bindable);
  }

  const bool IsMappable() const
  {
    return Flags.IsSet(RHIDeviceResourceFlags::Mappable);
  }

protected:
  ezBitflags<RHIDeviceResourceFlags> Flags = RHIDeviceResourceFlags::None;
};
