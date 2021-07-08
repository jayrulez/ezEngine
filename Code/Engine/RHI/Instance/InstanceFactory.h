#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Basics.h>

struct EZ_RHI_DLL ezRHIInstanceFactory
{
  using CreatorFunc = ezDelegate<ezRHIInstance*()>;

  static ezRHIInstance* CreateInstance(ezRHIApiType type);

  static void RegisterCreatorFunc(ezRHIApiType type, const CreatorFunc& func);
  static void UnregisterCreatorFunc(ezRHIApiType type);
};
