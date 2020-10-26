#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

/// <summary>
/// Describes a 3-dimensional region.
/// </summary>
struct EZ_RHI_DLL RHIViewport : ezHashableStruct<RHIViewport>
{
  /// <summary>
  /// The minimum X value.
  /// </summary>

  float X;
  /// <summary>
  /// The minimum Y value.
  /// </summary>

  float Y;
  /// <summary>
  /// The width.
  /// </summary>

  float Width;
  /// <summary>
  /// The height.
  /// </summary>

  float Height;
  /// <summary>
  /// The minimum depth.
  /// </summary>

  float MinDepth;
  /// <summary>
  /// The maximum depth.
  /// </summary>

  float MaxDepth;

  /// <summary>
  /// Constructs a new Viewport.
  /// </summary>
  /// <param name="x">The minimum X value.</param>
  /// <param name="y">The minimum Y value.</param>
  /// <param name="width">The width.</param>
  /// <param name="height">The height.</param>
  /// <param name="minDepth">The minimum depth.</param>
  /// <param name="maxDepth">The maximum depth.</param>
  RHIViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
  {
    X = x;
    Y = y;
    Width = width;
    Height = height;
    MinDepth = minDepth;
    MaxDepth = maxDepth;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIViewport& other) const
  {
    return X == other.X && Y == other.Y && Width == other.Width && Height == other.Height && MinDepth == other.MinDepth && MaxDepth == other.MaxDepth;
  }
};