#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Id.h>
#include <Foundation/Types/RefCounted.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RHI_LIB
#    define EZ_RHI_DLL __declspec(dllexport)
#  else
#    define EZ_RHI_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RHI_DLL
#endif

// Descriptor layout counts per shader stage:
//	Rebuilding shaders and graphics devices are necessary after changing these values
#define GPU_RESOURCE_HEAP_CBV_COUNT 12
#define GPU_RESOURCE_HEAP_SRV_COUNT 48
#define GPU_RESOURCE_HEAP_UAV_COUNT 8
#define GPU_SAMPLER_HEAP_COUNT 16

// Forward declarations
struct Shader;
struct BlendState;
struct RasterizerState;
struct DepthStencilState;
struct InputLayout;
struct GPUResource;
struct GPUBuffer;
struct Texture;
struct RootSignature;

/*
struct EZ_RHI_DLL ezRHIStruct
{
  typedef ezUInt8 StorageType;
  enum Enum
  {
    ENUM_COUNT
  };
  struct Bits { };
};
EZ_DECLARE_FLAGS_OPERATORS(_bitFlagName);
*/
