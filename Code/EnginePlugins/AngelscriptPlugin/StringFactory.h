#pragma once

#include <AngelscriptPlugin/AngelscriptManager.h>
#include <Foundation/Containers/ArrayMap.h>

class ezStringFactory : public asIStringFactory
{
public:
  ezStringFactory();
  ~ezStringFactory();

  const void* GetStringConstant(const char* data, asUINT length);

  ezInt32 ReleaseStringConstant(const void* str);

  ezInt32 GetRawStringData(const void* str, char* data, asUINT* length) const;

  ezArrayMap<ezString, ezUInt32> m_StringCache;
};
