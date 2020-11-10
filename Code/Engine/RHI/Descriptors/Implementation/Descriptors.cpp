#include <RHI/Descriptors/Descriptors.h>
#include <RHI/Resources/Resources.h>

ezUInt32 ezHashHelper<PipelineStateDesc>::Hash(const PipelineStateDesc& value)
{
  ezUInt32 combinedHash = 0;

  combinedHash = ezHashingUtils::xxHash32(value.ms, sizeof(Shader), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(value.as, sizeof(Shader), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(value.vs, sizeof(Shader), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(value.ps, sizeof(Shader), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(value.hs, sizeof(Shader), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(value.ds, sizeof(Shader), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(value.gs, sizeof(Shader), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(value.il, sizeof(InputLayout), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(value.rs, sizeof(RasterizerState), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(value.bs, sizeof(BlendState), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(value.dss, sizeof(DepthStencilState), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(&value.pt, sizeof(ezRHIPrimitiveTopology::Enum), combinedHash);
  combinedHash = ezHashingUtils::xxHash32(&value.sampleMask, sizeof(ezUInt32), combinedHash);

  return combinedHash;
}

ezUInt32 ezHashHelper<RenderPassDesc>::Hash(const RenderPassDesc& value)
{
  ezUInt32 combinedHash = value.attachments.size();

  for (auto& attachment : value.attachments)
  {
    combinedHash = ezHashingUtils::xxHash32(&attachment.texture->desc.Format, sizeof(ezRHIFormat::Enum), combinedHash);
    combinedHash = ezHashingUtils::xxHash32(&attachment.texture->desc.SampleCount, sizeof(ezUInt32), combinedHash);
  }


  return combinedHash;
}
