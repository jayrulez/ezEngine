#pragma once

#include <RHI/Descriptors/VertexLayoutDescription.h>
#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>
#include <RHI/Util.h>

#include <initializer_list>

class RHIShader;
struct RHISpecializationConstant;

/// <summary>
/// A <see cref="RHIPipeline"/> component describing a full set of shader stages and vertex layouts.
/// </summary>
struct EZ_RHI_DLL RHIShaderSetDescription : public ezHashableStruct<RHIShaderSetDescription>
{
  /// <summary>
  /// An array of <see cref="RHIVertexLayoutDescription"/> describing the set of vertex layouts understood by the
  /// <see cref="RHIPipeline"/>. Each element in this array describes the input layout of a single <see cref="RHIBuffer"/>
  /// to be bound when drawing.
  /// </summary>
  ezDynamicArray<RHIVertexLayoutDescription> VertexLayouts;

  /// <summary>
  /// An array of <see cref="RHIShader"/> objects, one for each shader stage which is to be active in the
  /// <see cref="RHIPipeline"/>. At a minimum, every graphics Pipeline must include a Vertex and Fragment
  /// shader. All other stages are optional, but if either Tessellation stage is present, then the other must also be.
  /// </summary>
  ezDynamicArray<RHIShader*> Shaders;

  /// <summary>
  /// An array of <see cref="RHISpecializationConstant"/> used to override specialization constants in the created
  /// <see cref="RHIPipeline"/>. Each element in this array describes a single ID-value pair, which will be matched with the
  /// constants specified in each <see cref="RHIShader"/>.
  /// </summary>
  ezDynamicArray<RHISpecializationConstant> Specializations;

  RHIShaderSetDescription() = default;

  /// <summary>
  /// Constructs a new ShaderSetDescription.
  /// </summary>
  /// <param name="vertexLayouts">An array of <see cref="RHIVertexLayoutDescription"/> describing the set of vertex layouts
  /// understood by the <see cref="Pipeline"/>. Each element in this array describes the input layout of a single
  /// <see cref="RHIBuffer"/> to be bound when drawing.</param>
  /// <param name="shaders">An array of <see cref="Shader"/> objects, one for each shader stage which is to be active
  /// in the <see cref="RHIPipeline"/>. At a minimum, every graphics Pipeline must include a Vertex and Fragment shader. All
  /// other stages are optional, but if either Tessellation stage is present, then the other must also be.</param>
  RHIShaderSetDescription(ezDynamicArray<RHIVertexLayoutDescription>& vertexLayouts, ezDynamicArray<RHIShader*>& shaders)
  {
    VertexLayouts = vertexLayouts;
    Shaders = shaders;
    //Specializations.Clear();
  }

  //RHIShaderSetDescription(std::initializer_list<RHIVertexLayoutDescription> vertexLayouts, std::initializer_list<RHIShader*> shaders)
  //{
  //  for (auto& layout : vertexLayouts)
  //  {
  //    VertexLayouts.PushBack(layout);
  //  }

  //  for (auto shader : shaders)
  //  {
  //    Shaders.PushBack(shader);
  //  }
  //}

  /// <summary>
  /// Constructs a new ShaderSetDescription.
  /// </summary>
  /// <param name="vertexLayouts">An array of <see cref="RHIVertexLayoutDescription"/> describing the set of vertex layouts
  /// understood by the <see cref="RHIPipeline"/>. Each element in this array describes the input layout of a single
  /// <see cref="RHIBuffer"/> to be bound when drawing.</param>
  /// <param name="shaders">An array of <see cref="Shader"/> objects, one for each shader stage which is to be active
  /// in the <see cref="Pipeline"/>. At a minimum, every graphics Pipeline must include a Vertex and Fragment shader. All
  /// other stages are optional, but if either Tessellation stage is present, then the other must also be.</param>
  /// <param name="specializations">An array of <see cref="RHISpecializationConstant"/> used to override specialization
  /// constants in the created <see cref="RHIPipeline"/>. Each element in this array describes a single ID-value pair, which
  /// will be matched with the constants specified in each <see cref="RHIShader"/>.</param>
  RHIShaderSetDescription(
    ezDynamicArray<RHIVertexLayoutDescription>& vertexLayouts,
    ezDynamicArray<RHIShader*>& shaders,
    ezDynamicArray<RHISpecializationConstant>& specializations)
  {
    VertexLayouts = vertexLayouts;
    Shaders = shaders;
    Specializations = specializations;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIShaderSetDescription& other) const
  {
    return Util::AreEquatable(VertexLayouts, other.VertexLayouts) &&
           Util::AreEqual(Shaders, other.Shaders) &&
           Util::AreEquatable(Specializations, other.Specializations);
  }
};
