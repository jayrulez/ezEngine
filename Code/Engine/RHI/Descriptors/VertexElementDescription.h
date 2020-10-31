#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>


/// <summary>
/// Describes a single element of a vertex.
/// </summary>
struct EZ_RHI_DLL RHIVertexElementDescription : public ezHashableStruct<RHIVertexElementDescription>
{
  EZ_DECLARE_POD_TYPE();
  /// <summary>
  /// The name of the element.
  /// </summary>
  ezString Name;

  /// <summary>
  /// The semantic type of the element.
  /// NOTE: When using SPIRV, all vertex elements will use
  /// <see cref="RHIVertexElementSemantic.TextureCoordinate"/>.
  /// </summary>
  ezEnum<RHIVertexElementSemantic> Semantic;

  /// <summary>
  /// The format of the element.
  /// </summary>
  ezEnum<RHIVertexElementFormat> Format;

  /// <summary>
  /// The offset in bytes from the beginning of the vertex.
  /// </summary>
  ezUInt32 Offset = 0;

  RHIVertexElementDescription() = default;

  /// <summary>
  /// Constructs a new VertexElementDescription describing a per-vertex element.
  /// </summary>
  /// <param name="name">The name of the element.</param>
  /// <param name="semantic">The semantic type of the element.</param>
  /// <param name="format">The format of the element.</param>
  RHIVertexElementDescription(ezString name, ezEnum<RHIVertexElementSemantic> semantic, ezEnum<RHIVertexElementFormat> format)
    : Name(name)
    , Semantic(semantic)
    , Format(format)
  {
  }

  /// <summary>
  /// Constructs a new VertexElementDescription.
  /// </summary>
  /// <param name="name">The name of the element.</param>
  /// <param name="semantic">The semantic type of the element.</param>
  /// <param name="format">The format of the element.</param>
  RHIVertexElementDescription(
    ezString name,
    ezEnum<RHIVertexElementFormat> format,
    ezEnum<RHIVertexElementSemantic> semantic)
  {
    Name = name;
    Format = format;
    Semantic = semantic;
    Offset = 0;
  }

  /// <summary>
  /// Constructs a new VertexElementDescription.
  /// </summary>
  /// <param name="name">The name of the element.</param>
  /// <param name="semantic">The semantic type of the element.</param>
  /// <param name="format">The format of the element.</param>
  /// <param name="offset">The offset in bytes from the beginning of the vertex.</param>
  RHIVertexElementDescription(
    ezString name,
    ezEnum<RHIVertexElementSemantic> semantic,
    ezEnum<RHIVertexElementFormat> format,
    ezUInt32 offset)
  {
    Name = name;
    Format = format;
    Semantic = semantic;
    Offset = offset;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIVertexElementDescription& other) const
  {
    return Name.IsEqual(other.Name) && Format == other.Format && Semantic == other.Semantic && Offset == other.Offset;
  }
};
