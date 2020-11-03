#include <RHI/Descriptors/OutputDescription.h>
#include <RHI/Resources/Framebuffer.h>

RHIOutputDescription RHIOutputDescription::CreateFromFramebuffer(RHIFramebuffer* framebuffer)
{
  ezEnum<RHITextureSampleCount> sampleCount = RHITextureSampleCount::Count1;
  std::optional<RHIOutputAttachmentDescription> depthAttachment;

  if (framebuffer->GetDepthTarget())
  {
    depthAttachment = RHIOutputAttachmentDescription(framebuffer->GetDepthTarget().value().GetTarget()->GetFormat());
    sampleCount = framebuffer->GetDepthTarget().value().GetTarget()->GetSampleCount();
  }

  ezDynamicArray<RHIOutputAttachmentDescription> colorAttachments;
  colorAttachments.SetCountUninitialized(framebuffer->GetColorTargets().GetCount());
  for (ezUInt32 i = 0; i < framebuffer->GetColorTargets().GetCount(); i++)
  {
    colorAttachments[i] = RHIOutputAttachmentDescription(framebuffer->GetColorTargets()[i].GetTarget()->GetFormat());
    sampleCount = framebuffer->GetColorTargets()[i].GetTarget()->GetSampleCount();
  }

  return RHIOutputDescription(depthAttachment, colorAttachments, sampleCount);
}


EZ_STATICLINK_FILE(RHI, RHI_Descriptors_Implementation_OutputDescription);

