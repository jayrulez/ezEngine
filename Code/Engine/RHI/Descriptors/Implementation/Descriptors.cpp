#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Resources/Resources.h>

ezUInt32 ezHashHelper<PipelineStateDesc>::Hash(const PipelineStateDesc& value)
{
  ezUInt32 combinedHash = 0;

  if (value.ms != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.ms, sizeof(Shader), combinedHash);
  if (value.as != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.as, sizeof(Shader), combinedHash);
  if (value.vs != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.vs, sizeof(Shader), combinedHash);
  if (value.ps != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.ps, sizeof(Shader), combinedHash);
  if (value.hs != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.hs, sizeof(Shader), combinedHash);
  if (value.ds != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.ds, sizeof(Shader), combinedHash);
  if (value.gs != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.gs, sizeof(Shader), combinedHash);
  if (value.il != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.il, sizeof(InputLayout), combinedHash);
  if (value.rs != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.rs, sizeof(RasterizerState), combinedHash);
  if (value.bs != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.bs, sizeof(BlendState), combinedHash);
  if (value.ds != nullptr)
    combinedHash = ezHashingUtils::xxHash32(value.dss, sizeof(DepthStencilState), combinedHash);

  combinedHash = ezHashingUtils::xxHash32(&value.pt, sizeof(ezRHIPrimitiveTopology::Enum), combinedHash);

  combinedHash = ezHashingUtils::xxHash32(&value.sampleMask, sizeof(ezUInt32), combinedHash);

  return combinedHash;
}

ezUInt32 ezHashHelper<RenderPassDesc>::Hash(const RenderPassDesc& value)
{
  ezUInt32 combinedHash = value.attachments.GetCount();

  for (auto& attachment : value.attachments)
  {
    if (attachment.texture != nullptr)
    {
      combinedHash = ezHashingUtils::xxHash32(&attachment.texture->desc.Format, sizeof(ezRHIFormat::Enum), combinedHash);
      combinedHash = ezHashingUtils::xxHash32(&attachment.texture->desc.SampleCount, sizeof(ezUInt32), combinedHash);
    }
  }


  return combinedHash;
}
