#pragma once

#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/VKBasics.h>

#include <RHI/Program/ProgramBase.h>

class ezRHIVKProgram : public ezRHIProgramBase
{
public:
  ezRHIVKProgram(ezRHIVKDevice& device, const ezDynamicArray<ezRHIShader*>& shaders);
};
