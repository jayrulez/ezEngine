#include <RHI/Backends/D3D11/D3D11ResourceLayout.h>

D3D11ResourceLayout::D3D11ResourceLayout(const RHIResourceLayoutDescription& description)
  : RHIResourceLayout(description)
{
  ezDynamicArray<RHIResourceLayoutElementDescription> elements = description.Elements;
  BindingInfosByRHIIndex.SetCountUninitialized(elements.GetCount());

  ezUInt32 cbIndex = 0;
  ezUInt32 texIndex = 0;
  ezUInt32 samplerIndex = 0;
  ezUInt32 unorderedAccessIndex = 0;

  for (ezUInt32 i = 0; i < BindingInfosByRHIIndex.GetCount(); i++)
  {
    ezUInt32 slot;
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

D3D11ResourceLayout::ResourceBindingInfo& D3D11ResourceLayout::GetDeviceSlotIndex(ezUInt32 resourceLayoutIndex)
{
  if (resourceLayoutIndex >= BindingInfosByRHIIndex.GetCount())
  {
    EZ_REPORT_FAILURE("Invalid resource index: {}. Maximum is: {}.", resourceLayoutIndex, BindingInfosByRHIIndex.GetCount() - 1);
  }

  return BindingInfosByRHIIndex[resourceLayoutIndex];
}


EZ_STATICLINK_FILE(RHI, RHI_Backends_D3D11_Implementation_D3D11ResourceLayout);

