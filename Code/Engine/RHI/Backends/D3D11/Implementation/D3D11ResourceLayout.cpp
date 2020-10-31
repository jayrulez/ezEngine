#include <RHI/Backends/D3D11/D3D11ResourceLayout.h>

D3D11ResourceLayout::ResourceBindingInfo::ResourceBindingInfo(ezUInt32 slot, ezBitflags<RHIShaderStages> stages, ezEnum<RHIResourceKind> kind, bool dynamicBuffer)
{
  Slot = slot;
  Stages = stages;
  Kind = kind;
  DynamicBuffer = dynamicBuffer;
}

ezString D3D11ResourceLayout::GetName() const
{
  return Name;
}

void D3D11ResourceLayout::SetName(const ezString& name)
{
  Name = name;
}

bool D3D11ResourceLayout::IsDisposed() const
{
  return Disposed;
}

void D3D11ResourceLayout::Dispose()
{
  if (!Disposed)
  {
    Disposed = true;
  }
}

D3D11ResourceLayout::D3D11ResourceLayout(const RHIResourceLayoutDescription& description)
  : RHIResourceLayout(description)
{
  ezDynamicArray<RHIResourceLayoutElementDescription> elements = description.Elements;
  BindingInfosByRHIIndex.SetCountUninitialized(elements.GetCount());

  int cbIndex = 0;
  int texIndex = 0;
  int samplerIndex = 0;
  int unorderedAccessIndex = 0;

  for (ezUInt32 i = 0; i < BindingInfosByRHIIndex.GetCount(); i++)
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
        return;
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

ezUInt32 D3D11ResourceLayout::GetUniformBufferCount() const
{
  return UniformBufferCount;
}

ezUInt32 D3D11ResourceLayout::GetStorageBufferCount() const
{
  return StorageBufferCount;
}

ezUInt32 D3D11ResourceLayout::GetTextureCount() const
{
  return TextureCount;
}

ezUInt32 D3D11ResourceLayout::GetSamplerCount() const
{
  return SamplerCount;
}

bool D3D11ResourceLayout::IsDynamicBuffer(ezUInt32 index) const
{
  return BindingInfosByRHIIndex[index].DynamicBuffer;
}

D3D11ResourceLayout::ResourceBindingInfo& D3D11ResourceLayout::GetDeviceSlotIndex(ezUInt32 resourceLayoutIndex)
{
  if (resourceLayoutIndex >= BindingInfosByRHIIndex.GetCount())
  {
    EZ_REPORT_FAILURE("Invalid resource index: {}. Maximum is: {}.", resourceLayoutIndex, BindingInfosByRHIIndex.GetCount() - 1);
  }

  return BindingInfosByRHIIndex[resourceLayoutIndex];
}
