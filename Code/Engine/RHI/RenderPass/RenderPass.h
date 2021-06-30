#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>
#include <memory>

class RenderPass : public QueryInterface
{
public:
    virtual ~RenderPass() = default;
    virtual const RenderPassDesc& GetDesc() const = 0;
};
