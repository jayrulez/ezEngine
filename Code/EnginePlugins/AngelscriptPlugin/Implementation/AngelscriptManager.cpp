#include <AngelscriptPlugin/AngelscriptManager.h>
#include <Core/Scripting/AngelscriptHelper.h>
#include <AngelscriptPlugin/AngelscriptBuilder.h>
#include <AngelscriptPlugin/StringFactory.h>

#ifdef BUILDSYSTEM_ENABLE_ANGELSCRIPT_SUPPORT

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
  , m_pStringFactory{nullptr}
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
  RegisterCore(m_pScriptEngine);

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
}

ezStringFactory* ezAngelscriptManager::GetStringFactory() const
{
  return m_pStringFactory;
}

#endif
