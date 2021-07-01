#pragma once

#include <RHIShaderCompilerHLSL/RHIShaderCompilerHLSLDLL.h>
#include <RHI/Instance/BaseTypes.h>

std::vector<uint8_t> Compile(const ShaderDesc& shader, ShaderBlobType blob_type);
