#include <RHIPCH.h>

#include <RHI/Resource/ResourceStateTracker.h>

ezRHIResourceStateTracker::ezRHIResourceStateTracker(ezRHIResource& resource)
  : m_Resource{resource}
{
}

bool ezRHIResourceStateTracker::HasResourceState() const
{
  return m_SubresourceStates.IsEmpty();
}

ezBitflags<ezRHIResourceState> ezRHIResourceStateTracker::GetResourceState() const
{
  return m_ResourceState;
}

void ezRHIResourceStateTracker::SetResourceState(ezBitflags<ezRHIResourceState> state)
{
  m_SubresourceStates.Clear();
  m_ResourceState = state;
  m_SubresourceStateGroups.Clear();
}

ezBitflags<ezRHIResourceState> ezRHIResourceStateTracker::GetSubresourceState(ezUInt32 mipLevel, ezUInt32 arrayLayer) const
{
  auto it = m_SubresourceStates.Find(std::make_tuple(mipLevel, arrayLayer));
  if (it != end(m_SubresourceStates))
  {
    return it.Value();
  }
  return m_ResourceState;
}

void ezRHIResourceStateTracker::SetSubresourceState(ezUInt32 mipLevel, ezUInt32 arrayLayer, ezBitflags<ezRHIResourceState> state)
{
  if (HasResourceState() && GetResourceState() == state)
  {
    return;
  }
  std::tuple<ezUInt32, ezUInt32> key = {mipLevel, arrayLayer};

  if (m_SubresourceStates.Contains(key))
  {
    if (--m_SubresourceStateGroups[m_SubresourceStates[key]] == 0)
      m_SubresourceStateGroups.Remove(m_SubresourceStates[key]);
  }
  m_SubresourceStates[key] = state;
  ++m_SubresourceStateGroups[state];
  if (m_SubresourceStateGroups.GetCount() == 1 && begin(m_SubresourceStateGroups).Value() == m_Resource.GetLayerCount() * m_Resource.GetLevelCount())
  {
    m_SubresourceStateGroups.Clear();
    m_SubresourceStates.Clear();
    m_ResourceState = state;
  }
}

void ezRHIResourceStateTracker::Merge(const ezRHIResourceStateTracker& other)
{
  if (other.HasResourceState())
  {
    auto state = other.GetResourceState();
    if (state != ezRHIResourceState::Unknown)
    {
      SetResourceState(state);
    }
  }
  else
  {
    for (ezUInt32 i = 0; i < other.m_Resource.GetLevelCount(); ++i)
    {
      for (ezUInt32 j = 0; j < other.m_Resource.GetLayerCount(); ++j)
      {
        auto state = other.GetSubresourceState(i, j);
        if (state != ezRHIResourceState::Unknown)
          SetSubresourceState(i, j, state);
      }
    }
  }
}
