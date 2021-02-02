#include <AngelscriptPlugin/AngelscriptBuilder.h>
#include <AngelscriptPlugin/AngelscriptManager.h>
#include <Core/Scripting/AngelscriptHelper.h>
#include <Foundation/Containers/ArrayMap.h>

#ifdef BUILDSYSTEM_ENABLE_ANGELSCRIPT_SUPPORT


class ezStringFactory : public asIStringFactory
{
public:
  ezStringFactory() {}
  ~ezStringFactory()
  {
    EZ_ASSERT_ALWAYS(m_StringCache.GetCount() == 0, "The script engine must release each string constant that it has requested");
  }

  const void* GetStringConstant(const char* data, asUINT length)
  {
    // The string factory might be modified from multiple
    // threads, so it is necessary to use a mutex.
    asAcquireExclusiveLock();

    ezString str(data);

    auto it = m_StringCache.Find(str);
    if (it != end(m_StringCache))
    {
      m_StringCache[str]++;
    }
    else
    {
      it = m_StringCache.Insert(str, 1);
    }

    asReleaseExclusiveLock();

    return reinterpret_cast<const void*>(&it.Key());
  }

  ezInt32 ReleaseStringConstant(const void* str)
  {
    if (!str)
    {
      return asERROR;
    }

    ezInt32 result = asSUCCESS;

    asAcquireExclusiveLock();
    const ezString* key = reinterpret_cast<const ezString*>(str);
    auto it = m_StringCache.Find(*key);

    if (it == end(m_StringCache))
    {
      result = asERROR;
    }
    else
    {
      m_StringCache[*key]--;
      if (m_StringCache[*key] == 0)
      {
        m_StringCache.Remove(*key);
      }
    }

    asReleaseExclusiveLock();

    return result;
  }

  ezInt32 GetRawStringData(const void* str, char* data, asUINT* length) const
  {
    if (!str)
    {
      return asERROR;
    }

    const ezString* nativeStr = reinterpret_cast<const ezString*>(str);

    if (length)
    {
      *length = nativeStr->GetElementCount();
    }

    if (data)
    {
      ezMemoryUtils::Copy(data, const_cast<char*>((*nativeStr).GetData()), nativeStr->GetElementCount());
    }

    return asSUCCESS;
  }

  ezMap<ezString, ezUInt32> m_StringCache;
};

void MessageCallback(const asSMessageInfo* msg, void* param)
{
  switch (msg->type)
  {
    case asMSGTYPE_ERROR:
      ezLog::Error("{0} ({1}, {2}) : {3} : {4}", msg->section, msg->row, msg->col, "ERR ", msg->message);
      break;

    case asMSGTYPE_WARNING:
      ezLog::Warning("{0} ({1}, {2}) : {3} : {4}", msg->section, msg->row, msg->col, "WARN", msg->message);
      break;

    case asMSGTYPE_INFORMATION:
    default:
      ezLog::Info("{0} ({1}, {2}) : {3} : {4}", msg->section, msg->row, msg->col, "INFO", msg->message);
      break;
  }
}

EZ_IMPLEMENT_SINGLETON(ezAngelscriptManager);

ezAngelscriptManager::ezAngelscriptManager()
  : m_SingletonRegistrar{this}
  , m_Initialized{false}
  , m_pScriptEngine{nullptr}
{
  m_pScriptBuilder = EZ_DEFAULT_NEW(ezAngelscriptBuilder);
}

ezAngelscriptManager::~ezAngelscriptManager()
{
  Shutdown();
  if (m_pScriptBuilder)
    EZ_DEFAULT_DELETE(m_pScriptBuilder);
}

void ezAngelscriptManager::Startup()
{
  if (m_Initialized)
    return;

  m_pScriptEngine = asCreateScriptEngine();
  if (!m_pScriptEngine)
  {
    EZ_REPORT_FAILURE("Failed to create script engine.");
    return;
  }

  EZ_ASSERT_ALWAYS(m_pScriptEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL) >= 0, "Failed to set message callback.");

  m_pScriptEngine->SetEngineProperty(asEP_STRING_ENCODING, 0);

  m_pStringFactory = EZ_DEFAULT_NEW(ezStringFactory);

  RegisterString(m_pScriptEngine);
  RegisterArray(m_pScriptEngine);
  RegisterCore(m_pScriptEngine).IgnoreResult();

  m_Initialized = true;
}

void ezAngelscriptManager::Shutdown()
{
  if (!m_Initialized)
    return;

  // Only delete the string factory if the stringCache is empty
  // If it is not empty, it means that someone might still attempt
  // to release string constants, so if we delete the string factory
  // the application might crash. Not deleting the cache would
  // lead to a memory leak, but since this is only happens when the
  // application is shutting down anyway, it is not important.
  if (m_pStringFactory->m_StringCache.GetCount() == 0)
  {
    EZ_DEFAULT_DELETE(m_pStringFactory);
    m_pStringFactory = nullptr;
  }

  m_pScriptEngine->ShutDownAndRelease();

  m_Initialized = false;
}

void ezAngelscriptManager::Test()
{
  ezInt32 result = m_pScriptBuilder->StartNewModule(m_pScriptEngine, "test");
  if (result < 0)
  {
    ezLog::Error("Unrecoverable error while starting a new module.");
    return;
  }

  result = m_pScriptBuilder->AddSectionFromFile("Scripts/test.as");
  if (result < 0)
  {
    ezLog::Error("Please correct the errors in the script and try again.");
    return;
  }

  result = m_pScriptBuilder->BuildModule();
  if (result < 0)
  {
    ezLog::Error("Please correct the errors in the script and try again.");
    return;
  }

  asIScriptModule* mod = m_pScriptEngine->GetModule("test");
  asIScriptFunction* func = mod->GetFunctionByDecl("void main()");

  if (func == nullptr)
  {
    ezLog::Error("The script must have the function 'void main()'. Please add it and try again.");
    return;
  }

  asIScriptContext* ctx = m_pScriptEngine->CreateContext();
  ctx->Prepare(func);

  result = ctx->Execute();
  if (result != asEXECUTION_FINISHED)
  {
    if (result == asEXECUTION_EXCEPTION)
    {
      ezLog::Error("An exception '{}' occurred. Please correct the code and try again.", ctx->GetExceptionString());
    }
  }
  ctx->Release();
}

#endif
