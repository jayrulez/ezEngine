#pragma once
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <spirv_hlsl.hpp>

class ezRHISPIRVReflection : public ezRHIShaderReflection
{
public:
  ezRHISPIRVReflection(const void* data, ezUInt32 size);
  const ezDynamicArray<ezRHIEntryPoint>& GetEntryPoints() const override;
  const ezDynamicArray<ezRHIResourceBindingDescription>& GetBindings() const override;
  const ezDynamicArray<ezRHIVariableLayout>& GetVariableLayouts() const override;
  const ezDynamicArray<ezRHIInputParameterDescription>& GetInputParameters() const override;
  const ezDynamicArray<ezRHIOutputParameterDescription>& GetOutputParameters() const override;
  const ezRHIShaderFeatureInfo& GetShaderFeatureInfo() const override;

private:
  ezDynamicArray<ezUInt32> m_Blob;
  ezDynamicArray<ezRHIEntryPoint> m_EntryPoints;
  ezDynamicArray<ezRHIResourceBindingDescription> m_Bindings;
  ezDynamicArray<ezRHIVariableLayout> m_Layouts;
  ezDynamicArray<ezRHIInputParameterDescription> m_InputParameters;
  ezDynamicArray<ezRHIOutputParameterDescription> m_OutputParameters;
  ezRHIShaderFeatureInfo m_ShaderFeatureInfo = {};
};
