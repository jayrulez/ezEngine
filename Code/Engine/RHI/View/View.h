#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>
#include <memory>

class Resource;

class EZ_RHI_DLL View : public QueryInterface
{
public:
    virtual ~View() = default;
    virtual std::shared_ptr<Resource> GetResource() = 0;
    virtual uint32_t GetDescriptorId() const = 0;
    virtual uint32_t GetBaseMipLevel() const = 0;
    virtual uint32_t GetLevelCount() const = 0;
    virtual uint32_t GetBaseArrayLayer() const = 0;
    virtual uint32_t GetLayerCount() const = 0;
};
