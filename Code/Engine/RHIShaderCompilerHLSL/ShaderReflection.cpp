#include <RHIShaderCompilerHLSL/ShaderReflection.h>
#ifdef DIRECTX_SUPPORT
#include <RHIShaderCompilerHLSL/DXILReflection.h>
#endif
#ifdef VULKAN_SUPPORT
#include <RHIShaderCompilerHLSL/SPIRVReflection.h>
#endif

ezRHIShaderReflection* CreateShaderReflection(ezRHIShaderBlobType type, const void* data, ezUInt32 size)
{

#ifdef DIRECTX_SUPPORT
    if(type == ShaderBlobType::kDXIL)
        return std::make_shared<DXILReflection>(data, size);
#endif
#ifdef VULKAN_SUPPORT
    if(type == ezRHIShaderBlobType::SPIRV)
        return EZ_DEFAULT_NEW(ezRHISPIRVReflection, data, size);
#endif

    return nullptr;
}
