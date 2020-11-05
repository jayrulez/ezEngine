#pragma once

#include <RHI/Descriptors/VertexElementDescription.h>
#include <RHI/FormatHelpers.h>
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <RHI/Util.h>


/// <summary>
/// Describes the layout of vertex data in a single <see cref="RHIBuffer"/> used as a vertex buffer.
/// </summary>
struct EZ_RHI_DLL RHIVertexLayoutDescription : public ezHashableStruct<RHIVertexLayoutDescription>
{
  EZ_DECLARE_POD_TYPE();

  /// <summary>
  /// The number of bytes in between successive elements in the <see cref="RHIBuffer"/>.
  /// </summary>
  ezUInt32 Stride = 0;

  /// <summary>
  /// An array of <see cref="RHIVertexElementDescription"/> objects, each describing a single element of vertex data.
  /// </summary>
  ezDynamicArray<RHIVertexElementDescription> Elements;

  /// <summary>
  /// A value controlling how often data for instances is advanced for this layout. For per-vertex elements, this value
  /// should be 0.
  /// For example, an InstanceStepRate of 3 indicates that 3 instances will be drawn with the same value for this layout. The
  /// next 3 instances will be drawn with the next value, and so on.
  /// </summary>
  ezUInt32 InstanceStepRate = 0;

  RHIVertexLayoutDescription() = default;

  /// <summary>
  /// Constructs a new RHIVertexLayoutDescription.
  /// </summary>
  /// <param name="stride">The number of bytes in between successive elements in the <see cref="RHIBuffer"/>.</param>
  /// <param name="elements">An array of <see cref="RHIVertexElementDescription"/> objects, each describing a single element
  /// of vertex data.</param>
  RHIVertexLayoutDescription(ezUInt32 stride, ezDynamicArray<RHIVertexElementDescription> elements)
  {
    Stride = stride;
    Elements = elements;
    InstanceStepRate = 0;
  }

  /// <summary>
  /// Constructs a new RHIVertexLayoutDescription.
  /// </summary>
  /// <param name="stride">The number of bytes in between successive elements in the <see cref="RHIBuffer"/>.</param>
  /// <param name="elements">An array of <see cref="RHIVertexElementDescription"/> objects, each describing a single element
  /// of vertex data.</param>
  /// <param name="instanceStepRate">A value controlling how often data for instances is advanced for this element. For
  /// per-vertex elements, this value should be 0.
  /// For example, an InstanceStepRate of 3 indicates that 3 instances will be drawn with the same value for this element.
  /// The next 3 instances will be drawn with the next value for this element, and so on.</param>
  RHIVertexLayoutDescription(ezUInt32 stride, ezUInt32 instanceStepRate, ezDynamicArray<RHIVertexElementDescription> elements)
  {
    Stride = stride;
    Elements = elements;
    InstanceStepRate = instanceStepRate;
  }

  /// <summary>
  /// Constructs a new RHIVertexLayoutDescription. The stride is assumed to be the sum of the size of all elements.
  /// </summary>
  /// <param name="elements">An array of <see cref="VertexElementDescription"/> objects, each describing a single element
  /// of vertex data.</param>
  RHIVertexLayoutDescription(ezDynamicArray<RHIVertexElementDescription> elements)
  {
    Elements = elements;
    ezUInt32 computedStride = 0;
    for (ezUInt32 i = 0; i < elements.GetCount(); i++)
    {
      ezUInt32 elementSize = FormatHelpers::GetSize(elements[i].Format);
      if (elements[i].Offset != 0)
      {
        computedStride = elements[i].Offset + elementSize;
      }
      else
      {
        computedStride += elementSize;
      }
    }

    Stride = computedStride;
    InstanceStepRate = 0;
  }

  RHIVertexLayoutDescription& operator=(const RHIVertexLayoutDescription& other)
  {
    Stride = other.Stride;

    Elements.SetCountUninitialized(other.Elements.GetCount());
    for (ezUInt32 i = 0; i < other.Elements.GetCount(); i++)
    {
      Elements[i] = other.Elements[i];
    }

    InstanceStepRate = other.InstanceStepRate;

    return *this;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIVertexLayoutDescription& other) const
  {
    return Stride == other.Stride &&
           Util::AreEquatable(Elements, other.Elements) &&
           InstanceStepRate == other.InstanceStepRate;
  }
};
