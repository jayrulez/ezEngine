#pragma once

#include <RHI/RHIDLL.h>
#include <RHI/RHIPCH.h>

#include <Foundation/Algorithm/HashableStruct.h>

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