#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// A <see cref="RHIPipeline"/> component describing the properties of the rasterizer.
/// </summary>
struct EZ_RHI_DLL RHIRasterizerStateDescription : public ezHashableStruct<RHIRasterizerStateDescription>
{
  EZ_DECLARE_POD_TYPE();

  /// <summary>
  /// Controls which face will be culled.
  /// </summary>
  ezEnum<RHIFaceCullMode> CullMode = RHIFaceCullMode::Back;

  /// <summary>
  /// Controls how the rasterizer fills polygons.
  /// </summary>
  ezEnum<RHIPolygonFillMode> FillMode = RHIPolygonFillMode::Solid;

  /// <summary>
  /// Controls the winding order used to determine the front face of primitives.
  /// </summary>
  ezEnum<RHIFrontFace> FrontFace = RHIFrontFace::Clockwise;

  /// <summary>
  /// Controls whether depth clipping is enabled.
  /// </summary>
  bool DepthClipEnabled = false;

  /// <summary>
  /// Controls whether the scissor test is enabled.
  /// </summary>
  bool ScissorTestEnabled = false;

  RHIRasterizerStateDescription() = default;

  /// <summary>
  /// Constructs a new RasterizerStateDescription.
  /// </summary>
  /// <param name="cullMode">Controls which face will be culled.</param>
  /// <param name="fillMode">Controls how the rasterizer fills polygons.</param>
  /// <param name="frontFace">Controls the winding order used to determine the front face of primitives.</param>
  /// <param name="depthClipEnabled">Controls whether depth clipping is enabled.</param>
  /// <param name="scissorTestEnabled">Controls whether the scissor test is enabled.</param>
  RHIRasterizerStateDescription(
    ezEnum<RHIFaceCullMode> cullMode,
    ezEnum<RHIPolygonFillMode> fillMode,
    ezEnum<RHIFrontFace> frontFace,
    bool depthClipEnabled,
    bool scissorTestEnabled)
  {
    CullMode = cullMode;
    FillMode = fillMode;
    FrontFace = frontFace;
    DepthClipEnabled = depthClipEnabled;
    ScissorTestEnabled = scissorTestEnabled;
  }

  /// <summary>
  /// Describes the default rasterizer state, with clockwise backface culling, solid polygon filling, and both depth
  /// clipping and scissor tests enabled.
  /// Settings:
  ///     CullMode = FaceCullMode.Back
  ///     FillMode = PolygonFillMode.Solid
  ///     FrontFace = FrontFace.Clockwise
  ///     DepthClipEnabled = true
  ///     ScissorTestEnabled = false
  /// </summary>
  static const RHIRasterizerStateDescription Default;

  /// <summary>
  /// Describes a rasterizer state with no culling, solid polygon filling, and both depth
  /// clipping and scissor tests enabled.
  /// Settings:
  ///     CullMode = FaceCullMode.None
  ///     FillMode = PolygonFillMode.Solid
  ///     FrontFace = FrontFace.Clockwise
  ///     DepthClipEnabled = true
  ///     ScissorTestEnabled = false
  /// </summary>
  static const RHIRasterizerStateDescription CullNone;

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIRasterizerStateDescription& other) const
  {
    return CullMode == other.CullMode && FillMode == other.FillMode && FrontFace == other.FrontFace && DepthClipEnabled == other.DepthClipEnabled && ScissorTestEnabled == other.ScissorTestEnabled;
  }
};
