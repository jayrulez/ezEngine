#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>

class QueryHeap : public QueryInterface
{
public:
    virtual ~QueryHeap() = default;
    virtual QueryHeapType GetType() const = 0;
};
