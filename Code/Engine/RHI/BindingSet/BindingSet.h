#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/QueryInterface.h>
#include <memory>
#include <vector>

class EZ_RHI_DLL BindingSet : public QueryInterface
{
public:
  virtual ~BindingSet() = default;
  virtual void WriteBindings(const std::vector<BindingDesc>& bindings) = 0;
};
