#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <RHI/Resources/DeviceResource.h>
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
  //ResourceLayout[] ResourceLayouts;
};
