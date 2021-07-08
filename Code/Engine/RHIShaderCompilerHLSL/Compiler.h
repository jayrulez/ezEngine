#pragma once

#include <RHIShaderCompilerHLSL/RHIShaderCompilerHLSLDLL.h>
#include <RHI/Basics.h>

EZ_RHISHADERCOMPILERHLSL_DLL ezDynamicArray<ezUInt8> Compile(const ezString& shaderPath, const ezRHIShaderCreationDescription& desc, ezRHIShaderBlobType blobType);
