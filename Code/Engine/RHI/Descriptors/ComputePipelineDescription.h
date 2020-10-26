#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>
#include <RHI/Resources/SpecializationConstant.h>

#include <Foundation/Algorithm/HashableStruct.h>

class RHIShader;
class RHIResourceLayout;

/// <summary>
/// Describes a compute <see cref="Pipeline"/>, for creation using a <see cref="ResourceFactory"/>.
/// </summary>
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
