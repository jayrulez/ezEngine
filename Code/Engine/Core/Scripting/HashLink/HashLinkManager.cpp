#include <Core/Scripting/HashLinkHelper.h>
#include <Core/Scripting/HashLinkManager.h>
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

static ezTimestamp GetFileModifiedTime(const char* filePath)
{
  ezFileStats stats;

  ezOSFile::GetFileStats(filePath, stats).IgnoreResult();

  return stats.m_LastModificationTime;
}

static hl_code* LoadCode(const char* filePath, char** errorMessage, bool printErrors)
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

static bool CheckReload(ezHashLinkManager* pHashLinkManager)
{
  ezTimestamp time = GetFileModifiedTime(pHashLinkManager->m_File);
  if (time.Compare(pHashLinkManager->m_FileModifiedTime, ezTimestamp::CompareMode::Identical))
    return false;

  char* errorMessage = nullptr;

  hl_code* code = LoadCode(pHashLinkManager->m_File, &errorMessage, false);
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

void ezHashLinkManager::Startup(ezString file, bool debugWait)
{
  if (m_Initialized)
    return;

  hl_global_init();

  char* args[] = {0};

  hl_sys_init((void**)args, 0, (void*)file.GetData());
  hl_register_thread(this);
  m_File = file;

  char* errorMessage = nullptr;

  m_pCode = LoadCode(m_File, &errorMessage, true);
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
    hl_setup_reload_check(CheckReload, this);
  }

  hl_code_free(m_pCode);

  if (m_DebugPort > 0 && !hl_module_debug(m_pModule, m_DebugPort, debugWait))
  {
    ezLog::Error("Could not start debugger on port {0}", m_DebugPort);
    return;
  }

  m_Initialized = true;
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
  vclosure closure;

  closure.t = m_pCode->functions[m_pModule->functions_indexes[m_pModule->code->entrypoint]].type;
  closure.fun = m_pModule->functions_ptrs[m_pModule->code->entrypoint];
  closure.hasValue = 0;

  SetupHandler();

  bool exceptionThrown = false;
  //hl_profile_setup(1);
  vdynamic* ret = hl_dyn_call_safe(&closure, nullptr, 0, &exceptionThrown);
  //hl_profile_end();

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
      vdynamic* global = *(vdynamic**)type->obj->global_value;
      vdynamic* globalObj = hl_alloc_obj(global->t);
      vdynamic* obj = hl_alloc_obj(type);

      int ctorHash = hl_hash((vbyte*)USTR("__constructor__"));
      hl_field_lookup* ctorField = obj_resolve_field(globalObj->t->obj, ctorHash);
      if (ctorField)
      {
        vdynamic* c = (vdynamic*)hl_dyn_getp(globalObj, ctorField->hashed_name, &hlt_dyn);

        vclosure* cl = (vclosure*)c;

        //vdynamic* vargs[1] = {obj};
        //hl_dyn_call(cl, vargs, 1);

        ((void (*)(vdynamic*))cl->fun)(obj); // this calls the constructor
      }

      int hfield = hl_hash_gen(USTR("execute"), false);
      hl_field_lookup* f = hl_lookup_find(type->obj->rt->lookup, type->obj->rt->nlookup, hfield);
      if (f)
      {
        vdynamic* ret = nullptr;
        hl_dyn_call_obj(obj, f->t, hfield, nullptr, ret);
      }
    }
  }
}

#endif
