#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class EZ_RHI_DLL ezRHIResourceStateTracker
{
public:
  ezRHIResourceStateTracker(ezRHIResource& resource);
  bool HasResourceState() const;
  ezBitflags<ezRHIResourceState> GetResourceState() const;
  void SetResourceState(ezBitflags<ezRHIResourceState> state);
  ezBitflags<ezRHIResourceState> GetSubresourceState(ezUInt32 mipLevel, ezUInt32 arrayLayer) const;
  void SetSubresourceState(ezUInt32 mipLevel, ezUInt32 arrayLayer, ezBitflags<ezRHIResourceState> state);
  void Merge(const ezRHIResourceStateTracker& other);

private:
  ezRHIResource& m_Resource;
  ezMap<std::tuple<ezUInt32, ezUInt32>, ezBitflags<ezRHIResourceState>> m_SubresourceStates;
  ezMap<ezBitflags<ezRHIResourceState>, ezUInt64> m_SubresourceStateGroups;
  ezBitflags<ezRHIResourceState> m_ResourceState = ezRHIResourceState::Unknown;
};
