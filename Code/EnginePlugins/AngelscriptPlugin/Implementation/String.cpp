#include <AngelscriptPlugin/AngelscriptManager.h>
#include <Foundation/Containers/ArrayMap.h>

static void ConstructString(ezString* thisPointer)
{
  new (thisPointer) ezString();
}

static void CopyConstructString(const ezString& other, ezString* thisPointer)
{
  const char* str = reinterpret_cast<const char*>(&other);

  new (thisPointer) ezString(str);
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
  ezInt32 result = 0;


  // register the string type
  result = scriptEngine->RegisterObjectType("string", sizeof(ezString), asOBJ_VALUE | asGetTypeTraits<ezString>());
  result = scriptEngine->RegisterStringFactory("string", ezAngelscriptManager::GetSingleton()->GetStringFactory());

  result = scriptEngine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructString), asCALL_CDECL_OBJLAST);
  result = scriptEngine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT, "void f(const string &in)", asFUNCTION(CopyConstructString), asCALL_CDECL_OBJLAST);
  result = scriptEngine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructString), asCALL_CDECL_OBJLAST);
  result = scriptEngine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asFUNCTION(CopyAssignString), asCALL_CDECL_OBJLAST);

  result = scriptEngine->RegisterObjectMethod("string", "uint length() const", asMETHOD(ezString, GetCharacterCount), asCALL_THISCALL);
  result = scriptEngine->RegisterObjectMethod("string", "bool empty() const", asMETHOD(ezString, IsEmpty), asCALL_THISCALL);
}
