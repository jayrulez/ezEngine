#include <AngelscriptPlugin/AngelscriptManager.h>
#include <Foundation/Strings/HashedString.h>

static void ConstructHashedString(ezHashedString* thisPtr)
{
  new (thisPtr) ezHashedString();
}

static void CopyConstructHashedString(const ezHashedString& other, ezHashedString* thisPtr)
{
  new (thisPtr) ezHashedString(other);
}

static void DestructHashedString(ezHashedString* thisPtr)
{
  thisPtr->~ezHashedString();
}

static void AssignHashedStringFromString(const ezString& str, ezHashedString* thisPtr)
{
  thisPtr->Assign(str);
}

ezResult RegisterHashedString(asIScriptEngine* scriptEngine)
{
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectType("HashedString", sizeof(ezHashedString), asOBJ_VALUE | asGetTypeTraits<ezString>()) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("HashedString", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructHashedString), asCALL_CDECL_OBJLAST) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("HashedString", asBEHAVE_CONSTRUCT, "void f(const HashedString &in)", asFUNCTION(CopyConstructHashedString), asCALL_CDECL_OBJLAST) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("HashedString", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructHashedString), asCALL_CDECL_OBJLAST) >= 0, "");

  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectMethod("HashedString", "void Assign(const string &in)", asFUNCTION(AssignHashedStringFromString), asCALL_CDECL_OBJLAST) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectMethod("HashedString", "string &GetString() const", asMETHOD(ezHashedString, GetString), asCALL_THISCALL) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectMethod("HashedString", "bool Empty() const", asMETHOD(ezHashedString, IsEmpty), asCALL_THISCALL) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectMethod("HashedString", "uint64 GetHash() const", asMETHOD(ezHashedString, GetHash), asCALL_THISCALL) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectMethod("HashedString", "void Clear()", asMETHOD(ezHashedString, Clear), asCALL_THISCALL) >= 0, "");

  return EZ_SUCCESS;
}
