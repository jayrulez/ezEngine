#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Instance/BaseTypes.h>
#include <memory>

class EZ_RHI_DLL Pipeline : public QueryInterface
{
public:
    virtual ~Pipeline() = default;
    virtual PipelineType GetPipelineType() const = 0;
    virtual std::vector<uint8_t> GetRayTracingShaderGroupHandles(uint32_t first_group, uint32_t group_count) const = 0;
};
