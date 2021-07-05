#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

enum class ezRHIShaderKind
{
  Unknown = 0,
  Pixel,
  Vertex,
  Geometry,
  Compute,
  Library,
  RayGeneration,
  Intersection,
  AnyHit,
  ClosestHit,
  Miss,
  Callable,
  Mesh,
  Amplification,
};

enum class ezRHIVariableType
{
  Struct,
  Float,
  Int,
  Uint,
  Bool,
};

struct EZ_RHI_DLL ezRHIEntryPoint : public ezHashableStruct<ezRHIEntryPoint>
{
  ezString m_Name;
  ezRHIShaderKind m_Kind;
  ezUInt32 m_PayloadSize;
  ezUInt32 m_AttributeSize;
};

struct EZ_RHI_DLL ezRHIInputParameterDescription
{
  ezUInt32 m_Location;
  ezString m_SemanticName;
  ezRHIResourceFormat m_Format;
};

struct EZ_RHI_DLL ezRHIOutputParameterDescription
{
  ezUInt32 m_Slot;
};

struct EZ_RHI_DLL ezRHIVariableLayout : public ezHashableStruct<ezRHIVariableLayout>
{
  ezString m_Name;
  ezRHIVariableType m_Type;
  ezUInt32 m_Offset;
  ezUInt32 m_Size;
  ezUInt32 m_Rows;
  ezUInt32 m_Columns;
  ezUInt32 m_Elements;
  ezDynamicArray<ezRHIVariableLayout> m_Members;
};

struct EZ_RHI_DLL ezRHIShaderFeatureInfo
{
  bool m_ResourceDescriptorHeapIndexing = false;
  bool m_SamplerDescriptorHeapIndexing = false;
};


class EZ_RHI_DLL ezRHIShaderReflection : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIShaderReflection() = default;
  virtual const ezDynamicArray<ezRHIEntryPoint>& GetEntryPoints() const = 0;
  virtual const ezDynamicArray<ezRHIResourceBindingDescription>& GetBindings() const = 0;
  virtual const ezDynamicArray<ezRHIVariableLayout>& GetVariableLayouts() const = 0;
  virtual const ezDynamicArray<ezRHIInputParameterDescription>& GetInputParameters() const = 0;
  virtual const ezDynamicArray<ezRHIOutputParameterDescription>& GetOutputParameters() const = 0;
  virtual const ezRHIShaderFeatureInfo& GetShaderFeatureInfo() const = 0;
};
