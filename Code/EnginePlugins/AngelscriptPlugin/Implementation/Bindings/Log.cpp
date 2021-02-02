#include <AngelscriptPlugin/AngelscriptManager.h>

void LogDebug(const ezString& msg)
{
  ezLog::Debug(msg.GetData());
}
void LogInfo(const ezString& msg)
{
  ezLog::Info(msg.GetData());
}
void LogWarning(const ezString& msg)
{
  ezLog::Warning(msg.GetData());
}
void LogSeriousWarning(const ezString& msg)
{
  ezLog::SeriousWarning(msg.GetData());
}
void LogError(const ezString& msg)
{
  ezLog::Error(msg.GetData());
}
void LogSuccess(const ezString& msg)
{
  ezLog::Success(msg.GetData());
}

ezResult RegisterLog(asIScriptEngine* scriptEngine)
{
  scriptEngine->SetDefaultNamespace("Log");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterGlobalFunction("void Debug(const string &in)", asFUNCTION(LogDebug), asCALL_CDECL)>=0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterGlobalFunction("void Info(const string &in)", asFUNCTION(LogInfo), asCALL_CDECL) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterGlobalFunction("void Warning(const string &in)", asFUNCTION(LogWarning), asCALL_CDECL) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterGlobalFunction("void SeriousWarning(const string &in)", asFUNCTION(LogSeriousWarning), asCALL_CDECL) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterGlobalFunction("void Error(const string &in)", asFUNCTION(LogError), asCALL_CDECL) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterGlobalFunction("void Success(const string &in)", asFUNCTION(LogSuccess), asCALL_CDECL) >= 0, "");
  scriptEngine->SetDefaultNamespace("");

  return EZ_SUCCESS;
}
