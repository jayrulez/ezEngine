#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

#include <RHI/Resources/DeviceResource.h>

/// <summary>
/// Describes a <see cref="RHIShader"/>, for creation using a <see cref="RHIResourceFactory"/>.
/// </summary>
struct EZ_RHI_DLL RHIShaderDescription : public ezHashableStruct<RHIShaderDescription>
{
  /// <summary>
  /// The shader stage this instance describes.
  /// </summary>
  ezEnum<RHIShaderStages> Stage = RHIShaderStages::None;

  /// <summary>
  /// An array containing the raw shader bytes.
  /// For Direct3D11 shaders, this array must contain HLSL bytecode or HLSL text.
  /// For Vulkan shaders, this array must contain SPIR-V bytecode.
  /// For OpenGL and OpenGL ES shaders, this array must contain the ASCII-encoded text of the shader code.
  /// For Metal shaders, this array must contain Metal bitcode (a "metallib" file), or UTF8-encoded Metal shading language
  /// text.
  /// </summary>
  ezDynamicArray<ezUInt8> ShaderBytes;

  /// <summary>
  /// The name of the entry point function in the shader module to be used in this stage.
  /// </summary>
  ezString EntryPoint;

  /// <summary>
  /// Indicates whether the shader should be debuggable. This flag only has an effect if <see cref="ShaderBytes"/> contains
  /// shader code that will be compiled.
  /// </summary>
  bool Debug = false;

  /// <summary>
  /// Constructs a new ShaderDescription.
  /// </summary>
  /// <param name="stage">The shader stage to create.</param>
  /// <param name="shaderBytes">An array containing the raw shader bytes.</param>
  /// <param name="entryPoint">The name of the entry point function in the shader module to be used in this stage.</param>
  RHIShaderDescription(ezEnum<RHIShaderStages> stage, ezDynamicArray<ezUInt8> shaderBytes, ezString entryPoint)
  {
    Stage = stage;
    ShaderBytes = shaderBytes;
    EntryPoint = entryPoint;
    Debug = false;
  }

  /// <summary>
  /// Constructs a new ShaderDescription.
  /// </summary>
  /// <param name="stage">The shader stage to create.</param>
  /// <param name="shaderBytes">An array containing the raw shader bytes.</param>
  /// <param name="entryPoint">The name of the entry point function in the shader module to be used in this stage.</param>
  /// <param name="debug">Indicates whether the shader should be debuggable. This flag only has an effect if
  /// <paramref name="shaderBytes"/> contains shader code that will be compiled.</param>
  RHIShaderDescription(ezEnum<RHIShaderStages> stage, ezDynamicArray<ezUInt8> shaderBytes, ezString entryPoint, bool debug)
  {
    Stage = stage;
    ShaderBytes = shaderBytes;
    EntryPoint = entryPoint;
    Debug = debug;
  }

  /// <summary>
  /// Element-wise equality.
  /// </summary>
  /// <param name="other">The instance to compare to.</param>
  /// <returns>True if all elements are equal; false otherswise.</returns>
  bool operator==(const RHIShaderDescription& other) const
  {
    return Stage == other.Stage && ShaderBytes == other.ShaderBytes && EntryPoint == other.EntryPoint && Debug == other.Debug;
  }
};

/// <summary>
/// A device resource encapsulating a single shader module.
/// See <see cref="RHIShaderDescription"/>.
/// </summary>
class EZ_RHI_DLL RHIShader : public RHIDeviceResource
{
public:
  RHIShader(ezEnum<RHIShaderStages> stage, const ezString& entryPoint)
  {
    Stage = stage;
    EntryPoint = entryPoint;
  }

  /// <summary>
  /// The shader stage this instance can be used in.
  /// </summary>
  ezEnum<RHIShaderStages> GetStage() const
  {
    return Stage;
  }

  /// <summary>
  /// The name of the entry point function.
  /// </summary>
  ezString GetEntryPoint() const
  {
    return EntryPoint;
  }

  /// <summary>
  /// A string identifying this instance. Can be used to differentiate between objects in graphics debuggers and other
  /// tools.
  /// </summary>
  virtual ezString GetName() const = 0;
  virtual void SetName(const ezString& name) = 0;

private:
  ezEnum<RHIShaderStages> Stage;
  ezString EntryPoint;
};

/// <summary>
/// A <see cref="RHIPipeline"/> component describing a full set of shader stages and vertex layouts.
/// </summary>
struct EZ_RHI_DLL RHIShaderSetDescription : ezHashableStruct<RHIShaderSetDescription>
{
  /// <summary>
  /// An array of <see cref="RHIVertexLayoutDescription"/> describing the set of vertex layouts understood by the
  /// <see cref="RHIPipeline"/>. Each element in this array describes the input layout of a single <see cref="RHIDeviceBuffer"/>
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
  /// <see cref="RHIDeviceBuffer"/> to be bound when drawing.</param>
  /// <param name="shaders">An array of <see cref="Shader"/> objects, one for each shader stage which is to be active
  /// in the <see cref="RHIPipeline"/>. At a minimum, every graphics Pipeline must include a Vertex and Fragment shader. All
  /// other stages are optional, but if either Tessellation stage is present, then the other must also be.</param>
  RHIShaderSetDescription(ezDynamicArray<RHIVertexLayoutDescription>& vertexLayouts, ezDynamicArray<RHIShader*>& shaders)
  {
    VertexLayouts = vertexLayouts;
    Shaders = shaders;
    //Specializations.Clear();
  }

  /// <summary>
  /// Constructs a new ShaderSetDescription.
  /// </summary>
  /// <param name="vertexLayouts">An array of <see cref="RHIVertexLayoutDescription"/> describing the set of vertex layouts
  /// understood by the <see cref="RHIPipeline"/>. Each element in this array describes the input layout of a single
  /// <see cref="RHIDeviceBuffer"/> to be bound when drawing.</param>
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
    return VertexLayouts == other.VertexLayouts && Shaders == other.Shaders && Specializations == other.Specializations;
  }
};
