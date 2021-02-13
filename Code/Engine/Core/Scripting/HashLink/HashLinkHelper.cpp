#include <Core/Scripting/HashLinkHelper.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/OSFile.h>

#ifdef BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT

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

static bool ez_hl_initialized;

static ezTimestamp get_file_modified_time(const char* filePath)
{
  ezFileStats fileStats;
  ezOSFile::GetFileStats(filePath, fileStats).IgnoreResult();
  return fileStats.m_LastModificationTime;
}

static hl_field_lookup* ez_hl_obj_resolve_field(hl_type_obj* o, int hfield)
{
  hl_runtime_obj* rt = o->rt;
  do
  {
    hl_field_lookup* f = hl_lookup_find(rt->lookup, rt->nlookup, hfield);
    if (f)
      return f;
    rt = rt->parent;
  } while (rt);
  return NULL;
}

static vdynamic* ez_hl_create_instance(hl_type* type, const ezDynamicArray<vdynamic*>& ctorArgs)
{
  vdynamic* global = *(vdynamic**)type->obj->global_value;
  if (global == nullptr)
  {
    ezLog::Error("Cannot instantiate class from type.");
    return nullptr;
  }

  vdynamic* obj = hl_alloc_obj(type);
  if (obj == nullptr)
  {
    ezLog::Error("Failed to allocate instance.");
    return nullptr;
  }
  hl_add_root(&obj);

  ezInt32 ctorHash = hl_hash((vbyte*)USTR("__constructor__"));
  hl_field_lookup* ctorField = ez_hl_obj_resolve_field(global->t->obj, ctorHash);
  if (ctorField == nullptr)
  {
    ezLog::Error("Type does not have a constructor.");
    return nullptr;
  }

  vclosure* ctorClosure = (vclosure*)hl_dyn_getp(global, ctorField->hashed_name, &hlt_dyn);

  ezUInt32 nargs = (ezUInt32)ctorClosure->t->fun->nargs;

  vdynamic* args[HL_MAX_ARGS + 1];
  //args[0] = obj;
  for (ezUInt32 i = 0; i < ctorArgs.GetCount(); i++)
  {
    args[i] = ctorArgs[i];
  }

  ctorClosure->value = obj;
  hl_dyn_call((vclosure*)ctorClosure, args, nargs);

  //((vdynamic* (*)(vdynamic*, vdynamic**, ezInt32))ctorClosure->fun)(obj, args, nargs); // this calls the constructor

  return obj;
}

static hl_code* ez_hl_load_code(const char* filePath, char** errorMessage, bool printErrors)
{
  hl_code* code = nullptr;

  ezFileReader codeStream;

  if (!codeStream.Open(filePath).Succeeded())
  {
    if (printErrors)
      ezLog::Error("Cannot load code at path '{0}' because the file doesn't exist.", filePath);
    return nullptr;
  }

  ezUInt32 codeSize = (ezUInt32)codeStream.GetFileSize();
  ezDynamicArray<unsigned char> codeData;
  codeData.SetCountUninitialized(codeSize);
  codeStream.ReadBytes(codeData.GetData(), codeStream.GetFileSize());
  codeStream.Close();

  code = hl_code_read(codeData.GetData(), codeSize, errorMessage);

  return code;
}

static bool ez_hl_check_reload(ezHashLinkContext* pContext)
{
  ezTimestamp time = get_file_modified_time(pContext->m_File);

  if (time.Compare(pContext->m_FileTime, ezTimestamp::CompareMode::Identical))
    return false;

  char* errorMessage = nullptr;

  hl_code* code = ez_hl_load_code(pContext->m_File, &errorMessage, false);
  if (code == nullptr)
    return false;

  bool changed = hl_module_patch(pContext->m_pModule, code);
  pContext->m_FileTime = time;
  hl_code_free(code);

  return changed;
}

static void ez_hl_setup_handler() {}

struct HlOptions
{
  bool debugWait = false;
  ezInt32 debugPort = 8888;
  bool enableHotReload = false;
  ezInt32 profileCount = -1;
};

static HlOptions defaultHLOptions = {};

static ezResult ez_hl_startup(ezHashLinkContext* pContext, HlOptions options = defaultHLOptions)
{
  if (ez_hl_initialized)
    return EZ_SUCCESS;
    
  ezInt32 profileCount = -1;
  hl_global_init();

  hl_sys_init((void**)NULL, 0, (void*)pContext->m_File.GetData());
  hl_register_thread(&profileCount);

  char* errorMessage = nullptr;

  pContext->m_pCode = ez_hl_load_code(pContext->m_File, &errorMessage, true);
  if (pContext->m_pCode == nullptr)
  {
    if (errorMessage)
      ezLog::Error(errorMessage);
    return EZ_FAILURE;
  }

  pContext->m_pModule = hl_module_alloc(pContext->m_pCode);
  if (pContext->m_pModule == nullptr)
  {
    return EZ_FAILURE;
  }

  if (!hl_module_init(pContext->m_pModule, options.enableHotReload))
    return EZ_FAILURE;

  if (options.enableHotReload)
  {
    pContext->m_FileTime = get_file_modified_time(pContext->m_File);
    hl_setup_reload_check(ez_hl_check_reload, pContext);
  }

  hl_code_free(pContext->m_pCode);

  if (options.debugPort > 0 && !hl_module_debug(pContext->m_pModule, options.debugPort, options.debugWait))
  {
    ezLog::Error("Could not start debugger on port {0}", options.debugPort);
    return EZ_FAILURE;
  }

  vclosure closure;
  closure.t = pContext->m_pCode->functions[pContext->m_pModule->functions_indexes[pContext->m_pModule->code->entrypoint]].type;
  closure.fun = pContext->m_pModule->functions_ptrs[pContext->m_pModule->code->entrypoint];
  closure.hasValue = 0;

  ez_hl_setup_handler();

  bool exceptionThrown = false;
  hl_profile_setup(profileCount);
  vdynamic* ret = hl_dyn_call_safe(&closure, nullptr, 0, &exceptionThrown);
  hl_profile_end();

  if (exceptionThrown)
  {
    varray* stack = hl_exception_stack();
    ezLog::Info("Uncaught exception: {}", hl_to_string(ret));
    for (ezInt32 index = 0; index < stack->size; index++)
    {
      ezLog::Info("Called from {}", hl_aptr(stack, uchar*)[index]);
    }
    hl_debug_break();
    hl_global_free();
    return EZ_FAILURE;
  }
  else
  {
    ez_hl_initialized = true;
    return EZ_SUCCESS;
  }
}

static void ez_hl_shutdown(ezHashLinkContext* pContext)
{
  if (!ez_hl_initialized)
    return;
  hl_module_free(pContext->m_pModule);
  hl_free(&pContext->m_pCode->alloc);
  hl_global_free();

  ez_hl_initialized = false;
}

ezHashLinkHelper::ezHashLinkHelper()
  : m_bInitialized{false}
{
}

void ezHashLinkHelper::Initialize(const ezString& hlFile)
{
  if (m_bInitialized)
    return;

  m_Context.m_File = hlFile;
  if (ez_hl_startup(&m_Context).Succeeded())
  {
    m_bInitialized = true;
  }
}

void ezHashLinkHelper::Shutdown()
{
  if (m_bInitialized)
    ez_hl_shutdown(&m_Context);
}

bool ezHashLinkHelper::IsInitialized() const
{
  return m_bInitialized;
}

const ezHashLinkContext& ezHashLinkHelper::GetContext() const
{
  return m_Context;
}

vdynamic* ezHashLinkHelper::CreateInstance(const ezString& className, const ezDynamicArray<vdynamic*>& ctorArgs)
{
  if (!m_bInitialized)
  {
    EZ_REPORT_FAILURE("HashLink is not initialized.");
    return nullptr;
  }

  for (ezInt32 i = 0; i < m_Context.m_pModule->code->ntypes; i++)
  {
    hl_type* type = m_Context.m_pModule->code->types + i;

    if (type->kind != HOBJ)
      continue;

    // todo: cache this
    if (ucmp(type->obj->name, ezStringWChar(className.GetData())) == 0)
    {
      vdynamic* obj = ez_hl_create_instance(type, ctorArgs);
      return obj;
    }
  }

  return nullptr;
}

// todo: create abstraction that caches classes and methods
vdynamic* ezHashLinkHelper::CallMethod(vdynamic* object, const char* method, ...)
{
  if (!m_bInitialized)
  {
    EZ_REPORT_FAILURE("HashLink is not initialized.");
    return nullptr;
  }

  ezStringWChar methodName = ezStringWChar(method);
  ezInt32 hfield = hl_hash((vbyte*)methodName.GetData());
  
  hl_field_lookup* f = hl_lookup_find(object->t->obj->rt->lookup, object->t->obj->rt->nlookup, hfield);
  if (f)
  {
    if (f->t->kind == HFUN)
    {
      ezInt32 nargs = f->t->fun->nargs;
      va_list argPtr;
      vdynamic* args[HL_MAX_ARGS + 1];
      va_start(argPtr, method);
      for (ezInt32 i = 0; i < nargs; i++)
      {
        args[i] = va_arg(argPtr, vdynamic*);
      }
      va_end(argPtr);

      vdynamic* ret = nullptr;
      hl_dyn_call_obj(object, f->t, hfield, (void**)args, ret);

      return ret;
    }
  }

  return nullptr;
}

#endif
