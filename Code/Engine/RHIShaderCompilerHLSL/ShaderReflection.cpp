#include <RHIShaderCompilerHLSL/ShaderReflection.h>
#ifdef DIRECTX_SUPPORT
#include <RHIShaderCompilerHLSL/DXILReflection.h>
#endif
#ifdef VULKAN_SUPPORT
#include <RHIShaderCompilerHLSL/SPIRVReflection.h>
#endif
#include <cassert>

ezSharedPtr<ShaderReflection> CreateShaderReflection(ShaderBlobType type, const void* data, size_t size)
{

#ifdef DIRECTX_SUPPORT
    if(type == ShaderBlobType::kDXIL)
    return EZ_DEFAULT_NEW(DXILReflection, data, size);
#endif
#ifdef VULKAN_SUPPORT
    if(type == ShaderBlobType::kSPIRV)
      return EZ_DEFAULT_NEW(SPIRVReflection, data, size);
#endif

    return nullptr;
}
