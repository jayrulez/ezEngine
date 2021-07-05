#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

class EZ_RHI_DLL ezRHIAdapter : public ezRHIQueryInterface
{
public:
  virtual ~ezRHIAdapter() = default;

  const ezString& GetName() const;
  ezRHIDevice* CreateDevice();

protected:
  virtual const ezString& GetNamePlatform() const = 0;
  virtual ezRHIDevice* CreateDevicePlatform() = 0;
};
