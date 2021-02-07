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

HL_PRIM hl_type hlt_array = {HARRAY};
HL_PRIM hl_type hlt_bytes = {HBYTES};
HL_PRIM hl_type hlt_dynobj = {HDYNOBJ};
HL_PRIM hl_type hlt_dyn = {HDYN};
HL_PRIM hl_type hlt_i32 = {HI32};
HL_PRIM hl_type hlt_i64 = {HI64};
HL_PRIM hl_type hlt_f32 = {HF32};
HL_PRIM hl_type hlt_f64 = {HF64};
HL_PRIM hl_type hlt_void = {HVOID};
HL_PRIM hl_type hlt_bool = {HBOOL};
HL_PRIM hl_type hlt_abstract = {HABSTRACT, {USTR("<abstract>")}};

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
  const ezHashLinkContext& GetContext() const;

  vdynamic* CreateInstance(const ezString& className, va_list args);
  vdynamic* CallMethod(vdynamic* object, const ezString& method, ...);

private:
  bool m_bInitialized;
  ezHashLinkContext m_Context;
};

#endif
