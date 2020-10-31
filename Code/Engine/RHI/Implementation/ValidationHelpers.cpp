#include <RHI/ValidationHelpers.h>
#include <RHI/Util.h>

namespace ValidationHelpers
{
  void ValidateResourceSet(RHIGraphicsDevice* graphicsDevice, const RHIResourceSetDescription& description)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    ezDynamicArray<RHIResourceLayoutElementDescription> elements = description.Layout->GetDescription().Elements;
    ezDynamicArray<RHIResource*> resources = description.BoundResources;

    if (elements.GetCount() != resources.GetCount())
    {
      EZ_REPORT_FAILURE("The number of resources specified ({}) must be equal to the number of resources in the ResourceLayout ({}).", resources.GetCount(), elements.GetCount());
    }

    for (ezUInt32 i = 0; i < elements.GetCount(); i++)
    {
      ValidateResourceKind(elements[i].Kind, resources[i], i);
    }

    for (ezUInt32 i = 0; i < description.Layout->GetDescription().Elements.GetCount(); i++)
    {
      RHIResourceLayoutElementDescription element = description.Layout->GetDescription().Elements[i];
      if (element.Kind == RHIResourceKind::UniformBuffer || element.Kind == RHIResourceKind::StructuredBufferReadOnly || element.Kind == RHIResourceKind::StructuredBufferReadWrite)
      {
        RHIBufferRange range = Util::GetBufferRange(description.BoundResources[i], 0);

        if (!graphicsDevice->GetFeatures().BufferRangeBindingSupported() && (range.Offset != 0 || range.Size != range.Buffer->GetSize()))
        {
          EZ_REPORT_FAILURE("The {nameof(DeviceBufferRange)} in slot {i} uses a non-zero offset or less-than-full size, which requires {nameof(GraphicsDeviceFeatures)}.{nameof(GraphicsDeviceFeatures.BufferRangeBinding)}.");
        }

        ezUInt32 alignment = element.Kind == RHIResourceKind::UniformBuffer
                               ? graphicsDevice->GetUniformBufferMinOffsetAlignment()
                               : graphicsDevice->GetStructuredBufferMinOffsetAlignment();

        if ((range.Offset % alignment) != 0)
        {
          ezStringBuilder sb;
          sb.AppendFormat("The DeviceBufferRange in slot {} has an invalid offset: {range.Offset}. ", i, range.Offset);
          sb.AppendFormat("The offset for this buffer must be a multiple of {}.", alignment);
          EZ_REPORT_FAILURE(sb.GetData());
        }
      }
    }
#endif
  }

  void ValidateResourceKind(ezEnum<RHIResourceKind> kind, RHIResource* resource, ezUInt32 slot)
  {

    RHIBuffer* db = nullptr;
    bool dbValid = Util::GetDeviceBuffer(resource, db);
    switch (kind)
    {
      case RHIResourceKind::UniformBuffer:
      {
        if (!dbValid || (db->GetUsage() & RHIBufferUsage::UniformBuffer) == 0)
        {
          EZ_REPORT_FAILURE("Resource in slot {slot} does not match {nameof(ResourceKind)}.{kind} specified in the {nameof(ResourceLayout)}. It must be a {nameof(DeviceBuffer)} or {nameof(DeviceBufferRange)} with {nameof(BufferUsage)}.{nameof(BufferUsage.UniformBuffer)}.");
        }
        break;
      }
      case RHIResourceKind::StructuredBufferReadOnly:
      {
        if (!dbValid || (db->GetUsage() & (RHIBufferUsage::StructuredBufferReadOnly | RHIBufferUsage::StructuredBufferReadWrite)) == 0)
        {
          EZ_REPORT_FAILURE("Resource in slot {slot} does not match {nameof(ResourceKind)}.{kind} specified in the {nameof(ResourceLayout)}. It must be a {nameof(DeviceBuffer)} with {nameof(BufferUsage)}.{nameof(BufferUsage.StructuredBufferReadOnly)}.");
        }
        break;
      }
      case RHIResourceKind::StructuredBufferReadWrite:
      {
        if (!dbValid || (db->GetUsage() & RHIBufferUsage::StructuredBufferReadWrite) == 0)
        {
          EZ_REPORT_FAILURE("Resource in slot {slot} does not match {nameof(ResourceKind)} specified in the {nameof(ResourceLayout)}. It must be a {nameof(DeviceBuffer)} with {nameof(BufferUsage)}.{nameof(BufferUsage.StructuredBufferReadWrite)}.");
        }
        break;
      }
      case RHIResourceKind::TextureReadOnly:
      {
        RHITextureView* tv = dynamic_cast<RHITextureView*>(resource);
        RHITexture* t = dynamic_cast<RHITexture*>(resource);

        if (!(tv && (tv->GetTarget()->GetUsage() & RHITextureUsage::Sampled) != 0) && !(t && (t->GetUsage() & RHITextureUsage::Sampled) != 0))
        {
          ezStringBuilder sb;
          sb.AppendFormat("Resource in slot {} does not match ResourceKind specified in the ", slot);
          sb.AppendFormat("ResourceLayout. It must be a Texture or TextureView whose target ");
          sb.AppendFormat("has TextureUsage::Sampled.");
          EZ_REPORT_FAILURE(sb.GetData());
        }
        break;
      }
      case RHIResourceKind::TextureReadWrite:
      {
        RHITextureView* tv = dynamic_cast<RHITextureView*>(resource);
        RHITexture* t = dynamic_cast<RHITexture*>(resource);

        if (!(tv && (tv->GetTarget()->GetUsage() & RHITextureUsage::Storage) != 0) && !(t && (t->GetUsage() & RHITextureUsage::Storage) != 0))
        {
          ezStringBuilder sb;
          sb.AppendFormat("Resource in slot {} does not match ResourceKind specified in the ", slot);
          sb.AppendFormat("ResourceLayout. It must be a Texture or extureView whose target ");
          sb.AppendFormat("has TextureUsage::Storage.");
          EZ_REPORT_FAILURE(sb.GetData());
        }
        break;
      }
      case RHIResourceKind::Sampler:
      {
        if (!(dynamic_cast<RHISampler*>(resource)))
        {
          EZ_REPORT_FAILURE("Resource in slot {} does not match ResourceKind specified in the ResourceLayout. It must be a Sampler.", slot);
        }
        break;
      }
      default:
        EZ_REPORT_FAILURE("Invalid resource kind");
    }
  }
} // namespace ValidationHelpers
