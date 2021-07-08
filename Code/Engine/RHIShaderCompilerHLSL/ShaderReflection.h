#pragma once
#include <RHIShaderCompilerHLSL/RHIShaderCompilerHLSLDLL.h>

#include <RHI/Basics.h>

EZ_RHISHADERCOMPILERHLSL_DLL ezRHIShaderReflection* CreateShaderReflection(ezRHIShaderBlobType type, const void* data, ezUInt32 size);
