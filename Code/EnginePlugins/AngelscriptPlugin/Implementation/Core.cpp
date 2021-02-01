#include <AngelscriptPlugin/AngelscriptManager.h>

void TestPrint(const ezString& msg)
{
  ezLog::Debug(msg.GetData());
}

void ezAngelscriptManager::RegisterCore(asIScriptEngine* scriptEngine)
{
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterGlobalFunction("void TestPrint(const string &in)", asFUNCTION(TestPrint), asCALL_CDECL) >= 0, "Function registration failed.");
}
