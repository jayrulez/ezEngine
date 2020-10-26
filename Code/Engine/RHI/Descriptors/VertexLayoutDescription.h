#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Descriptors/VertexElementDescription.h>
#include <RHI/Utils.h>

#include <Foundation/Algorithm/HashableStruct.h>


/// <summary>
/// Describes the layout of vertex data in a single <see cref="RHIDeviceBuffer"/> used as a vertex buffer.
/// </summary>
struct EZ_RHI_DLL RHIVertexLayoutDescription : public ezHashableStruct<RHIVertexLayoutDescription>
{
  /// <summary>
  /// The number of bytes in between successive elements in the <see cref="RHIDeviceBuffer"/>.
  /// </summary>
  ezUInt32 Stride;

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
  ezUInt32 InstanceStepRate;

  /// <summary>
  /// Constructs a new RHIVertexLayoutDescription.
  /// </summary>
  /// <param name="stride">The number of bytes in between successive elements in the <see cref="RHIDeviceBuffer"/>.</param>
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
  /// <param name="stride">The number of bytes in between successive elements in the <see cref="RHIDeviceBuffer"/>.</param>
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
      ezUInt32 elementSize = RHIFormatUtils::GetSize(elements[i].Format);
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

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements and all array elements are equal; false otherswise.</returns>
  bool operator==(const RHIVertexLayoutDescription& other) const
  {
    return Stride == other.Stride && Elements == other.Elements && InstanceStepRate == other.InstanceStepRate;
  }
};
