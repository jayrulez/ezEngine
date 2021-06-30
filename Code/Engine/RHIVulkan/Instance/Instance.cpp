#include <RHI/Instance/Instance.h>
#include <RHIVulkan/Instance/VKInstance.h>
#include <cassert>

extern std::shared_ptr<Instance> CreateInstance(ApiType type)
{
  return std::make_shared<VKInstance>();
}
