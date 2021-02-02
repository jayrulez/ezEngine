#include <AngelscriptPlugin/AngelscriptManager.h>
#include <Core/World/World.h>

static void ConstructWorldDesc(const ezString& name, ezWorldDesc* thisPtr)
{
  new (thisPtr) ezWorldDesc(name.GetData());
}

static void DestructWorldDesc(ezWorldDesc* thisPtr)
{
  thisPtr->~ezWorldDesc();
}

static void ConstructWorld(ezWorldDesc& desc, ezWorld* thisPtr)
{
  new (thisPtr) ezWorld(desc);
}

static void DestructWorld(ezWorld* thisPtr)
{
  thisPtr->~ezWorld();
}

ezResult RegisterWorld(asIScriptEngine* scriptEngine)
{
  scriptEngine->RegisterObjectType("WorldDesc", sizeof(ezWorldDesc), asOBJ_VALUE | asOBJ_POD);
  scriptEngine->RegisterObjectBehaviour("WorldDesc", asBEHAVE_CONSTRUCT, "void f(const string &in)", asFUNCTION(ConstructWorldDesc), asCALL_CDECL_OBJLAST);
  scriptEngine->RegisterObjectBehaviour("WorldDesc", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructWorldDesc), asCALL_CDECL_OBJLAST);


  scriptEngine->RegisterObjectType("World", sizeof(ezWorld), asOBJ_VALUE | asGetTypeTraits<ezWorld>());
  scriptEngine->RegisterObjectBehaviour("World", asBEHAVE_CONSTRUCT, "void f(WorldDesc &in)", asFUNCTION(ConstructWorld), asCALL_CDECL_OBJLAST);
  scriptEngine->RegisterObjectBehaviour("World", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructWorld), asCALL_CDECL_OBJLAST);

  return EZ_SUCCESS;
}
