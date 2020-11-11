#include <RHI/Vulkan/Vulkan_Internal.h>
#include <RHI/Vulkan/GraphicsDevice_Vulkan.h>
#include <Foundation/Logging/Log.h>

//PFN_vkCreateRayTracingPipelinesKHR createRayTracingPipelinesKHR = nullptr;
//PFN_vkCreateAccelerationStructureKHR createAccelerationStructureKHR = nullptr;
//PFN_vkBindAccelerationStructureMemoryKHR bindAccelerationStructureMemoryKHR = nullptr;
//PFN_vkDestroyAccelerationStructureKHR destroyAccelerationStructureKHR = nullptr;
//PFN_vkGetAccelerationStructureMemoryRequirementsKHR getAccelerationStructureMemoryRequirementsKHR = nullptr;
//PFN_vkGetAccelerationStructureDeviceAddressKHR getAccelerationStructureDeviceAddressKHR = nullptr;
//PFN_vkGetRayTracingShaderGroupHandlesKHR getRayTracingShaderGroupHandlesKHR = nullptr;
//PFN_vkCmdBuildAccelerationStructureKHR cmdBuildAccelerationStructureKHR = nullptr;
//PFN_vkCmdTraceRaysKHR cmdTraceRaysKHR = nullptr;
//
//PFN_vkCmdDrawMeshTasksNV cmdDrawMeshTasksNV = nullptr;
//PFN_vkCmdDrawMeshTasksIndirectNV cmdDrawMeshTasksIndirectNV = nullptr;

namespace Vulkan_Internal
{
  // Converters:
  VkFormat _ConvertFormat(ezEnum<ezRHIFormat> value)
  {
    switch (value)
    {
      case ezRHIFormat::Unknown:
        return VK_FORMAT_UNDEFINED;
        break;
      case ezRHIFormat::R32G32B32A32_FLOAT:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
        break;
      case ezRHIFormat::R32G32B32A32_UINT:
        return VK_FORMAT_R32G32B32A32_UINT;
        break;
      case ezRHIFormat::R32G32B32A32_SINT:
        return VK_FORMAT_R32G32B32A32_SINT;
        break;
      case ezRHIFormat::R32G32B32_FLOAT:
        return VK_FORMAT_R32G32B32_SFLOAT;
        break;
      case ezRHIFormat::R32G32B32_UINT:
        return VK_FORMAT_R32G32B32_UINT;
        break;
      case ezRHIFormat::R32G32B32_SINT:
        return VK_FORMAT_R32G32B32_SINT;
        break;
      case ezRHIFormat::R16G16B16A16_FLOAT:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
        break;
      case ezRHIFormat::R16G16B16A16_UNORM:
        return VK_FORMAT_R16G16B16A16_UNORM;
        break;
      case ezRHIFormat::R16G16B16A16_UINT:
        return VK_FORMAT_R16G16B16A16_UINT;
        break;
      case ezRHIFormat::R16G16B16A16_SNORM:
        return VK_FORMAT_R16G16B16A16_SNORM;
        break;
      case ezRHIFormat::R16G16B16A16_SINT:
        return VK_FORMAT_R16G16B16A16_SINT;
        break;
      case ezRHIFormat::R32G32_FLOAT:
        return VK_FORMAT_R32G32_SFLOAT;
        break;
      case ezRHIFormat::R32G32_UINT:
        return VK_FORMAT_R32G32_UINT;
        break;
      case ezRHIFormat::R32G32_SINT:
        return VK_FORMAT_R32G32_SINT;
        break;
      case ezRHIFormat::R32G8X24_TYPELESS:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
        break;
      case ezRHIFormat::D32_FLOAT_S8X24_UINT:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
        break;
      case ezRHIFormat::R10G10B10A2_UNORM:
        return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        break;
      case ezRHIFormat::R10G10B10A2_UINT:
        return VK_FORMAT_A2B10G10R10_UINT_PACK32;
        break;
      case ezRHIFormat::R11G11B10_FLOAT:
        return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        break;
      case ezRHIFormat::R8G8B8A8_UNORM:
        return VK_FORMAT_R8G8B8A8_UNORM;
        break;
      case ezRHIFormat::R8G8B8A8_UNORM_SRGB:
        return VK_FORMAT_R8G8B8A8_SRGB;
        break;
      case ezRHIFormat::R8G8B8A8_UINT:
        return VK_FORMAT_R8G8B8A8_UINT;
        break;
      case ezRHIFormat::R8G8B8A8_SNORM:
        return VK_FORMAT_R8G8B8A8_SNORM;
        break;
      case ezRHIFormat::R8G8B8A8_SINT:
        return VK_FORMAT_R8G8B8A8_SINT;
        break;
      case ezRHIFormat::R16G16_FLOAT:
        return VK_FORMAT_R16G16_SFLOAT;
        break;
      case ezRHIFormat::R16G16_UNORM:
        return VK_FORMAT_R16G16_UNORM;
        break;
      case ezRHIFormat::R16G16_UINT:
        return VK_FORMAT_R16G16_UINT;
        break;
      case ezRHIFormat::R16G16_SNORM:
        return VK_FORMAT_R16G16_SNORM;
        break;
      case ezRHIFormat::R16G16_SINT:
        return VK_FORMAT_R16G16_SINT;
        break;
      case ezRHIFormat::R32_TYPELESS:
        return VK_FORMAT_D32_SFLOAT;
        break;
      case ezRHIFormat::D32_FLOAT:
        return VK_FORMAT_D32_SFLOAT;
        break;
      case ezRHIFormat::R32_FLOAT:
        return VK_FORMAT_R32_SFLOAT;
        break;
      case ezRHIFormat::R32_UINT:
        return VK_FORMAT_R32_UINT;
        break;
      case ezRHIFormat::R32_SINT:
        return VK_FORMAT_R32_SINT;
        break;
      case ezRHIFormat::R24G8_TYPELESS:
        return VK_FORMAT_D24_UNORM_S8_UINT;
        break;
      case ezRHIFormat::D24_UNORM_S8_UINT:
        return VK_FORMAT_D24_UNORM_S8_UINT;
        break;
      case ezRHIFormat::R8G8_UNORM:
        return VK_FORMAT_R8G8_UNORM;
        break;
      case ezRHIFormat::R8G8_UINT:
        return VK_FORMAT_R8G8_UINT;
        break;
      case ezRHIFormat::R8G8_SNORM:
        return VK_FORMAT_R8G8_SNORM;
        break;
      case ezRHIFormat::R8G8_SINT:
        return VK_FORMAT_R8G8_SINT;
        break;
      case ezRHIFormat::R16_TYPELESS:
        return VK_FORMAT_D16_UNORM;
        break;
      case ezRHIFormat::R16_FLOAT:
        return VK_FORMAT_R16_SFLOAT;
        break;
      case ezRHIFormat::D16_UNORM:
        return VK_FORMAT_D16_UNORM;
        break;
      case ezRHIFormat::R16_UNORM:
        return VK_FORMAT_R16_UNORM;
        break;
      case ezRHIFormat::R16_UINT:
        return VK_FORMAT_R16_UINT;
        break;
      case ezRHIFormat::R16_SNORM:
        return VK_FORMAT_R16_SNORM;
        break;
      case ezRHIFormat::R16_SINT:
        return VK_FORMAT_R16_SINT;
        break;
      case ezRHIFormat::R8_UNORM:
        return VK_FORMAT_R8_UNORM;
        break;
      case ezRHIFormat::R8_UINT:
        return VK_FORMAT_R8_UINT;
        break;
      case ezRHIFormat::R8_SNORM:
        return VK_FORMAT_R8_SNORM;
        break;
      case ezRHIFormat::R8_SINT:
        return VK_FORMAT_R8_SINT;
        break;
      case ezRHIFormat::BC1_UNORM:
        return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        break;
      case ezRHIFormat::BC1_UNORM_SRGB:
        return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
        break;
      case ezRHIFormat::BC2_UNORM:
        return VK_FORMAT_BC2_UNORM_BLOCK;
        break;
      case ezRHIFormat::BC2_UNORM_SRGB:
        return VK_FORMAT_BC2_SRGB_BLOCK;
        break;
      case ezRHIFormat::BC3_UNORM:
        return VK_FORMAT_BC3_UNORM_BLOCK;
        break;
      case ezRHIFormat::BC3_UNORM_SRGB:
        return VK_FORMAT_BC3_SRGB_BLOCK;
        break;
      case ezRHIFormat::BC4_UNORM:
        return VK_FORMAT_BC4_UNORM_BLOCK;
        break;
      case ezRHIFormat::BC4_SNORM:
        return VK_FORMAT_BC4_SNORM_BLOCK;
        break;
      case ezRHIFormat::BC5_UNORM:
        return VK_FORMAT_BC5_UNORM_BLOCK;
        break;
      case ezRHIFormat::BC5_SNORM:
        return VK_FORMAT_BC5_SNORM_BLOCK;
        break;
      case ezRHIFormat::B8G8R8A8_UNORM:
        return VK_FORMAT_B8G8R8A8_UNORM;
        break;
      case ezRHIFormat::B8G8R8A8_UNORM_SRGB:
        return VK_FORMAT_B8G8R8A8_SRGB;
        break;
      case ezRHIFormat::BC6H_UF16:
        return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        break;
      case ezRHIFormat::BC6H_SF16:
        return VK_FORMAT_BC6H_SFLOAT_BLOCK;
        break;
      case ezRHIFormat::BC7_UNORM:
        return VK_FORMAT_BC7_UNORM_BLOCK;
        break;
      case ezRHIFormat::BC7_UNORM_SRGB:
        return VK_FORMAT_BC7_SRGB_BLOCK;
        break;
    }
    return VK_FORMAT_UNDEFINED;
  }
  VkCompareOp _ConvertComparisonFunc(ezEnum<ezRHIComparisonFunc> value)
  {
    switch (value)
    {
      case ezRHIComparisonFunc::Never:
        return VK_COMPARE_OP_NEVER;
        break;
      case ezRHIComparisonFunc::Less:
        return VK_COMPARE_OP_LESS;
        break;
      case ezRHIComparisonFunc::Equal:
        return VK_COMPARE_OP_EQUAL;
        break;
      case ezRHIComparisonFunc::LessEqual:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
        break;
      case ezRHIComparisonFunc::Greater:
        return VK_COMPARE_OP_GREATER;
        break;
      case ezRHIComparisonFunc::NotEqual:
        return VK_COMPARE_OP_NOT_EQUAL;
        break;
      case ezRHIComparisonFunc::GreaterEqual:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
        break;
      case ezRHIComparisonFunc::Always:
        return VK_COMPARE_OP_ALWAYS;
        break;
      default:
        break;
    }
    return VK_COMPARE_OP_NEVER;
  }
  VkBlendFactor _ConvertBlend(ezEnum<ezRHIBlendFactor> value)
  {
    switch (value)
    {
      case ezRHIBlendFactor::Zero:
        return VK_BLEND_FACTOR_ZERO;
        break;
      case ezRHIBlendFactor::One:
        return VK_BLEND_FACTOR_ONE;
        break;
      case ezRHIBlendFactor::SourceColor:
        return VK_BLEND_FACTOR_SRC_COLOR;
        break;
      case ezRHIBlendFactor::InverseSourceColor:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        break;
      case ezRHIBlendFactor::SourceAlpha:
        return VK_BLEND_FACTOR_SRC_ALPHA;
        break;
      case ezRHIBlendFactor::InverseSourceAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        break;
      case ezRHIBlendFactor::DestinationAlpha:
        return VK_BLEND_FACTOR_DST_ALPHA;
        break;
      case ezRHIBlendFactor::InverseDestinationAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        break;
      case ezRHIBlendFactor::DestinationColor:
        return VK_BLEND_FACTOR_DST_COLOR;
        break;
      case ezRHIBlendFactor::InverseDestinationColor:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        break;
      case ezRHIBlendFactor::SourceAlphaSaturated:
        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        break;
      case ezRHIBlendFactor::BlendFactor:
        return VK_BLEND_FACTOR_CONSTANT_COLOR;
        break;
      case ezRHIBlendFactor::InverseBlendFactor:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        break;
      case ezRHIBlendFactor::Source1Color:
        return VK_BLEND_FACTOR_SRC1_COLOR;
        break;
      case ezRHIBlendFactor::InverseSource1Color:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        break;
      case ezRHIBlendFactor::Source1Alpha:
        return VK_BLEND_FACTOR_SRC1_ALPHA;
        break;
      case ezRHIBlendFactor::InverseSource1Alpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        break;
      default:
        break;
    }
    return VK_BLEND_FACTOR_ZERO;
  }
  VkBlendOp _ConvertBlendOp(ezEnum<ezRHIBlendOp> value)
  {
    switch (value)
    {
      case ezRHIBlendOp::Add:
        return VK_BLEND_OP_ADD;
        break;
      case ezRHIBlendOp::Subtract:
        return VK_BLEND_OP_SUBTRACT;
        break;
      case ezRHIBlendOp::ReverseSubtract:
        return VK_BLEND_OP_REVERSE_SUBTRACT;
        break;
      case ezRHIBlendOp::Min:
        return VK_BLEND_OP_MIN;
        break;
      case ezRHIBlendOp::Max:
        return VK_BLEND_OP_MAX;
        break;
      default:
        break;
    }
    return VK_BLEND_OP_ADD;
  }
  VkSamplerAddressMode _ConvertTextureAddressMode(ezEnum<ezRHITextureAddressMode> value)
  {
    switch (value)
    {
      case ezRHITextureAddressMode::Wrap:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        break;
      case ezRHITextureAddressMode::Mirror:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        break;
      case ezRHITextureAddressMode::Clamp:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        break;
      case ezRHITextureAddressMode::Border:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        break;
      default:
        break;
    }
    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  }
  VkStencilOp _ConvertStencilOp(ezEnum<ezRHIStencilOp> value)
  {
    switch (value)
    {
      case ezRHIStencilOp::Keep:
        return VK_STENCIL_OP_KEEP;
        break;
      case ezRHIStencilOp::Zero:
        return VK_STENCIL_OP_ZERO;
        break;
      case ezRHIStencilOp::Replace:
        return VK_STENCIL_OP_REPLACE;
        break;
      case ezRHIStencilOp::IncrementSaturated:
        return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        break;
      case ezRHIStencilOp::DecrementSaturated:
        return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        break;
      case ezRHIStencilOp::Invert:
        return VK_STENCIL_OP_INVERT;
        break;
      case ezRHIStencilOp::Increment:
        return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        break;
      case ezRHIStencilOp::Decrement:
        return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        break;
      default:
        break;
    }
    return VK_STENCIL_OP_KEEP;
  }
  constexpr VkImageLayout _ConvertImageLayout(IMAGE_LAYOUT value)
  {
    switch (value)
    {
      case IMAGE_LAYOUT_GENERAL:
        return VK_IMAGE_LAYOUT_GENERAL;
      case IMAGE_LAYOUT_RENDERTARGET:
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      case IMAGE_LAYOUT_DEPTHSTENCIL:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      case IMAGE_LAYOUT_DEPTHSTENCIL_READONLY:
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
      case IMAGE_LAYOUT_SHADER_RESOURCE:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      case IMAGE_LAYOUT_UNORDERED_ACCESS:
        return VK_IMAGE_LAYOUT_GENERAL;
      case IMAGE_LAYOUT_COPY_SRC:
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      case IMAGE_LAYOUT_COPY_DST:
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    }
    return VK_IMAGE_LAYOUT_UNDEFINED;
  }
  VkShaderStageFlags _ConvertStageFlags(ezEnum<ezRHIShaderStage> value)
  {
    switch (value)
    {
      case ezRHIShaderStage::MeshShader:
        return VK_SHADER_STAGE_MESH_BIT_NV;
      case ezRHIShaderStage::AmplificationShader:
        return VK_SHADER_STAGE_TASK_BIT_NV;
      case ezRHIShaderStage::VertexShader:
        return VK_SHADER_STAGE_VERTEX_BIT;
      case ezRHIShaderStage::HullShader:
        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
      case ezRHIShaderStage::DomainShader:
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
      case ezRHIShaderStage::GeometryShader:
        return VK_SHADER_STAGE_GEOMETRY_BIT;
      case ezRHIShaderStage::PixelShader:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
      case ezRHIShaderStage::ComputeShader:
        return VK_SHADER_STAGE_COMPUTE_BIT;
      default:
      case ezRHIShaderStage::ENUM_COUNT:
        return VK_SHADER_STAGE_ALL;
    }
  }
  VkAccessFlags _ParseImageLayout(IMAGE_LAYOUT value)
  {
    VkAccessFlags flags = 0;

    switch (value)
    {
      case IMAGE_LAYOUT_GENERAL:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        flags |= VK_ACCESS_SHADER_WRITE_BIT;
        flags |= VK_ACCESS_TRANSFER_READ_BIT;
        flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
        flags |= VK_ACCESS_MEMORY_READ_BIT;
        flags |= VK_ACCESS_MEMORY_WRITE_BIT;
        break;
      case IMAGE_LAYOUT_RENDERTARGET:
        flags |= VK_ACCESS_SHADER_WRITE_BIT;
        break;
      case IMAGE_LAYOUT_DEPTHSTENCIL:
        flags |= VK_ACCESS_SHADER_WRITE_BIT;
        break;
      case IMAGE_LAYOUT_DEPTHSTENCIL_READONLY:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        break;
      case IMAGE_LAYOUT_SHADER_RESOURCE:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        break;
      case IMAGE_LAYOUT_UNORDERED_ACCESS:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        flags |= VK_ACCESS_SHADER_WRITE_BIT;
        break;
      case IMAGE_LAYOUT_COPY_SRC:
        flags |= VK_ACCESS_TRANSFER_READ_BIT;
        break;
      case IMAGE_LAYOUT_COPY_DST:
        flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    }

    return flags;
  }
  VkAccessFlags _ParseBufferState(BUFFER_STATE value)
  {
    VkAccessFlags flags = 0;

    switch (value)
    {
      case BUFFER_STATE_GENERAL:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        flags |= VK_ACCESS_SHADER_WRITE_BIT;
        flags |= VK_ACCESS_TRANSFER_READ_BIT;
        flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
        flags |= VK_ACCESS_HOST_READ_BIT;
        flags |= VK_ACCESS_HOST_WRITE_BIT;
        flags |= VK_ACCESS_MEMORY_READ_BIT;
        flags |= VK_ACCESS_MEMORY_WRITE_BIT;
        flags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        flags |= VK_ACCESS_INDEX_READ_BIT;
        flags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        flags |= VK_ACCESS_UNIFORM_READ_BIT;
        break;
      case BUFFER_STATE_VERTEX_BUFFER:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        flags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        break;
      case BUFFER_STATE_INDEX_BUFFER:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        flags |= VK_ACCESS_INDEX_READ_BIT;
        break;
      case BUFFER_STATE_CONSTANT_BUFFER:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        flags |= VK_ACCESS_UNIFORM_READ_BIT;
        break;
      case BUFFER_STATE_INDIRECT_ARGUMENT:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        flags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        break;
      case BUFFER_STATE_SHADER_RESOURCE:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        flags |= VK_ACCESS_UNIFORM_READ_BIT;
        break;
      case BUFFER_STATE_UNORDERED_ACCESS:
        flags |= VK_ACCESS_SHADER_READ_BIT;
        flags |= VK_ACCESS_SHADER_WRITE_BIT;
        break;
      case BUFFER_STATE_COPY_SRC:
        flags |= VK_ACCESS_TRANSFER_READ_BIT;
        break;
      case BUFFER_STATE_COPY_DST:
        flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
      default:
        break;
    }

    return flags;
  }
  bool checkDeviceExtensionSupport(const char* checkExtension, const ezDynamicArray<VkExtensionProperties>& available_deviceExtensions)
  {

    for (const auto& x : available_deviceExtensions)
    {
      if (strcmp(x.extensionName, checkExtension) == 0)
      {
        return true;
      }
    }

    return false;
  }

  // Validation layer helpers:
  const ezDynamicArray<const char*> getValidationLayers()
  {
    ezDynamicArray<const char*> validationLayers;
    validationLayers.PushBack("VK_LAYER_KHRONOS_validation");
    return validationLayers;
  }
  bool checkValidationLayerSupport()
  {
    ezUInt32 layerCount;
    VkResult res = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    EZ_ASSERT_ALWAYS(res == VK_SUCCESS, "Unable to enumerate instance layer properties.");

    ezDynamicArray<VkLayerProperties> availableLayers;
    availableLayers.SetCount(layerCount);
    res = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetData());
    EZ_ASSERT_ALWAYS(res == VK_SUCCESS, "Unable to enumerate instance layer properties.");

    const ezDynamicArray<const char*> validationLayers = getValidationLayers();
    for (const char* layerName : validationLayers)
    {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers)
      {
        if (strcmp(layerName, layerProperties.layerName) == 0)
        {
          layerFound = true;
          break;
        }
      }

      if (!layerFound)
      {
        return false;
      }
    }

    return true;
  }
  VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
  {
    // Log debug messge
    ezStringBuilder sb("");

    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
      sb.AppendFormat("[Vulkan Warning]: {}\n", callback_data->pMessage);
    }
    else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
      sb.AppendFormat("[Vulkan Error]: {}\n", callback_data->pMessage);
    }

    ezLog::Debug(sb);

    return VK_FALSE;
  }
  VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, ezUInt64 obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
  {

    ezStringBuilder sb("");
    sb.AppendFormat("[VULKAN validation layer]: {}\n", msg);
    ezLog::Debug(sb);

    return VK_FALSE;
  }
  VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
  {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
      return func(instance, pCreateInfo, pAllocator, pMessenger);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
  void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
  {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
      func(instance, messenger, pAllocator);
    }
  }
  VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
  {
    auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr)
    {
      return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else
    {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }
  void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
  {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr)
    {
      func(instance, callback, pAllocator);
    }
  }

  // Queue families:
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
  {
    QueueFamilyIndices indices;

    ezUInt32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    ezDynamicArray<VkQueueFamilyProperties> queueFamilies;
    queueFamilies.SetCount(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.GetData());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
      VkBool32 presentSupport = false;
      VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
      EZ_ASSERT_ALWAYS(res == VK_SUCCESS, "Unable to get physical device surface support");

      if (indices.presentFamily < 0 && queueFamily.queueCount > 0 && presentSupport)
      {
        indices.presentFamily = i;
      }

      if (indices.graphicsFamily < 0 && queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      {
        indices.graphicsFamily = i;
      }

      if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
      {
        indices.copyFamily = i;
      }

      i++;
    }

    return indices;
  }
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
  {
    SwapChainSupportDetails details;

    VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    EZ_ASSERT_ALWAYS(res == VK_SUCCESS, "Unable to get physical device surface capabilities");

    ezUInt32 formatCount;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    EZ_ASSERT_ALWAYS(res == VK_SUCCESS, "Unable to get physical device surface formats");

    if (formatCount != 0)
    {
      details.formats.SetCount(formatCount);
      res = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.GetData());
      EZ_ASSERT_ALWAYS(res == VK_SUCCESS, "Unable to get physical device surface formats");
    }

    ezUInt32 presentModeCount;
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    EZ_ASSERT_ALWAYS(res == VK_SUCCESS, "Unable to get physical device surface present modes");

    if (presentModeCount != 0)
    {
      details.presentModes.SetCount(presentModeCount);
      res = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.GetData());
      EZ_ASSERT_ALWAYS(res == VK_SUCCESS, "Unable to get physical device surface present modes");
    }

    return details;
  }
  ezUInt32 findMemoryType(VkPhysicalDevice device, ezUInt32 typeFilter, VkMemoryPropertyFlags properties)
  {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    for (ezUInt32 i = 0; i < memProperties.memoryTypeCount; i++)
    {
      if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
      {
        return i;
      }
    }

    EZ_ASSERT_ALWAYS(false, "");
    return ~0;
  }

  // Device selection helpers:
  const ezDynamicArray<const char*> getRequiredDeviceExtensions()
  {
    ezDynamicArray<const char*> required_deviceExtensions;
    required_deviceExtensions.PushBack(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    required_deviceExtensions.PushBack(VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME);

    return required_deviceExtensions;
  }
  bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
  {
    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    if (!indices.isComplete())
    {
      return false;
    }

    ezUInt32 extensionCount;
    VkResult res = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    EZ_ASSERT_ALWAYS(res == VK_SUCCESS, "Unable to enumerate device extension properties");
    ezDynamicArray<VkExtensionProperties> available;
    available.SetCount(extensionCount);
    res = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, available.GetData());
    EZ_ASSERT_ALWAYS(res == VK_SUCCESS, "Unable to enumerate device extension properties");

    const ezDynamicArray<const char*> required_deviceExtensions = getRequiredDeviceExtensions();
    for (auto& x : required_deviceExtensions)
    {
      if (!checkDeviceExtensionSupport(x, available))
      {
        return false; // device doesn't have a required extension
      }
    }

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);

    return !swapChainSupport.formats.IsEmpty() && !swapChainSupport.presentModes.IsEmpty();
  }
} // namespace Vulkan_Internal
