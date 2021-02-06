#include <AngelscriptPlugin/AngelscriptManager.h>
#include <Foundation/Containers/ArrayMap.h>

static void ConstructString(ezString* thisPointer)
{
  new (thisPointer) ezString();
}

static void CopyConstructString(const ezString& other, ezString* thisPointer)
{
  new (thisPointer) ezString(other);
}

static void DestructString(ezString* thisPointer)
{
  thisPointer->~ezString();
}

static ezString& CopyAssignString(const ezString& source, ezString& dest)
{
  dest = source;
  return dest;
}

void ezAngelscriptManager::RegisterString(asIScriptEngine* scriptEngine)
{
  // register the string type
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectType("string", sizeof(ezString), asOBJ_VALUE | asGetTypeTraits<ezString>()) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterStringFactory("string", ezAngelscriptManager::GetSingleton()->GetStringFactory()) >= 0, "");

  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructString), asCALL_CDECL_OBJLAST) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT, "void f(const string &in)", asFUNCTION(CopyConstructString), asCALL_CDECL_OBJLAST) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructString), asCALL_CDECL_OBJLAST) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asFUNCTION(CopyAssignString), asCALL_CDECL_OBJLAST) >= 0, "");

  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectMethod("string", "uint Length() const", asMETHOD(ezString, GetCharacterCount), asCALL_THISCALL) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectMethod("string", "bool Empty() const", asMETHOD(ezString, IsEmpty), asCALL_THISCALL) >= 0, "");
}
