#include <RHIShaderCompilerHLSL/ShaderReflection.h>
#ifdef DIRECTX_SUPPORT
#include <RHIShaderCompilerHLSL/DXILReflection.h>
#endif
#ifdef VULKAN_SUPPORT
#include <RHIShaderCompilerHLSL/SPIRVReflection.h>
#endif
#include <cassert>

std::shared_ptr<ShaderReflection> CreateShaderReflection(ShaderBlobType type, const void* data, size_t size)
{

#ifdef DIRECTX_SUPPORT
    if(type == ShaderBlobType::kDXIL)
        return std::make_shared<DXILReflection>(data, size);
#endif
#ifdef VULKAN_SUPPORT
    if(type == ShaderBlobType::kSPIRV)
        return std::make_shared<SPIRVReflection>(data, size);
#endif

    return nullptr;
}
