#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Fence/Fence.h>
#include <RHI/CommandList/CommandList.h>

class CommandQueue : public QueryInterface
{
public:
    virtual ~CommandQueue() = default;
    virtual void Wait(const ezSharedPtr<Fence>& fence, uint64_t value) = 0;
    virtual void Signal(const ezSharedPtr<Fence>& fence, uint64_t value) = 0;
    virtual void ExecuteCommandLists(const std::vector<ezSharedPtr<CommandList>>& command_lists) = 0;
};
