#include <AngelscriptPlugin/AngelscriptManager.h>
ezResult RegisterLog(asIScriptEngine* scriptEngine);
ezResult RegisterMath(asIScriptEngine* scriptEngine);
ezResult RegisterWorld(asIScriptEngine* scriptEngine);
ezResult RegisterGameObject(asIScriptEngine* scriptEngine);
ezResult RegisterPhysics(asIScriptEngine* scriptEngine);

//void TestPrint(const ezString& msg)
//{
//  ezLog::Debug(msg.GetData());
//}

ezResult ezAngelscriptManager::RegisterCore(asIScriptEngine* scriptEngine)
{
  EZ_SUCCEED_OR_RETURN(RegisterLog(scriptEngine));
  //EZ_SUCCEED_OR_RETURN(RegisterMath(scriptEngine));
  EZ_SUCCEED_OR_RETURN(RegisterWorld(scriptEngine));
  //EZ_SUCCEED_OR_RETURN(RegisterGameObject(scriptEngine));
  //EZ_SUCCEED_OR_RETURN(RegisterPhysics(scriptEngine));
  //EZ_ASSERT_ALWAYS(scriptEngine->RegisterGlobalFunction("void TestPrint(const string &in)", asFUNCTION(TestPrint), asCALL_CDECL) >= 0, "Function registration failed.");

  return EZ_SUCCESS;
}
