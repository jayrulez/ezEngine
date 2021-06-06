#pragma once

#include <Foundation/Containers/HashTable.h>
#include <Foundation/Strings/HashedString.h>
#include <RHI/RHIDLL.h>

struct ezRHIGPUResourceType
{
  typedef ezUInt8 StorageType;

  enum Enum
  {
    Buffer = 0,
    Texture = 1,
    ResourceView = 2,
    RenderTargetView = 3,
    UnorderedAccessView = 4,
    RayTracingAccelerationStructure = 5,
    Unknown,

    ENUM_COUNT,

    Default = Unknown
  };
};

class EZ_RHI_DLL ezRHIResourceBase : public ezRHIObject
{
public:
  void SetDebugName(const char* name) const
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    m_sDebugName.Assign(name);
#endif

    SetDebugNamePlatform(name);
  }

  virtual const ezRHIResourceBase* GetParentResource() const { return this; }

protected:
  friend class ezRHIDevice;

  inline ~ezRHIResourceBase()
  {
  }

  virtual void SetDebugNamePlatform(const char* name) const = 0;

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  mutable ezHashedString m_sDebugName;
#endif

  ezRHIResourceView* m_pDefaultResourceView;
  ezRHIRenderTargetView* m_pDefaultRenderTargetView;
  ezRHIUnorderedAccessView* m_pDefaultUnorderedAccessView;

  ezDynamicArray<ezRHIResourceView*> m_pResourceViews;
  ezDynamicArray<ezRHIRenderTargetView*> m_pRenderTargetViews;
  ezDynamicArray<ezRHIUnorderedAccessView*> m_pUnorderedAccessViews;
};

template <typename CreationDesc>
class EZ_RHI_DLL ezRHIResource : public ezRHIResourceBase
{
public:
  EZ_ALWAYS_INLINE ezRHIResource(const CreationDesc& desc, ezRHIGPUResourceType::Enum type)
  {
    m_Type = type;
  }

  inline bool IsTexture() const { return m_Type == ezRHIGPUResourceType::Texture; }
  inline bool IsBuffer() const { return m_Type == ezRHIGPUResourceType::Buffer; }
  inline bool IsAccelerationStructure() const { return m_Type == ezRHIGPUResourceType::RayTracingAccelerationStructure; }

  EZ_ALWAYS_INLINE const CreationDesc& GetDescription() const { return m_Desc; }

protected:
  CreationDesc m_Desc;
  ezRHIGPUResourceType::Enum m_Type = ezRHIGPUResourceType::Unknown;
};
