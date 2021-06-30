#pragma once
#include <RHI/Instance/BaseTypes.h>
#include <memory>
#include <string>
#include <vector>

std::shared_ptr<ShaderReflection> CreateShaderReflection(ShaderBlobType type, const void* data, size_t size);
