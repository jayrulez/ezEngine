#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <RHI/Resources/DeviceResource.h>
#include <RHI/Resources/Framebuffer.h>
#include <RHI/Resources/ResourceLayout.h>
#include <RHI/Resources/Shader.h>
#include <RHI/State/BlendState.h>
#include <RHI/State/DepthStencilState.h>
#include <RHI/State/RasterizerState.h>

/// <summary>
/// Describes a graphics <see cref="RHIPipeline"/>, for creation using a <see cref="RHIResourceFactory"/>.
/// </summary>
struct EZ_RHI_DLL RHIGraphicsPipelineDescription : public ezHashableStruct<RHIGraphicsPipelineDescription>
{
  /// <summary>
  /// A description of the blend state, which controls how color values are blended into each color target.
  /// </summary>
  RHIBlendStateDescription BlendState;

  /// <summary>
  /// A description of the depth stencil state, which controls depth tests, writing, and comparisons.
  /// </summary>
  RHIDepthStencilStateDescription DepthStencilState;

  /// <summary>
  /// A description of the rasterizer state, which controls culling, clipping, scissor, and polygon-fill behavior.
  /// </summary>
  RHIRasterizerStateDescription RasterizerState;

  /// <summary>
  /// The <see cref="RHIPrimitiveTopology"/> to use, which controls how a series of input vertices is interpreted by the
  /// <see cref="RHIPipeline"/>.
  /// </summary>
  ezEnum<RHIPrimitiveTopology> PrimitiveTopology;

  /// <summary>
  /// A description of the shader set to be used.
  /// </summary>
  RHIShaderSetDescription ShaderSet;
  /// <summary>
  /// An array of <see cref="RHIResourceLayout"/>, which controls the layout of shader resources in the <see cref="RHIPipeline"/>.
  /// </summary>
  ezDynamicArray<RHIResourceLayout*> ResourceLayouts;

  /// <summary>
  /// A description of the output attachments used by the <see cref="RHIPipeline"/>.
  /// </summary>
  RHIOutputDescription Outputs;

  /// <summary>
  /// Specifies which model the rendering backend should use for binding resources.
  /// If <code>null</code>, the pipeline will use the value specified in <see cref="GraphicsDeviceOptions"/>.
  /// </summary>
  std::optional<ezEnum<RHIResourceBindingModel>> ResourceBindingModel;

  /// <summary>
  /// Constructs a new <see cref="GraphicsPipelineDescription"/>.
  /// </summary>
  /// <param name="blendState">A description of the blend state, which controls how color values are blended into each
  /// color target.</param>
  /// <param name="depthStencilStateDescription">A description of the depth stencil state, which controls depth tests,
  /// writing, and comparisons.</param>
  /// <param name="rasterizerState">A description of the rasterizer state, which controls culling, clipping, scissor, and
  /// polygon-fill behavior.</param>
  /// <param name="primitiveTopology">The <see cref="PrimitiveTopology"/> to use, which controls how a series of input
  /// vertices is interpreted by the <see cref="Pipeline"/>.</param>
  /// <param name="shaderSet">A description of the shader set to be used.</param>
  /// <param name="resourceLayouts">An array of <see cref="ResourceLayout"/>, which controls the layout of shader resoruces
  /// in the <see cref="Pipeline"/>.</param>
  /// <param name="outputs">A description of the output attachments used by the <see cref="Pipeline"/>.</param>
  RHIGraphicsPipelineDescription(
    const RHIBlendStateDescription& blendState,
    const RHIDepthStencilStateDescription& depthStencilStateDescription,
    const RHIRasterizerStateDescription rasterizerState,
    ezEnum<RHIPrimitiveTopology> primitiveTopology,
    const RHIShaderSetDescription& shaderSet,
    const ezDynamicArray<RHIResourceLayout*>& resourceLayouts,
    const RHIOutputDescription& outputs)
  {
    BlendState = blendState;
    DepthStencilState = depthStencilStateDescription;
    RasterizerState = rasterizerState;
    PrimitiveTopology = primitiveTopology;
    ShaderSet = shaderSet;
    ResourceLayouts = resourceLayouts;
    Outputs = outputs;
    ResourceBindingModel = std::nullopt;
  }

  /// <summary>
  /// Constructs a new <see cref="GraphicsPipelineDescription"/>.
  /// </summary>
  /// <param name="blendState">A description of the blend state, which controls how color values are blended into each
  /// color target.</param>
  /// <param name="depthStencilStateDescription">A description of the depth stencil state, which controls depth tests,
  /// writing, and comparisons.</param>
  /// <param name="rasterizerState">A description of the rasterizer state, which controls culling, clipping, scissor, and
  /// polygon-fill behavior.</param>
  /// <param name="primitiveTopology">The <see cref="PrimitiveTopology"/> to use, which controls how a series of input
  /// vertices is interpreted by the <see cref="Pipeline"/>.</param>
  /// <param name="shaderSet">A description of the shader set to be used.</param>
  /// <param name="resourceLayout">A <see cref="ResourceLayout"/>, which controls the layout of shader resoruces in the
  /// <see cref="Pipeline"/>.</param>
  /// <param name="outputs">A description of the output attachments used by the <see cref="Pipeline"/>.</param>
  RHIGraphicsPipelineDescription(
    const RHIBlendStateDescription& blendState,
    const RHIDepthStencilStateDescription& depthStencilStateDescription,
    const RHIRasterizerStateDescription rasterizerState,
    ezEnum<RHIPrimitiveTopology> primitiveTopology,
    const RHIShaderSetDescription& shaderSet,
    RHIResourceLayout* resourceLayout,
    const RHIOutputDescription& outputs)
  {
    BlendState = blendState;
    DepthStencilState = depthStencilStateDescription;
    RasterizerState = rasterizerState;
    PrimitiveTopology = primitiveTopology;
    ShaderSet = shaderSet;
    ResourceLayouts.PushBack(resourceLayout);
    Outputs = outputs;
    ResourceBindingModel = std::nullopt;
  }

  /// <summary>
  /// Constructs a new <see cref="GraphicsPipelineDescription"/>.
  /// </summary>
  /// <param name="blendState">A description of the blend state, which controls how color values are blended into each
  /// color target.</param>
  /// <param name="depthStencilStateDescription">A description of the depth stencil state, which controls depth tests,
  /// writing, and comparisons.</param>
  /// <param name="rasterizerState">A description of the rasterizer state, which controls culling, clipping, scissor, and
  /// polygon-fill behavior.</param>
  /// <param name="primitiveTopology">The <see cref="PrimitiveTopology"/> to use, which controls how a series of input
  /// vertices is interpreted by the <see cref="Pipeline"/>.</param>
  /// <param name="shaderSet">A description of the shader set to be used.</param>
  /// <param name="resourceLayouts">An array of <see cref="ResourceLayout"/>, which controls the layout of shader resoruces
  /// in the <see cref="Pipeline"/>.</param>
  /// <param name="outputs">A description of the output attachments used by the <see cref="Pipeline"/>.</param>
  /// <param name="resourceBindingModel">The <see cref="ResourceBindingModel"/> to use for this pipeline. Overrides
  /// the value specified in <see cref="GraphicsDeviceOptions"/>.</param>
  RHIGraphicsPipelineDescription(
    const RHIBlendStateDescription& blendState,
    const RHIDepthStencilStateDescription& depthStencilStateDescription,
    const RHIRasterizerStateDescription rasterizerState,
    ezEnum<RHIPrimitiveTopology> primitiveTopology,
    const RHIShaderSetDescription& shaderSet,
    const ezDynamicArray<RHIResourceLayout*>& resourceLayouts,
    const RHIOutputDescription& outputs,
    ezEnum<RHIResourceBindingModel> resourceBindingModel)
  {
    BlendState = blendState;
    DepthStencilState = depthStencilStateDescription;
    RasterizerState = rasterizerState;
    PrimitiveTopology = primitiveTopology;
    ShaderSet = shaderSet;
    ResourceLayouts = resourceLayouts;
    Outputs = outputs;
    ResourceBindingModel = std::make_optional(resourceBindingModel);
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIGraphicsPipelineDescription& other) const
  {
    return BlendState == other.BlendState &&
           DepthStencilState == other.DepthStencilState &&
           RasterizerState == other.RasterizerState &&
           PrimitiveTopology == other.PrimitiveTopology &&
           ShaderSet == other.ShaderSet &&
           ResourceLayouts == other.ResourceLayouts &&
           ResourceBindingModel == other.ResourceBindingModel &&
           Outputs == other.Outputs;
  }
};

struct EZ_RHI_DLL RHIComputePipelineDescription : public ezHashableStruct<RHIComputePipelineDescription>
{
  /// <summary>
  /// The compute <see cref="Shader"/> to be used in the Pipeline. This must be a Shader with
  /// <see cref="ShaderStages.Compute"/>.
  /// </summary>
  RHIShader* ComputeShader;
  /// <summary>
  /// An array of <see cref="ResourceLayout"/>, which controls the layout of shader resoruces in the <see cref="Pipeline"/>.
  /// </summary>
  ezDynamicArray<RHIResourceLayout*> ResourceLayouts;
  /// <summary>
  /// The X dimension of the thread group size.
  /// </summary>
  unsigned int ThreadGroupSizeX = 0;
  /// <summary>
  /// The Y dimension of the thread group size.
  /// </summary>
  unsigned int ThreadGroupSizeY = 0;
  /// <summary>
  /// The Z dimension of the thread group size.
  /// </summary>
  unsigned int ThreadGroupSizeZ = 0;
  /// <summary>
  /// An array of <see cref="SpecializationConstant"/> used to override specialization constants in the created
  /// <see cref="Pipeline"/>. Each element in this array describes a single ID-value pair, which will be matched with the
  /// constants specified in the <see cref="Shader"/>.
  /// </summary>
  ezDynamicArray<RHISpecializationConstant> Specializations;

  /// <summary>
  /// Constructs a new ComputePipelineDescription.
  /// </summary>
  /// <param name="computeShader">The compute <see cref="Shader"/> to be used in the Pipeline. This must be a Shader with
  /// <see cref="ShaderStages.Compute"/>.</param>
  /// <param name="resourceLayouts">The set of resource layouts available to the Pipeline.</param>
  /// <param name="threadGroupSizeX">The X dimension of the thread group size.</param>
  /// <param name="threadGroupSizeY">The Y dimension of the thread group size.</param>
  /// <param name="threadGroupSizeZ">The Z dimension of the thread group size.</param>
  RHIComputePipelineDescription(
    RHIShader* computeShader,
    ezDynamicArray<RHIResourceLayout*>& resourceLayouts,
    ezUInt32 threadGroupSizeX,
    ezUInt32 threadGroupSizeY,
    ezUInt32 threadGroupSizeZ)
  {
    ComputeShader = computeShader;
    ResourceLayouts = resourceLayouts;
    ThreadGroupSizeX = threadGroupSizeX;
    ThreadGroupSizeY = threadGroupSizeY;
    ThreadGroupSizeZ = threadGroupSizeZ;
    //Specializations.Clear();
  }

  /// <summary>
  /// Constructs a new ComputePipelineDescription.
  /// </summary>
  /// <param name="shaderStage">The compute <see cref="Shader"/> to be used in the Pipeline. This must be a Shader with
  /// <see cref="ShaderStages.Compute"/>.</param>
  /// <param name="resourceLayout">The resource layout available to the Pipeline.</param>
  /// <param name="threadGroupSizeX">The X dimension of the thread group size.</param>
  /// <param name="threadGroupSizeY">The Y dimension of the thread group size.</param>
  /// <param name="threadGroupSizeZ">The Z dimension of the thread group size.</param>
  RHIComputePipelineDescription(
    RHIShader* shaderStage,
    RHIResourceLayout* resourceLayout,
    ezUInt32 threadGroupSizeX,
    ezUInt32 threadGroupSizeY,
    ezUInt32 threadGroupSizeZ)
  {
    ComputeShader = shaderStage;
    ResourceLayouts.PushBack(resourceLayout);
    ThreadGroupSizeX = threadGroupSizeX;
    ThreadGroupSizeY = threadGroupSizeY;
    ThreadGroupSizeZ = threadGroupSizeZ;
    //Specializations.Clear();
  }

  /// <summary>
  /// Constructs a new ComputePipelineDescription.
  /// </summary>
  /// <param name="shaderStage">The compute <see cref="Shader"/> to be used in the Pipeline. This must be a Shader with
  /// <see cref="ShaderStages.Compute"/>.</param>
  /// <param name="resourceLayout">The resource layout available to the Pipeline.</param>
  /// <param name="threadGroupSizeX">The X dimension of the thread group size.</param>
  /// <param name="threadGroupSizeY">The Y dimension of the thread group size.</param>
  /// <param name="threadGroupSizeZ">The Z dimension of the thread group size.</param>
  /// <param name="specializations">An array of <see cref="SpecializationConstant"/> used to override specialization
  /// constants in the created <see cref="Pipeline"/>. Each element in this array describes a single ID-value pair, which
  /// will be matched with the constants specified in the <see cref="Shader"/>.</param>
  RHIComputePipelineDescription(
    RHIShader* shaderStage,
    RHIResourceLayout* resourceLayout,
    ezUInt32 threadGroupSizeX,
    ezUInt32 threadGroupSizeY,
    ezUInt32 threadGroupSizeZ,
    ezDynamicArray<RHISpecializationConstant>& specializations)
  {
    ComputeShader = shaderStage;
    ResourceLayouts.PushBack(resourceLayout);
    ThreadGroupSizeX = threadGroupSizeX;
    ThreadGroupSizeY = threadGroupSizeY;
    ThreadGroupSizeZ = threadGroupSizeZ;
    Specializations = specializations;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIComputePipelineDescription& other) const
  {
    return ComputeShader == other.ComputeShader &&
           ResourceLayouts == other.ResourceLayouts &&
           ThreadGroupSizeX == other.ThreadGroupSizeX &&
           ThreadGroupSizeY == other.ThreadGroupSizeY &&
           ThreadGroupSizeZ == other.ThreadGroupSizeZ;
  }
};

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
