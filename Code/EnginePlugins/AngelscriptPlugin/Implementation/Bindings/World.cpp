#include <AngelscriptPlugin/AngelscriptManager.h>
#include <Core/World/World.h>

static void ConstructGameObjectId(ezUInt64 instanceIndex, ezUInt8 generation, ezUInt8 worldIndex, ezGameObjectId* thisPtr)
{
  new (thisPtr) ezGameObjectId(instanceIndex, generation, worldIndex);
}

static void DestructGameObjectId(ezGameObjectId* thisPtr)
{
  thisPtr->~ezGameObjectId();
}

static void ConstructGameObjectHandle(ezGameObjectId id, ezGameObjectHandle* thisPtr)
{
  new (thisPtr) ezGameObjectHandle(id);
}

static void DestructGameObjectHandle(ezGameObjectHandle* thisPtr)
{
  thisPtr->~ezGameObjectHandle();
}

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
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectType("GameObjectId", sizeof(ezGameObjectId), asOBJ_VALUE | asOBJ_POD) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("GameObjectId", asBEHAVE_CONSTRUCT, "void f(uint64, uint8, uint8)", asFUNCTION(ConstructGameObjectId), asCALL_CDECL_OBJLAST) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("GameObjectId", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructGameObjectId), asCALL_CDECL_OBJLAST) >= 0, "");

  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectType("GameObjectHandle", sizeof(ezGameObjectHandle), asOBJ_VALUE | asOBJ_POD) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("GameObjectHandle", asBEHAVE_CONSTRUCT, "void f(GameObjectId &in)", asFUNCTION(ConstructGameObjectHandle), asCALL_CDECL_OBJLAST) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("GameObjectHandle", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructGameObjectHandle), asCALL_CDECL_OBJLAST) >= 0, "");

  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectType("GameObjectDesc", sizeof(ezGameObjectDesc), asOBJ_VALUE | asOBJ_POD) >= 0, "");
  //EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectProperty("GameObjectDesc", "bool ActiveFlag", asOFFSET(ezGameObjectDesc, m_bActiveFlag)) >= 0, "");
  //EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectProperty("GameObjectDesc", "bool Dynamic", asOFFSET(ezGameObjectDesc, m_bDynamic)) >= 0, "");
  //EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectProperty("GameObjectDesc", "uint16 TeamID", asOFFSET(ezGameObjectDesc, m_uiTeamID)) >= 0, "");
  //EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectProperty("GameObjectDesc", "HashedString Name", asOFFSET(ezGameObjectDesc, m_sName)) >= 0, "");
  //EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectProperty("GameObjectDesc", "GameObjectHandle Parent", asOFFSET(ezGameObjectDesc, m_hParent)) >= 0, "");

  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectType("WorldDesc", sizeof(ezWorldDesc), asOBJ_VALUE | asOBJ_POD) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("WorldDesc", asBEHAVE_CONSTRUCT, "void f(const string &in)", asFUNCTION(ConstructWorldDesc), asCALL_CDECL_OBJLAST) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("WorldDesc", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructWorldDesc), asCALL_CDECL_OBJLAST) >= 0, "");

  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectType("World", sizeof(ezWorld), asOBJ_VALUE | asGetTypeTraits<ezWorld>()) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("World", asBEHAVE_CONSTRUCT, "void f(WorldDesc &in)", asFUNCTION(ConstructWorld), asCALL_CDECL_OBJLAST) >= 0, "");
  EZ_ASSERT_ALWAYS(scriptEngine->RegisterObjectBehaviour("World", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructWorld), asCALL_CDECL_OBJLAST) >= 0, "");

  return EZ_SUCCESS;
}
