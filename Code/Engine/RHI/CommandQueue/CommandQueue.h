#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Fence/Fence.h>
#include <RHI/CommandList/CommandList.h>

class CommandQueue : public QueryInterface
{
public:
    virtual ~CommandQueue() = default;
    virtual void Wait(const std::shared_ptr<Fence>& fence, uint64_t value) = 0;
    virtual void Signal(const std::shared_ptr<Fence>& fence, uint64_t value) = 0;
    virtual void ExecuteCommandLists(const std::vector<std::shared_ptr<CommandList>>& command_lists) = 0;
};
