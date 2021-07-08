#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Basics.h>

#include <vulkan/vulkan.hpp>

constexpr ezUInt32 MAX_BINDLESS_HEAP_SIZE = 10000;

class ezRHIVKAdapter;
class ezRHIVKBindingSet;
class ezRHIVKBindingSetLayout;
class ezRHIVKCommandList;
class ezRHIVKCommandQueue;
class ezRHIVKDevice;
class ezRHIVKTimelineSemaphore;
class ezRHIVKFramebuffer;
class ezRHIVKGPUBindlessDescriptorPoolTyped;
class ezRHIVKGPUDescriptorPool;
class ezRHIVKGPUDescriptorPoolRange;
class ezRHIVKInstance;
class ezRHIVKMemory;
class ezRHIVKComputePipeline;
class ezRHIVKGraphicsPipeline;
class ezRHIVKPipeline;
class ezRHIVKRayTracingPipeline;
class ezRHIVKProgram;
class ezRHIVKQueryHeap;
class ezRHIVKRenderPass;
class ezRHIVKResource;
class ezRHIVKSwapChain;
class ezRHIVKView;

EZ_DEFINE_AS_POD_TYPE(vk::DynamicState);
EZ_DEFINE_AS_POD_TYPE(vk::FragmentShadingRateCombinerOpKHR);

struct ezRHIVKDescriptorSetPool
{
  vk::UniqueDescriptorPool m_Pool;
  vk::UniqueDescriptorSet m_Set;
};

namespace VKUtilities
{
  vk::Format ToVKFormat(ezRHIResourceFormat::Enum format);
  ezRHIResourceFormat::Enum ToEngineFormat(vk::Format format);
}
