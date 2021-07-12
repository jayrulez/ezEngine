#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Program/ProgramBase.h>
#include <RHI/Shader/Shader.h>
#include <vector>

class VKDevice;

class VKProgram : public ProgramBase
{
public:
  VKProgram(VKDevice& device, const std::vector<std::shared_ptr<Shader>>& shaders);
};
