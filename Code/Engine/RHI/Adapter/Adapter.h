#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Device/Device.h>
#include <RHI/Instance/QueryInterface.h>
#include <memory>
#include <string>

class EZ_RHI_DLL Adapter : public QueryInterface
{
public:
  virtual ~Adapter() = default;
  virtual const ezString& GetName() const = 0;
  virtual std::shared_ptr<Device> CreateDevice() = 0;
};
