#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>
#include <memory>
#include <vector>

class BindingSet : public QueryInterface
{
public:
    virtual ~BindingSet() = default;
    virtual void WriteBindings(const std::vector<BindingDesc>& bindings) = 0;
};
