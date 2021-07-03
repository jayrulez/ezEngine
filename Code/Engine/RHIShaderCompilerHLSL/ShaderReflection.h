#pragma once
#include <RHIShaderCompilerHLSL/RHIShaderCompilerHLSLDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <memory>
#include <string>
#include <vector>

EZ_RHISHADERCOMPILERHLSL_DLL ezSharedPtr<ShaderReflection> CreateShaderReflection(ShaderBlobType type, const void* data, size_t size);
