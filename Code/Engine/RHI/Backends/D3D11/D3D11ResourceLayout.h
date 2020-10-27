#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <RHI/Resources/ResourceLayout.h>

struct ResourceBindingInfo
{
  ezUInt32 Slot = 0;
  ezBitflags<RHIShaderStages> Stages;
  ezEnum<RHIResourceKind> Kind;
  bool DynamicBuffer = false;

  ResourceBindingInfo(ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezEnum<RHIResourceKind> kind, bool dynamicBuffer)
  {
    Slot = slot;
    Stages = stages;
    Kind = kind;
    DynamicBuffer = dynamicBuffer;
  }
};

class D3D11ResourceLayout : public RHIResourceLayout
{
private:
  ezDynamicArray<ResourceBindingInfo> BindingInfosByRHIIndex;

  ezUInt32 UniformBufferCount = 0;
  ezUInt32 StorageBufferCount = 0;
  ezUInt32 TextureCount = 0;
  ezUInt32 SamplerCount = 0;

  bool Disposed = false;
  ezString Name;

public:
  virtual ezString GetName() const override { return Name; }

  virtual void SetName(const ezString& name) override
  {
    Name = name;
  }
  virtual bool IsDisposed() const override { return Disposed; }
  virtual void Dispose() override
  {
    if (!Disposed)
    {
      Disposed = true;
    }
  }

public:
  D3D11ResourceLayout(const RHIResourceLayoutDescription& description)
    : RHIResourceLayout(description)
  {
    ezDynamicArray<RHIResourceLayoutElementDescription> elements = description.Elements;
    BindingInfosByRHIIndex.SetCountUninitialized(elements.GetCount());

    int cbIndex = 0;
    int texIndex = 0;
    int samplerIndex = 0;
    int unorderedAccessIndex = 0;

    for (int i = 0; i < BindingInfosByRHIIndex.GetCount(); i++)
    {
      int slot;
      switch (elements[i].Kind)
      {
        case RHIResourceKind::UniformBuffer:
          slot = cbIndex++;
          break;
        case RHIResourceKind::StructuredBufferReadOnly:
          slot = texIndex++;
          break;
        case RHIResourceKind::StructuredBufferReadWrite:
          slot = unorderedAccessIndex++;
          break;
        case RHIResourceKind::TextureReadOnly:
          slot = texIndex++;
          break;
        case RHIResourceKind::TextureReadWrite:
          slot = unorderedAccessIndex++;
          break;
        case RHIResourceKind::Sampler:
          slot = samplerIndex++;
          break;
        default:
          EZ_REPORT_FAILURE("Invalid RHIResourceKind");
      }

      BindingInfosByRHIIndex[i] = ResourceBindingInfo(
        slot,
        elements[i].Stages,
        elements[i].Kind,
        (elements[i].Options & RHIResourceLayoutElementOptions::DynamicBinding) != 0);
    }

    UniformBufferCount = cbIndex;
    StorageBufferCount = unorderedAccessIndex;
    TextureCount = texIndex;
    SamplerCount = samplerIndex;
  }
  ezUInt32 GetUniformBufferCount() const { return UniformBufferCount; }
  ezUInt32 GetStorageBufferCount() const { return StorageBufferCount; }
  ezUInt32 GetTextureCount() const { return TextureCount; }
  ezUInt32 GetSamplerCount() const { return SamplerCount; }
  bool IsDynamicBuffer(ezUInt32 index) const { return BindingInfosByRHIIndex[index].DynamicBuffer; }
  ResourceBindingInfo& GetDeviceSlotIndex(ezUInt32 resourceLayoutIndex)
  {
    if (resourceLayoutIndex >= BindingInfosByRHIIndex.GetCount())
    {
      EZ_REPORT_FAILURE("Invalid resource index: {}. Maximum is: {}.", resourceLayoutIndex, BindingInfosByRHIIndex.GetCount() - 1);
    }

    return BindingInfosByRHIIndex[resourceLayoutIndex];
  }
};
