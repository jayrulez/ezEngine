#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

class RHIResource;
class RHITexture;
class RHIGraphicsDevice;
struct RHIBufferRange;

namespace Util
{
  template <typename Base, typename Derived>
  Derived* AssertSubtype(Base* base)
  {
    if (base == nullptr)
    {
      EZ_REPORT_FAILURE("Base type must not be null.");
      return nullptr;
    }

    Derived* derived = dynamic_cast<Derived*>(base);
    if (derived == nullptr)
      EZ_REPORT_FAILURE("Type must be derived from specified base type.");

    return derived;
  }

  template <typename T, typename Derived>
  void ClearArray(ezArrayBase<T, Derived> array)
  {
    ezUInt32 count = array.GetCount();
    array.Clear();
    array.SetCount(count);
  }

  void GetMipDimensions(RHITexture* tex, ezUInt32 mipLevel, ezUInt32& width, ezUInt32& height, ezUInt32& depth);

  ezUInt32 GetDimension(ezUInt32 largestLevelDimension, ezUInt32 mipLevel);

  RHIBufferRange GetBufferRange(RHIResource* resource, ezUInt32 additionalOffset);
  bool GetDeviceBuffer(RHIResource* resource, RHIBuffer* buffer);

  RHITextureView* GetTextureView(RHIGraphicsDevice* gd, RHIResource* resource);

} // namespace Util
