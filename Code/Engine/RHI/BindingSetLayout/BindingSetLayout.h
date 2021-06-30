#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>
#include <memory>
#include <vector>

class BindingSetLayout : public QueryInterface
{
public:
    virtual ~BindingSetLayout() = default;
};
