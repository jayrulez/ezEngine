#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>

class Memory : public QueryInterface
{
public:
    virtual ~Memory() = default;
    virtual MemoryType GetMemoryType() const = 0;
};
