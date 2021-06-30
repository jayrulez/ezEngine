#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/ApiType/ApiType.h>
#include <RHI/Adapter/Adapter.h>
#include <memory>
#include <string>
#include <vector>

class Instance : public QueryInterface
{
public:
    virtual ~Instance() = default;
    virtual std::vector<std::shared_ptr<Adapter>> EnumerateAdapters() = 0;
};

extern std::shared_ptr<Instance> CreateInstance(ApiType type);
