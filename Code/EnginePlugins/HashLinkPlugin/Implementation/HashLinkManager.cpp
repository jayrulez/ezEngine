#include "..\HashLinkManager.h"
#include <Core/Scripting/HashLinkHelper.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/OSFile.h>
#include <HashLinkPlugin/HashLinkManager.h>

#ifdef BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT

extern "C"
{
  HL_PRIM vdynamic* hl_obj_get_field(vdynamic* obj, int hfield);
  HL_PRIM vdynamic* hl_call_method(vdynamic* c, varray* args);
}

// Helper functions
static ezString GetAbsolutePath(const ezString& path);

static ezTimestamp GetFileModifiedTime(const char* filePath);

// hl helper functions

static hl_field_lookup* obj_resolve_field(hl_type_obj* o, int hfield)
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

static vdynamic* dyn_call_constructor(vclosure* c, vdynamic* ctor, vdynamic** args, int nargs)
{
  struct
  {
    varray a;
    vdynamic* args[HL_MAX_ARGS + 1];
  } tmp;
  vclosure ctmp;
  int i = 0;
  if (nargs > HL_MAX_ARGS)
    hl_error("Too many arguments");
  tmp.a.t = &hlt_array;
  tmp.a.at = &hlt_dyn;
  tmp.a.size = nargs;
  if (c->hasValue && c->t->fun->nargs >= 0)
  {
    ctmp.t = c->t->fun->parent;
    ctmp.hasValue = 0;
    ctmp.fun = c->fun;
    //tmp.args[0] = hl_make_dyn(&c->value, ctmp.t->fun->args[0]);
    tmp.args[0] = ctor;
    tmp.a.size++;
    for (i = 0; i < nargs; i++)
      tmp.args[i + 1] = args[i];
    c = &ctmp;
  }
  else
  {
    for (i = 0; i < nargs; i++)
      tmp.args[i] = args[i];
  }
  return hl_call_method((vdynamic*)c, &tmp.a);
}

static vdynamic* create_instance(hl_type* type, ...)
{
  vdynamic* instMain = hl_alloc_obj(type);
  if (instMain == nullptr)
  {
    ezLog::Error("Failed to allocate instance.");
    return nullptr;
  }

  vdynamic* global = *(vdynamic**)type->obj->global_value;
  vdynamic* instGlobal = hl_alloc_obj(global->t);
  if (instGlobal == nullptr)
  {
    ezLog::Error("Failed to allocate global instance.");
    return nullptr;
  }

  ezInt32 ctorHash = hl_hash((vbyte*)USTR("__constructor__"));
  hl_field_lookup* ctorField = obj_resolve_field(instGlobal->t->obj, ctorHash);
  if (ctorField == nullptr)
  {
    ezLog::Error("Type does not have a constructor.");
    return nullptr;
  }

  vclosure* ctorClosure = (vclosure*)hl_obj_get_field(instGlobal, ctorHash);

  ezUInt32 nargs = (ezUInt32)ctorClosure->t->fun->nargs;
  va_list argPtr;
  va_start(argPtr, type);
  vdynamic* args[HL_MAX_ARGS + 1];
  for (ezUInt32 i = 0; i < nargs; i++)
  {
    args[i] = va_arg(argPtr, vdynamic*);
  }
  va_end(argPtr);

  dyn_call_constructor((vclosure*)ctorClosure, instMain, args, nargs);

  return instMain;
}

static vdynamic* create_instance2(hl_type* type, vdynamic*...)
{
  vdynamic* global = *(vdynamic**)type->obj->global_value;
  if (global == nullptr)
  {
    ezLog::Error("Cannot instantiate class from type.");
    return nullptr;
  }
  vdynamic* globalObj = hl_alloc_obj(global->t);
  if (globalObj == nullptr)
  {
    ezLog::Error("Failed to allocate global instance.");
    return nullptr;
  }
  vdynamic* obj = hl_alloc_obj(type);
  if (obj == nullptr)
  {
    ezLog::Error("Failed to allocate instance.");
    return nullptr;
  }

  ezInt32 ctorHash = hl_hash((vbyte*)USTR("__constructor__"));
  hl_field_lookup* ctorField = obj_resolve_field(globalObj->t->obj, ctorHash);
  if (ctorField == nullptr)
  {
    ezLog::Error("Type does not have a constructor.");
    return nullptr;
  }
  vdynamic* c = (vdynamic*)hl_dyn_getp(globalObj, ctorField->hashed_name, &hlt_dyn);

  vclosure* ctorClosure = (vclosure*)c;

  //vdynamic* vargs[1] = {obj};
  //hl_dyn_call(cl, vargs, 1);

  ezUInt32 nargs = (ezUInt32)ctorClosure->t->fun->nargs;
  auto ctorArgs = ctorClosure->t->fun->args;

  va_list argPtr;
  va_start(argPtr, type);
  vdynamic* args[HL_MAX_ARGS + 1];
  args[0] = obj;
  for (ezUInt32 i = 0; i < nargs; i++)
  {
    args[i + 1] = va_arg(argPtr, vdynamic*);
  }
  va_end(argPtr);

  //vdynamic arg1;
  //arg1.t = &hlt_i32;
  //arg1.v.i = 15;
  //vdynamic* args[1] = {&arg1};

  ctorClosure->value = obj;
  hl_dyn_call(ctorClosure, args, 1);
  //((void (*)(vdynamic*, vdynamic**, ezInt32))ctorClosure->fun)(obj, args, nargs); // this calls the constructor


  return obj;
}

static vdynamic* create_instance3(hl_type* type, ...)
{
  vdynamic* global = *(vdynamic**)type->obj->global_value;
  if (global == nullptr)
  {
    ezLog::Error("Cannot instantiate class from type.");
    return nullptr;
  }

  vdynamic* globalObj = hl_alloc_obj(global->t);
  if (globalObj == nullptr)
  {
    ezLog::Error("Failed to allocate global instance.");
    return nullptr;
  }

  vdynamic* obj = hl_alloc_obj(type);
  if (obj == nullptr)
  {
    ezLog::Error("Failed to allocate instance.");
    return nullptr;
  }

  ezInt32 ctorHash = hl_hash((vbyte*)USTR("__constructor__"));
  hl_field_lookup* ctorField = obj_resolve_field(globalObj->t->obj, ctorHash);
  if (ctorField == nullptr)
  {
    ezLog::Error("Type does not have a constructor.");
    return nullptr;
  }

  vclosure* ctorClosure = (vclosure*)hl_dyn_getp(globalObj, ctorField->hashed_name, &hlt_dyn);

  ezUInt32 nargs = (ezUInt32)ctorClosure->t->fun->nargs;

  va_list argPtr;
  va_start(argPtr, type);
  vdynamic* args[HL_MAX_ARGS + 1];
  for (ezUInt32 i = 0; i < nargs; i++)
  {
    args[i] = va_arg(argPtr, vdynamic*);
  }
  va_end(argPtr);

  ctorClosure->value = obj;
  hl_dyn_call(ctorClosure, args, nargs);
  //((void (*)(vdynamic*, vdynamic**, ezInt32))ctorClosure->fun)(obj, args, nargs); // this calls the constructor

  return obj;
}

static vdynamic* create_instance4(hl_type* type, vdynamic* outObject, ...)
{
  vdynamic* global = *(vdynamic**)type->obj->global_value;
  if (global == nullptr)
  {
    ezLog::Error("Cannot instantiate class from type.");
    return nullptr;
  }

  vdynamic* globalObj = hl_alloc_obj(global->t);
  if (globalObj == nullptr)
  {
    ezLog::Error("Failed to allocate global instance.");
    return nullptr;
  }

  outObject = hl_alloc_obj(type);
  if (outObject == nullptr)
  {
    ezLog::Error("Failed to allocate instance.");
    return nullptr;
  }

  ezInt32 ctorHash = hl_hash((vbyte*)USTR("__constructor__"));
  hl_field_lookup* ctorField = obj_resolve_field(globalObj->t->obj, ctorHash);
  if (ctorField == nullptr)
  {
    ezLog::Error("Type does not have a constructor.");
    return nullptr;
  }

  vclosure* ctorClosure = (vclosure*)hl_dyn_getp(globalObj, ctorField->hashed_name, &hlt_dyn);

  ezUInt32 nargs = (ezUInt32)ctorClosure->t->fun->nargs;

  va_list argPtr;
  va_start(argPtr, type);
  vdynamic* args[HL_MAX_ARGS + 1];
  for (ezUInt32 i = 0; i < nargs; i++)
  {
    args[i] = va_arg(argPtr, vdynamic*);
  }
  va_end(argPtr);

  ctorClosure->value = outObject;
  hl_dyn_call(ctorClosure, args, nargs);
  //((void (*)(vdynamic*, vdynamic**, ezInt32))ctorClosure->fun)(obj, args, nargs); // this calls the constructor

  return outObject;
}

static hl_code* load_code(const char* filePath, char** errorMessage, bool printErrors)
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

static bool check_reload(ezHashLinkManager* pHashLinkManager)
{
  ezTimestamp time = GetFileModifiedTime(pHashLinkManager->m_File);
  if (time.Compare(pHashLinkManager->m_FileModifiedTime, ezTimestamp::CompareMode::Identical))
    return false;

  char* errorMessage = nullptr;

  hl_code* code = load_code(pHashLinkManager->m_File, &errorMessage, false);
  if (code == nullptr)
    return false;

  bool changed = hl_module_patch(pHashLinkManager->m_pModule, code);
  pHashLinkManager->m_FileModifiedTime = time;
  hl_code_free(code);

  return changed;
}

static void SetupHandler()
{
}

EZ_IMPLEMENT_SINGLETON(ezHashLinkManager);

ezHashLinkManager::ezHashLinkManager()
  : m_SingletonRegistrar{this}
  , m_Initialized{false}
  , m_pCode{nullptr}
  , m_pModule{nullptr}
  , m_DebugPort{0}
  , m_bEnableHotReload{true}
{
}

ezHashLinkManager::~ezHashLinkManager()
{
}

struct pTest
{
  int i = 0;
};

void ezHashLinkManager::Startup(const ezString& file, void* threadStackTop, bool debugWait)
{
  if (m_Initialized)
    return;

  ezInt32 profileCount = -1;

  hl_global_init();

  char* args[] = {0};
  m_File = GetAbsolutePath(file);

  hl_sys_init((void**)NULL, 0, (void*)m_File.GetData());
  pTest p;

  hl_register_thread(&p);

  char* errorMessage = nullptr;

  m_pCode = load_code(m_File, &errorMessage, true);
  if (m_pCode == nullptr)
  {
    if (errorMessage)
      ezLog::Error(errorMessage);
    return;
  }

  m_pModule = hl_module_alloc(m_pCode);
  if (m_pModule == nullptr)
  {
    return;
  }

  if (!hl_module_init(m_pModule, m_bEnableHotReload))
    return;

  if (m_bEnableHotReload)
  {
    m_FileModifiedTime = GetFileModifiedTime(m_File);
    hl_setup_reload_check(check_reload, this);
  }

  hl_code_free(m_pCode);

  if (m_DebugPort > 0 && !hl_module_debug(m_pModule, m_DebugPort, debugWait))
  {
    ezLog::Error("Could not start debugger on port {0}", m_DebugPort);
    return;
  }


  vclosure closure;

  closure.t = m_pCode->functions[m_pModule->functions_indexes[m_pModule->code->entrypoint]].type;
  closure.fun = m_pModule->functions_ptrs[m_pModule->code->entrypoint];
  closure.hasValue = 0;

  SetupHandler();

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
  }
  else
  {




    for (int i = 0; i < 1000; i++)
    {
      ezLog::Info("Run {}", i + 1);
      Test3();
    }




    m_Initialized = true;
  }
}

void ezHashLinkManager::Shutdown()
{
  if (!m_Initialized)
    return;

  hl_module_free(m_pModule);
  hl_free(&m_pCode->alloc);
  hl_global_free();

  m_Initialized = false;
}

ezResult ezHashLinkManager::Run()
{
  //if (m_Initialized)
  //{
  //  vclosure closure;

  //  closure.t = m_pCode->functions[m_pModule->functions_indexes[m_pModule->code->entrypoint]].type;
  //  closure.fun = m_pModule->functions_ptrs[m_pModule->code->entrypoint];
  //  closure.hasValue = 0;

  //  SetupHandler();

  //  bool exceptionThrown = false;
  //  //hl_profile_setup(1);
  //  vdynamic* ret = hl_dyn_call_safe(&closure, nullptr, 0, &exceptionThrown);
  //  //hl_profile_end();

  //  if (exceptionThrown)
  //  {
  //    varray* stack = hl_exception_stack();
  //    ezLog::Info("Uncaught exception: {}", hl_to_string(ret));
  //    for (ezInt32 index = 0; index < stack->size; index++)
  //    {
  //      ezLog::Info("Called from {}", hl_aptr(stack, uchar*)[index]);
  //    }
  //    hl_debug_break();
  //    hl_global_free();
  //    return EZ_FAILURE;
  //  }
  //}

  return EZ_SUCCESS;
}

//struct ezHLField
//{
//
//};
//
//struct ezHLMethod
//{
//
//};
//
//struct ezHLClass
//{
//  ezDynamicArray<ezHLField*> m_Fields;
//  ezDynamicArray<ezHLMethod*> m_Methods;
//};

void ezHashLinkManager::Test()
{
  for (ezInt32 i = 0; i < m_pModule->code->ntypes; i++)
  {
    hl_type* type = m_pModule->code->types + i;

    if (type->kind != HOBJ)
      continue;

    auto testClass = USTR("test2x.TestClass");
    if (ucmp(type->obj->name, testClass) == 0)
    {
      vdynamic* global = *(vdynamic**)type->obj->global_value;
      vdynamic* globalObj = hl_alloc_obj(global->t);
      vdynamic* obj = hl_alloc_obj(type);

      //auto x = obj->t->obj->global_value;

      int ctorHash = hl_hash((vbyte*)USTR("__constructor__"));
      hl_field_lookup* ctorField = obj_resolve_field(global->t->obj, ctorHash); // hl_lookup_find(global->t->obj->rt->lookup, global->t->obj->rt->nlookup, ctorHash);
      if (ctorField)
      {
        vdynamic* c = (vdynamic*)hl_dyn_getp(globalObj, ctorField->hashed_name, &hlt_dyn);

        vclosure* cl = (vclosure*)c;

        cl->value = obj;

        //(((void*)(vdynamic*))cl->fun)((vdynamic*)cl->value);
        ((void (*)(vdynamic*))cl->fun)((vdynamic*)cl->value);

        //hl_add_root(globalObj);
        //hl_add_root(obj);

        //cl->hasValue = 0;

        //hl_dyn_call_obj(c, c->t, c->v.i, nullptr, 0);
        //hl_dyn_call(cl, nullptr, 0);
        //vclosure v;
        //v.t = globalObj->t->obj->m->functions_types[ctorField->field_index];
        //v.fun = globalObj->t->obj->m->functions_ptrs[ctorField->field_index];
        //v.hasValue = 0;
        //vdynamic* ret = nullptr;
        //hl_dyn_call_obj(globalObj, ctorField->t, ctorHash, nullptr, ret);
        //vclosure cl;
        //cl.fun = ctorField->hashed_name;
        //cl.t = ctorField->t;
        //cl.hasValue = 0;
        //vdynamic* ret = nullptr;
        //hl_dyn_call(&cl, nullptr, 0);
        //hl_dyn_call_obj(global, ctorField->t, ctorHash, nullptr, ret);
      }

      int hfield = hl_hash_gen(USTR("execute"), false);
      hl_field_lookup* f = hl_lookup_find(type->obj->rt->lookup, type->obj->rt->nlookup, hfield);
      if (f)
      {
        vdynamic* ret = nullptr;
        hl_dyn_call_obj(obj, f->t, hfield, nullptr, ret);
      }

      for (int fieldIndex = 0; fieldIndex < type->obj->rt->nmethods; fieldIndex++)
      {
        void** field = type->obj->rt->methods + fieldIndex;

        ezLog::Debug("method: {0}", field);
      }
      //hl_dyn_call_obj(obj,type, (void**)ctor, nullptr, );


      //hl_runtime_obj* rt = hl_get_obj_rt(type);

      //for (int j = 0; j < rt->nmethods; j++)
      //{
      //  void* method = rt->methods+j;
      //}
    }
  }
}

void ezHashLinkManager::Test2()
{
  for (ezInt32 i = 0; i < m_pModule->code->ntypes; i++)
  {
    hl_type* type = m_pModule->code->types + i;

    if (type->kind != HOBJ)
      continue;

    auto testClass = USTR("test2x.TestClass");
    if (ucmp(type->obj->name, testClass) == 0)
    {
      vdynamic* obj = create_instance(type);

      ezInt32 hfield = hl_hash_gen(USTR("execute"), false);
      hl_field_lookup* f = hl_lookup_find(type->obj->rt->lookup, type->obj->rt->nlookup, hfield);
      if (f)
      {
        vdynamic* ret = nullptr;
        hl_dyn_call_obj(obj, f->t, hfield, nullptr, ret);
      }
    }
  }
}

void ezHashLinkManager::Test3()
{
  for (ezInt32 i = 0; i < m_pModule->code->ntypes; i++)
  {
    hl_type* type = m_pModule->code->types + i;

    if (type->kind != HOBJ)
      continue;

    auto testClass = USTR("test2x.TestClass");
    if (ucmp(type->obj->name, testClass) == 0)
    {
      
      vdynamic* arg1 = hl_alloc_dynamic(&hlt_i32);
      arg1->v.i = 15;
      vdynamic* arg2 = hl_alloc_dynamic(&hlt_i32);
      arg2->v.i = 134;
      vdynamic* obj = create_instance3(type, arg1, arg2);

      ezInt32 hfield = hl_hash((vbyte*)USTR("execute"));
      hl_field_lookup* f = hl_lookup_find(type->obj->rt->lookup, type->obj->rt->nlookup, hfield);
      if (f)
      {
        vdynamic* ret = nullptr;
        hl_dyn_call_obj(obj, f->t, hfield, nullptr, ret);
      }
    }
  }
}

ezString GetAbsolutePath(const ezString& file)
{
  ezStringBuilder path;

  if (!ezPathUtils::IsAbsolutePath(file))
  {
    if (ezFileSystem::ResolveSpecialDirectory(">project", path).Succeeded())
    {
      path.Append("/", file.GetData());
      if (ezFileSystem::ExistsFile(path))
      {
        path.MakeCleanPath();
        return path;
      }
    }

    path = ezOSFile::GetApplicationDirectory();
    path.Append(file.GetData());

    // todo: verify that file exists
  }
  else
  {
    path = file;
  }

  path.MakeCleanPath();
  return path;
}

ezTimestamp GetFileModifiedTime(const char* filePath)
{
  ezFileStats stats;

  ezOSFile::GetFileStats(filePath, stats).IgnoreResult();

  return stats.m_LastModificationTime;
}

#endif
