#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Timestamp.h>
#include <stdarg.h>

#ifdef BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT
extern "C"
{
#  include <HashLink/src/hl.h>
#  include <HashLink/src/hlc.h>
#  include <HashLink/src/hlmodule.h>
}

#  define HL_MAX_ARGS 9
#  define HL_MAX_FIELD_LEN 128



extern HL_PRIM hl_type hlt_array;
extern HL_PRIM hl_type hlt_bytes;
extern HL_PRIM hl_type hlt_dynobj;
extern HL_PRIM hl_type hlt_dyn;
extern HL_PRIM hl_type hlt_i32;
extern HL_PRIM hl_type hlt_i64;
extern HL_PRIM hl_type hlt_f32;
extern HL_PRIM hl_type hlt_f64;
extern HL_PRIM hl_type hlt_void;
extern HL_PRIM hl_type hlt_bool;
extern HL_PRIM hl_type hlt_abstract;

struct EZ_CORE_DLL ezHashLinkContext
{
  hl_code* m_pCode;
  hl_module* m_pModule;
  vdynamic* m_pReturn;
  ezString m_File;
  ezTimestamp m_FileTime;
};

class EZ_CORE_DLL ezHashLinkHelper
{
public:
  ezHashLinkHelper();
  void Initialize(const ezString& hlFile);
  void Shutdown();
  bool IsInitialized() const;
  const ezHashLinkContext& GetContext() const;

  vdynamic* CreateInstance(const ezString& className, const ezDynamicArray<vdynamic*>& ctorArgs);
  vdynamic* CallMethod(vdynamic* object, const char* method, ...);

private:
  bool m_bInitialized;
  ezHashLinkContext m_Context;
};

#endif
