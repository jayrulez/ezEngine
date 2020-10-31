#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

class RHIResourceSet;

#include <Foundation/Algorithm/HashableStruct.h>

struct RHIBoundResourceSetInfo : public ezHashableStruct<RHIBoundResourceSetInfo>
{
  RHIResourceSet* Set = nullptr;
  ezHybridArray<ezUInt32, 5> Offsets;

  RHIBoundResourceSetInfo() = default;

  RHIBoundResourceSetInfo(RHIResourceSet* set, ezDynamicArray<ezUInt32> offsets)
  {
    Set = set;
    Offsets.SetCountUninitialized(offsets.GetCount());
    Offsets.GetArrayPtr().CopyFrom(offsets.GetArrayPtr());
  }

  bool Equals(RHIResourceSet* set, ezUInt32 offsetsCount, ezDynamicArray<ezUInt32> offsets) const
  {
    if (Set != set || offsetsCount != Offsets.GetCount())
      return false;

    if (Offsets.GetCount() != offsets.GetCount())
      return false;

    for (ezUInt32 i = 0; i < Offsets.GetCount(); i++)
    {
      if (Offsets[i] != offsets[i])
        return false;
    }

    return true;
  }

  bool Equals(const RHIBoundResourceSetInfo& other) const
  {
    if (Set != other.Set || Offsets.GetCount() != Offsets.GetCount())
      return false;

    if (Offsets.GetCount() != other.Offsets.GetCount())
      return false;

    for (ezUInt32 i = 0; i < Offsets.GetCount(); i++)
    {
      if (Offsets[i] != other.Offsets[i])
        return false;
    }

    return true;
  }

  bool operator==(const RHIBoundResourceSetInfo& other) const
  {
    return this->Equals(other);
  }
};
