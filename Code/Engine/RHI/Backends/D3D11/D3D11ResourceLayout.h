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
    ResourceBindingInfo(ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezEnum<RHIResourceKind> kind, bool dynamicBuffer);
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
  virtual ezString GetName() const override;

  virtual void SetName(const ezString& name) override;
  virtual bool IsDisposed() const override;
  virtual void Dispose() override;

public:
  D3D11ResourceLayout(const RHIResourceLayoutDescription& description);
  ezUInt32 GetUniformBufferCount() const;
  ezUInt32 GetStorageBufferCount() const;
  ezUInt32 GetTextureCount() const;
  ezUInt32 GetSamplerCount() const;
  bool IsDynamicBuffer(ezUInt32 index) const;
  ResourceBindingInfo& GetDeviceSlotIndex(ezUInt32 resourceLayoutIndex);
};
