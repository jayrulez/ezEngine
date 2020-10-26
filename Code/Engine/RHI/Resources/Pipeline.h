#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/GraphicsPipelineDescription.h>
#include <RHI/Descriptors/ComputePipelineDescription.h>
#include <RHI/Descriptors/OutputDescription.h>

#include <RHI/Resources/DeviceResource.h>
#include <RHI/Resources/ResourceLayout.h>

/// <summary>
/// A device resource encapsulating all state in a graphics pipeline. Used in
/// <see cref="CommandList.SetPipeline(Pipeline)"/> to prepare a <see cref="CommandList"/> for draw commands.
/// See <see cref="GraphicsPipelineDescription"/>.
/// </summary>
class EZ_RHI_DLL RHIPipeline : public RHIDeviceResource
{
public:
  RHIPipeline(const ezDynamicArray<RHIResourceLayout*>& resourceLayouts)
  {
#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
    ResourceLayouts.SetCountUninitialized(resourceLayouts.GetCount());
    ResourceLayouts.GetArrayPtr().CopyFrom(resourceLayouts.GetArrayPtr());
#endif
  }

  RHIPipeline(const RHIGraphicsPipelineDescription& graphicsDescription)
    : RHIPipeline(graphicsDescription.ResourceLayouts)
  {
  }

  RHIPipeline(const RHIComputePipelineDescription& computeDescription)
    : RHIPipeline(computeDescription.ResourceLayouts)
  {
  }

  /// <summary>
  /// Gets a value indicating whether this instance represents a compute Pipeline.
  /// If false, this instance is a graphics pipeline.
  /// </summary>
  virtual bool IsComputePipeline() const = 0;

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  RHIOutputDescription GetGraphicsOutputDescription() const
  {
    return GraphicsOutputDescription;
  }

  ezDynamicArray<RHIResourceLayout*> GetResourceLayouts() const
  {
    return ResourceLayouts;
  }
#endif

private:
  RHIOutputDescription GraphicsOutputDescription;
  ezDynamicArray<RHIResourceLayout*> ResourceLayouts;
};
