
#pragma once

#include <Foundation/Containers/HashTable.h>
#include <Foundation/Strings/HashedString.h>
#include <RHI/Renderer/Basics.h>

class EZ_RHI_DLL ezRHIResourceBase : public ezRefCounted
{
public:
  void SetDebugName(const char* szName) const
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    m_sDebugName.Assign(szName);
#endif

    SetDebugNamePlatform(szName);
  }

  virtual const ezRHIResourceBase* GetParentResource() const { return this; }

protected:
  friend class ezRHIRenderDeviceImpl;

  inline ~ezRHIResourceBase()
  {
    EZ_ASSERT_DEV(m_hDefaultResourceView.IsInvalidated(), "");
    EZ_ASSERT_DEV(m_hDefaultRenderTargetView.IsInvalidated(), "");

    EZ_ASSERT_DEV(m_ResourceViews.IsEmpty(), "Dangling resource views");
    EZ_ASSERT_DEV(m_RenderTargetViews.IsEmpty(), "Dangling render target views");
    EZ_ASSERT_DEV(m_UnorderedAccessViews.IsEmpty(), "Dangling unordered access views");
  }

  virtual void SetDebugNamePlatform(const char* szName) const = 0;

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  mutable ezHashedString m_sDebugName;
#endif

  ezRHIResourceViewHandle m_hDefaultResourceView;
  ezRHIRenderTargetViewHandle m_hDefaultRenderTargetView;

  ezHashTable<ezUInt32, ezRHIResourceViewHandle> m_ResourceViews;
  ezHashTable<ezUInt32, ezRHIRenderTargetViewHandle> m_RenderTargetViews;
  ezHashTable<ezUInt32, ezRHIUnorderedAccessViewHandle> m_UnorderedAccessViews;
};

/// \brief Base class for GAL resources, stores a creation description of the object and also allows for reference counting.
template <typename CreationDescription>
class ezRHIResource : public ezRHIResourceBase
{
public:
  EZ_ALWAYS_INLINE ezRHIResource(const CreationDescription& Description)
    : m_Description(Description)
  {
  }

  EZ_ALWAYS_INLINE const CreationDescription& GetDescription() const { return m_Description; }

protected:
  const CreationDescription m_Description;
};
