#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/ResourceLayout.h>

class D3D11ResourceLayout : public RHIResourceLayout
{
public:
  struct ResourceBindingInfo
  {
    EZ_DECLARE_POD_TYPE();

    ezUInt32 Slot = 0;
    ezBitflags<RHIShaderStages> Stages;
    ezEnum<RHIResourceKind> Kind;
    bool DynamicBuffer = false;

    ResourceBindingInfo() = default;
    ResourceBindingInfo(ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezEnum<RHIResourceKind> kind, bool dynamicBuffer)
    {
      Slot = slot;
      Stages = stages;
      Kind = kind;
      DynamicBuffer = dynamicBuffer;
    }
  };

private:
  ezDynamicArray<ResourceBindingInfo> BindingInfosByRHIIndex;

  ezUInt32 UniformBufferCount = 0;
  ezUInt32 StorageBufferCount = 0;
  ezUInt32 TextureCount = 0;
  ezUInt32 SamplerCount = 0;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override
  {
    return Name;
  }

  virtual void SetName(const ezString& name) override
  {
    Name = name;
  }
  virtual bool IsDisposed() const override
  {
    return Disposed;
  }
  virtual void Dispose() override
  {
    if (!Disposed)
    {
      Disposed = true;
    }
  }

public:
  D3D11ResourceLayout(const RHIResourceLayoutDescription& description);

  ezUInt32 GetUniformBufferCount() const
  {
    return UniformBufferCount;
  }

  ezUInt32 GetStorageBufferCount() const
  {
    return StorageBufferCount;
  }

  ezUInt32 GetTextureCount() const
  {
    return TextureCount;
  }

  ezUInt32 GetSamplerCount() const
  {
    return SamplerCount;
  }

  bool IsDynamicBuffer(ezUInt32 index) const
  {
    return BindingInfosByRHIIndex[index].DynamicBuffer;
  }

  ResourceBindingInfo& GetDeviceSlotIndex(ezUInt32 resourceLayoutIndex);

  virtual ezUInt32 GetHash() const override
  {
    ezUInt32 hash = 0;

    // TODO: hash

    return hash;
  }
};
