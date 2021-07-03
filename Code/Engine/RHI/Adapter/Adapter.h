#pragma once
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Device/Device.h>
#include <string>
#include <memory>

class Adapter : public QueryInterface
{
public:
    virtual ~Adapter() = default;
    virtual const std::string& GetName() const = 0;
    virtual ezSharedPtr<Device> CreateDevice() = 0;
};
