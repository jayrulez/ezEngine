#pragma once

#include <RHI/Descriptors/BlendStateDescription.h>
#include <RHI/Descriptors/DepthStencilStateDescription.h>
#include <RHI/Descriptors/OutputDescription.h>
#include <RHI/Descriptors/RasterizerStateDescription.h>
#include <RHI/Descriptors/ShaderSetDescription.h>
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <RHI/Util.h>

#include <optional>

class RHIResourceLayout;

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
    const ezDynamicArray<RHIResourceLayout*> resourceLayouts,
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
           Util::AreEqual(ResourceLayouts, other.ResourceLayouts) &&
           ResourceBindingModel == other.ResourceBindingModel &&
           Outputs == other.Outputs;
  }
};
