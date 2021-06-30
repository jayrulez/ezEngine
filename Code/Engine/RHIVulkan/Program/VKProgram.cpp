#include <RHIVulkan/Program/VKProgram.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHI/Shader/Shader.h>
#include <RHIVulkan/View/VKView.h>
#include <RHIVulkan/BindingSet/VKBindingSet.h>

VKProgram::VKProgram(VKDevice& device, const std::vector<std::shared_ptr<Shader>>& shaders)
    : ProgramBase(shaders)
{
}
