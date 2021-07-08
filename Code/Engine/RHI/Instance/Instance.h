#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Adapter/Adapter.h>
#include <memory>
#include <string>
#include <vector>

class EZ_RHI_DLL Instance : public QueryInterface
{
public:
    virtual ~Instance() = default;
    virtual std::vector<std::shared_ptr<Adapter>> EnumerateAdapters() = 0;
};

struct EZ_RHI_DLL InstanceFactory
{
  using CreatorFunc = ezDelegate<std::shared_ptr<Instance>()>;

  static std::shared_ptr<Instance> CreateInstance(ApiType type);

  static void RegisterCreatorFunc(ApiType type, const CreatorFunc& func);
  static void UnregisterCreatorFunc(ApiType type);
};

