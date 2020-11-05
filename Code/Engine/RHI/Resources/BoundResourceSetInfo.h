#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

class RHIResourceSet;

#include <Foundation/Algorithm/HashableStruct.h>

struct RHIBoundResourceSetInfo : public ezHashableStruct<RHIBoundResourceSetInfo>
{
  RHIResourceSet* Set = nullptr;
  RHISmallFixedOrDynamicArray Offsets;

  RHIBoundResourceSetInfo() = default;

  RHIBoundResourceSetInfo(RHIResourceSet* set, ezDynamicArray<ezUInt32> offsets);
  ~RHIBoundResourceSetInfo();

  bool Equals(RHIResourceSet* set, ezUInt32 offsetsCount, ezDynamicArray<ezUInt32> offsets) const;

  bool Equals(const RHIBoundResourceSetInfo& other) const;

  bool operator==(const RHIBoundResourceSetInfo& other) const
  {
    return this->Equals(other);
  }
};
