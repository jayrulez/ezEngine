#pragma once

#include <Foundation/Basics.h>
#include <RendererFoundation/RendererFoundationDLL.h>
#include <RendererFoundation/Descriptors/Descriptors.h>

// Configure the DLL Import/Export Define
#if EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_RENDERERVK_LIB
#    define EZ_RENDERERVK_DLL __declspec(dllexport)
#  else
#    define EZ_RENDERERVK_DLL __declspec(dllimport)
#  endif
#else
#  define EZ_RENDERERVK_DLL
#endif

#define VK_USE_PLATFORM_WIN32_KHR


EZ_DEFINE_AS_POD_TYPE(VkExtensionProperties);
EZ_DEFINE_AS_POD_TYPE(VkLayerProperties);
EZ_DEFINE_AS_POD_TYPE(VkPhysicalDevice);
EZ_DEFINE_AS_POD_TYPE(VkQueueFamilyProperties);
EZ_DEFINE_AS_POD_TYPE(VkDeviceQueueCreateInfo);
EZ_DEFINE_AS_POD_TYPE(VkSurfaceFormatKHR);
EZ_DEFINE_AS_POD_TYPE(VkPresentModeKHR);
EZ_DEFINE_AS_POD_TYPE(VkImage);
EZ_DEFINE_AS_POD_TYPE(VkImageView);
EZ_DEFINE_AS_POD_TYPE(VkQueue);
EZ_DEFINE_AS_POD_TYPE(VkPipelineShaderStageCreateInfo);

namespace ezInternal::Vk
{
  struct PipelineStateDesc;
  class Pipeline;

  enum QueueType
  {
    Graphics,
    Compute,
    Copy,
    Count
  };

  enum class PipelineType
  {
    Graphics,
    Compute
  };

  struct PipelineStateDesc
  {
    PipelineType Type = PipelineType::Graphics;
    ezGALPrimitiveTopology::Enum PrimitiveTopology = ezGALPrimitiveTopology::Triangles;
    ezGALShaderCreationDescription VertexShader;
    ezGALShaderCreationDescription FragmentShader;
    ezGALShaderCreationDescription HullShader;
    ezGALShaderCreationDescription DomainShader;
    ezGALShaderCreationDescription GeometryShader;
    ezGALBlendStateCreationDescription BlendState;
    ezGALRasterizerStateCreationDescription RasterizerState;
    ezGALDepthStencilStateCreationDescription DepthStencilState;
  };
}
