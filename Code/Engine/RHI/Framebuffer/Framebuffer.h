#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/Instance/QueryInterface.h>
#include <memory>
#include <RHI/BindingSetLayout/BindingSetLayout.h>

class EZ_RHI_DLL Framebuffer : public QueryInterface
{
public:
    virtual ~Framebuffer() = default;
};
