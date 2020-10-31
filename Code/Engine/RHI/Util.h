#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

class RHIResource;
class RHITexture;
class RHITextureView;
class RHIBuffer;
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

  template <typename T, typename TDerived>
  bool AreEquatable(const ezArrayBase<T, TDerived>& lhs, const ezArrayBase<T, TDerived>& rhs)
  {
    if (lhs.GetCount() != rhs.GetCount())
      return false;

    for (ezUInt32 i = 0; i < lhs.GetCount(); i++)
    {
      if (!(lhs[i] == rhs[i]))
        return false;
    }

    return true;
  }

  template <typename T, typename TDerived>
  bool AreEqual(const ezArrayBase<T, TDerived>& lhs, const ezArrayBase<T, TDerived>& rhs)
  {
    if (lhs.GetCount() != rhs.GetCount())
      return false;

    for (ezUInt32 i = 0; i < lhs.GetCount(); i++)
    {
      if (!(std::addressof(lhs[i]) == std::addressof(rhs[i])))
        return false;
    }

    return true;
  }

  template <typename T>
  ezDynamicArray<T> ShallowClone(const ezDynamicArray<T>& source)
  {
    return ezDynamicArray(source);
  }

  void GetMipDimensions(RHITexture* tex, ezUInt32 mipLevel, ezUInt32& width, ezUInt32& height, ezUInt32& depth);

  ezUInt32 GetDimension(ezUInt32 largestLevelDimension, ezUInt32 mipLevel);

  RHIBufferRange GetBufferRange(RHIResource* resource, ezUInt32 additionalOffset);
  bool GetDeviceBuffer(RHIResource* resource, RHIBuffer* buffer);

  RHITextureView* GetTextureView(RHIGraphicsDevice* gd, RHIResource* resource);

} // namespace Util
