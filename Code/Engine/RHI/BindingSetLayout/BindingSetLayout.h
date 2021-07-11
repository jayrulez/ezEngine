#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/QueryInterface.h>
#include <memory>
#include <vector>

class EZ_RHI_DLL BindingSetLayout : public QueryInterface
{
public:
  virtual ~BindingSetLayout() = default;
};
