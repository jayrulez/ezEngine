#include <RHI/Resources/Pipeline.h>

RHIPipeline::RHIPipeline(const ezDynamicArray<RHIResourceLayout*>& resourceLayouts)
{
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  ResourceLayouts.SetCountUninitialized(resourceLayouts.GetCount());
  ResourceLayouts.GetArrayPtr().CopyFrom(resourceLayouts.GetArrayPtr());
#endif
}

RHIPipeline::RHIPipeline(const RHIGraphicsPipelineDescription& graphicsDescription)
  : RHIPipeline(graphicsDescription.ResourceLayouts)
{
}

RHIPipeline::RHIPipeline(const RHIComputePipelineDescription& computeDescription)
  : RHIPipeline(computeDescription.ResourceLayouts)
{
}

RHIOutputDescription RHIPipeline::GetGraphicsOutputDescription() const
{
  return GraphicsOutputDescription;
}

ezDynamicArray<RHIResourceLayout*> RHIPipeline::GetResourceLayouts() const
{
  return ResourceLayouts;
}
