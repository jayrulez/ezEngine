#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <Foundation/Math/Rect.h>


struct EZ_RHI_DLL RHIViewportScaling // : byte
{
  using StorageType = ezUInt8;
  enum Enum
  {
    None,
    Stretch,
    AspectRatioStretch,
    Default = None
  };
};


/// <summary>
/// Describes a 3-dimensional region.
/// </summary>
struct EZ_RHI_DLL RHIViewport : public ezHashableStruct<RHIViewport>
{
  /// <summary>
  /// The minimum X value.
  /// </summary>

  float X = 0;
  /// <summary>
  /// The minimum Y value.
  /// </summary>

  float Y = 0;
  /// <summary>
  /// The width.
  /// </summary>

  float Width = 0;
  /// <summary>
  /// The height.
  /// </summary>

  float Height = 0;
  /// <summary>
  /// The minimum depth.
  /// </summary>

  float MinDepth = 0;
  /// <summary>
  /// The maximum depth.
  /// </summary>

  float MaxDepth = 0;

  /// <summary>
  /// Initializes a new instance of the <see cref="Viewport"/> struct.
  /// </summary>
  /// <param name="width">The width of the viewport in pixels.</param>
  /// <param name="height">The height of the viewport in pixels.</param>
  RHIViewport(float width, float height)
  {
    X = 0.0f;
    Y = 0.0f;
    Width = width;
    Height = height;
    MinDepth = 0.0f;
    MaxDepth = 1.0f;
  }

  /// <summary>
  /// Initializes a new instance of the <see cref="Viewport"/> struct.
  /// </summary>
  /// <param name="x">The x coordinate of the upper-left corner of the viewport in pixels.</param>
  /// <param name="y">The y coordinate of the upper-left corner of the viewport in pixels.</param>
  /// <param name="width">The width of the viewport in pixels.</param>
  /// <param name="height">The height of the viewport in pixels.</param>
  RHIViewport(float x, float y, float width, float height)
  {
    X = x;
    Y = y;
    Width = width;
    Height = height;
    MinDepth = 0.0f;
    MaxDepth = 1.0f;
  }

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
  /// Initializes a new instance of the <see cref="Viewport"/> struct.
  /// </summary>
  /// <param name="bounds">A <see cref="Rectangle"/> that defines the location and size of the viewport in a render target.</param>
  //RHIViewport(Rectangle bounds)
  //  {
  //    X = bounds.X;
  //    Y = bounds.Y;
  //    Width = bounds.Width;
  //    Height = bounds.Height;
  //    MinDepth = 0.0f;
  //    MaxDepth = 1.0f;
  //  }

  /// <summary>
  /// Initializes a new instance of the <see cref="Viewport"/> struct.
  /// </summary>
  /// <param name="bounds">A <see cref="RectangleF"/> that defines the location and size of the viewport in a render target.</param>
  RHIViewport(const ezRectFloat& bounds)
  {
    X = bounds.x;
    Y = bounds.y;
    Width = bounds.width;
    Height = bounds.height;
    MinDepth = 0.0f;
    MaxDepth = 1.0f;
  }

  /// <summary>
  /// Initializes a new instance of the <see cref="Viewport"/> struct.
  /// </summary>
  /// <param name="bounds">A <see cref="Vector4"/> that defines the location and size of the viewport in a render target.</param>
  RHIViewport(ezVec4 bounds)
  {
    X = bounds.x;
    Y = bounds.y;
    Width = bounds.z;
    Height = bounds.w;
    MinDepth = 0.0f;
    MaxDepth = 1.0f;
  }

  /// <summary>
  /// Gets the bounds of the viewport.
  /// </summary>
  /// <value>The bounds.</value>
  ezRectFloat GetBounds() const
  {
    return ezRectFloat(X, Y, Width, Height);
  }

  /// <summary>
  /// Sets the bounds of the viewport.
  /// </summary>
  /// <value>The bounds.</value>
  void SetBounds(const ezRectFloat& bounds)
  {
    X = bounds.x;
    Y = bounds.y;
    Width = bounds.width;
    Height = bounds.height;
  }

  /// <summary>
  /// Gets the aspect ratio used by the viewport.
  /// </summary>
  /// <value>The aspect ratio.</value>
  float GetAspectRatio() const
  {
    if (!ezMath::IsZero<float>(Height, ezMath::DefaultEpsilon<float>()))
    {
      return Width / Height;
    }

    return 0.0f;
  }

  //ezVec3 Project(const ezVec3& source, const ezMat4& worldViewProjection)
  //{
  //  float halfViewportWidth = Width * 0.5f;
  //  float halfViewportHeight = Height * 0.5f;

  //  ezVec3 scale(halfViewportWidth, -halfViewportHeight, MaxDepth - MinDepth);
  //  ezVec3 offset(X + halfViewportWidth, Y + halfViewportHeight, MinDepth);

  //  var result = Vector3.Transform(source, worldViewProjection);
  //  result = VectorEx.MultiplyAdd(result, scale, offset);
  //  return result;
  //}

  //ezVec3 Project(const ezVec3& source, const ezMat4& projection, const ezMat4& view, const ezMat4& world)
  //{
  //  float halfViewportWidth = Width * 0.5f;
  //  float halfViewportHeight = Height * 0.5f;

  //  Vector3 scale(halfViewportWidth, -halfViewportHeight, MaxDepth - MinDepth);
  //  Vector3 offset(X + halfViewportWidth, Y + halfViewportHeight, MinDepth);

  //  Matrix4x4 transform = Matrix4x4.Multiply(world, view);
  //  transform = Matrix4x4.Multiply(transform, projection);

  //  var result = Vector3.Transform(source, transform);
  //  result = VectorEx.MultiplyAdd(result, scale, offset);
  //  return result;
  //}

  //ezVec3 Unproject(const ezVec3& source, const ezMat4& projection, const ezMat4& view, const ezMat4& world)
  //{
  //  ezVec3 scale(Width * 0.5f, -Height * 0.5f, MaxDepth - MinDepth);
  //  scale = Vector3.Divide(Vector3.One, scale);

  //  ezVec3 offset = new Vector3(-X, -Y, -MinDepth);
  //  offset = VectorEx.MultiplyAdd(scale, offset, new Vector3(-1.0f, 1.0f, 0.0f));

  //  Matrix4x4 transform = Matrix4x4.Multiply(world, view);
  //  transform = Matrix4x4.Multiply(transform, projection);
  //  Matrix4x4.Invert(transform, out transform);

  //  Vector3 result = VectorEx.MultiplyAdd(source, scale, offset);
  //  return Vector3.Transform(result, transform);
  //}

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIViewport& other) const
  {
    return X == other.X &&
           Y == other.Y &&
           Width == other.Width &&
           Height == other.Height &&
           MinDepth == other.MinDepth &&
           MaxDepth == other.MaxDepth;
  }

  bool operator!=(const RHIViewport& other) const
  {
    return !(*this == other);
  }

  ///////////////////Statics//////////////////////
  //static RectangleF ComputeDisplayArea(ViewportScaling scaling, int backBufferWidth, int backBufferHeight, int outputWidth, int outputHeight)
  //{
  //  switch (scaling)
  //  {
  //    case ViewportScaling.Stretch:
  //      // Output fills the entire window area
  //      return new RectangleF(outputWidth, outputHeight);

  //    case ViewportScaling.AspectRatioStretch:
  //      // Output fills the window area but respects the original aspect ratio, using pillar boxing or letter boxing as required
  //      // Note: This scaling option is not supported for legacy Win32 windows swap chains
  //      {
  //        Debug.Assert(backBufferHeight > 0);
  //        float aspectRatio = (float)backBufferWidth / backBufferHeight;

  //        // Horizontal fill
  //        float scaledWidth = outputWidth;
  //        float scaledHeight = outputWidth / aspectRatio;
  //        if (scaledHeight >= outputHeight)
  //        {
  //          // Do vertical fill
  //          scaledWidth = outputHeight * aspectRatio;
  //          scaledHeight = outputHeight;
  //        }

  //        float offsetX = (outputWidth - scaledWidth) * 0.5f;
  //        float offsetY = (outputHeight - scaledHeight) * 0.5f;

  //        var result = new RectangleF(offsetX, offsetY, scaledWidth, scaledHeight);

  //        // Clip to display window
  //        result.X = Math.Max(0, result.X);
  //        result.Y = Math.Max(0, result.Y);
  //        result.Width = Math.Min(outputWidth, result.Width);
  //        result.Height = Math.Min(outputHeight, result.Height);
  //        return result;
  //      }

  //    case ViewportScaling.None:
  //    default:
  //      // Output is displayed in the upper left corner of the window area
  //      return new RectangleF(Math.Min(backBufferWidth, outputWidth), Math.Min(backBufferHeight, outputHeight));
  //  }
  //}

  //static RectangleF ComputeTitleSafeArea(int backBufferWidth, int backBufferHeight)
  //{
  //  float safew = (backBufferWidth + 19.0f) / 20.0f;
  //  float safeh = (backBufferHeight + 19.0f) / 20.0f;

  //  return RectangleF.FromLTRB(
  //    safew,
  //    safeh,
  //    backBufferWidth - safew + 0.5f,
  //    backBufferHeight - safeh + 0.5f);
  //}
};
