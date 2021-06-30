#pragma once
#include <RHI/Program/ProgramBase.h>
#include <RHI/Shader/Shader.h>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <map>
#include <set>

class VKDevice;

class VKProgram : public ProgramBase
{
public:
    VKProgram(VKDevice& device, const std::vector<std::shared_ptr<Shader>>& shaders);
};
