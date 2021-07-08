#include <RHIVulkanPCH.h>

#include <RHIVulkan/Program/VKProgram.h>

ezRHIVKProgram::ezRHIVKProgram(ezRHIVKDevice& device, const ezDynamicArray<ezRHIShader*>& shaders)
  : ezRHIProgramBase{shaders}
{
}
