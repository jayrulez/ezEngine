#include <RHI/Resources/BoundResourceSetInfo.h>

RHIBoundResourceSetInfo::RHIBoundResourceSetInfo(RHIResourceSet* set, ezDynamicArray<ezUInt32> offsets)
{
  Set = set;
  Offsets = new RHISmallFixedOrDynamicArray(offsets.GetCount(), offsets);
}

RHIBoundResourceSetInfo::~RHIBoundResourceSetInfo()
{
  Offsets->Dispose();
  delete Offsets;
}

bool RHIBoundResourceSetInfo::Equals(RHIResourceSet* set, ezUInt32 offsetsCount, ezDynamicArray<ezUInt32> offsets) const
{
  if (Set != set || offsetsCount != Offsets->GetCount())
    return false;

  if (Offsets->GetCount() != offsets.GetCount())
    return false;

  for (ezUInt32 i = 0; i < Offsets->GetCount(); i++)
  {
    if (Offsets->Get(i) != offsets[i])
      return false;
  }

  return true;
}

bool RHIBoundResourceSetInfo::Equals(const RHIBoundResourceSetInfo& other) const
{
  if (Set != other.Set || Offsets->GetCount() != Offsets->GetCount())
    return false;

  if (Offsets->GetCount() != other.Offsets->GetCount())
    return false;

  for (ezUInt32 i = 0; i < Offsets->GetCount(); i++)
  {
    if (Offsets->Get(i) != other.Offsets->Get(i))
      return false;
  }

  return true;
}


EZ_STATICLINK_FILE(RHI, RHI_Resources_Implementation_BoundResourceSetInfo);

