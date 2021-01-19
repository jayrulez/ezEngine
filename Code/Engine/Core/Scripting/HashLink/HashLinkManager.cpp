#include <Core/Scripting/HashLinkHelper.h>
#include <Core/Scripting/HashLinkManager.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/OSFile.h>

#ifdef BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT

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

static bool CheckReload(ezHLContext* context)
{
  ezTimestamp time = GetFileModifiedTime(context->filePath);
  if (time.Compare(context->fileModifiedTime, ezTimestamp::CompareMode::Identical))
    return false;

  char* errorMessage = nullptr;

  hl_code* code = LoadCode(context->filePath, &errorMessage, false);
  if (code == nullptr)
    return false;

  bool changed = hl_module_patch(context->module, code);
  context->fileModifiedTime = time;
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
{
}

ezHashLinkManager::~ezHashLinkManager()
{
}

void ezHashLinkManager::Startup()
{
  if (m_Initialized)
    return;

  hl_global_init();
  m_Initialized = true;
}

void ezHashLinkManager::Shutdown()
{
  if (!m_Initialized)
    return;

  // unload all contexts

  m_Initialized = false;
}

ezResult ezHashLinkManager::RunModule(const char* modulePath, bool hotReload, bool debug)
{
  ezHLContext context;

  hl_sys_init(nullptr, 0, &modulePath);
  hl_register_thread(&context);
  context.filePath = modulePath;

  char* errorMessage = nullptr;

  context.code = LoadCode(nullptr, &errorMessage, true);
  if (context.code == nullptr)
  {
    if (errorMessage)
      ezLog::Error(errorMessage);
    return EZ_FAILURE;
  }

  context.module = hl_module_alloc(context.code);
  if (context.module == nullptr)
  {
    return EZ_FAILURE;
  }

  if (!hl_module_init(context.module, hotReload))
    return EZ_FAILURE;

  if (hotReload)
  {
    context.fileModifiedTime = GetFileModifiedTime(context.filePath);
    hl_setup_reload_check(CheckReload, &context);
  }
  hl_code_free(context.code);

  // todo: load these from config file?
  ezInt32 debugPort = 7890;
  bool debugWait = false;
  if (debug && !hl_module_debug(context.module, debugPort, debugWait))
  {
    ezLog::Error("Could not start debugger on port {}", debugPort);
    return EZ_FAILURE;
  }

  vclosure closure;

  closure.t = context.code->functions[context.module->functions_indexes[context.module->code->entrypoint]].type;
  closure.fun = context.module->functions_ptrs[context.module->code->entrypoint];
  closure.hasValue = 0;

  SetupHandler();

  bool exceptionThrown = false;
  hl_profile_setup(1);
  context.ret = hl_dyn_call_safe(&closure, nullptr, 0, &exceptionThrown);
  hl_profile_end();

  if (exceptionThrown)
  {
    varray* stack = hl_exception_stack();
    ezLog::Info("Uncaught exception: {}", hl_to_string(context.ret));
    for (ezInt32 index = 0; index < stack->size; index++)
    {
      ezLog::Info("Called from {}", hl_aptr(stack, uchar*)[index]);
    }
    hl_debug_break();
    hl_global_free();
    return EZ_FAILURE;
  }

  hl_module_free(context.module);
  hl_free(&context.code->alloc);
  return EZ_SUCCESS;
}

#endif
