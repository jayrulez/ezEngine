#pragma once
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Resource/Resource.h>
#include <memory>

class RenderPass : public QueryInterface
{
public:
  virtual ~RenderPass() = default;
  virtual const RenderPassDesc& GetDesc() const = 0;
};
