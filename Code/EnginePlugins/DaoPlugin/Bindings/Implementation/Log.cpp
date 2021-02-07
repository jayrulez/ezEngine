#include <DaoPluginPCH.h>

#include <Dao/kernel/dao.h>
#include <Dao/kernel/daoValue.h>

static void dao_LogDebug(DaoProcess* proc, DaoValue* param[], ezInt32 nparam)
{
  char* message = param[0]->xString.value->chars;
  ezLog::Debug(message);
}

#ifdef __cplusplus
extern "C"
{
#endif
  int DaoOnLoad(DaoVmSpace* vmSpace, DaoNamespace* ns)
  {
    DaoNamespace* logNamespace = DaoVmSpace_GetNamespace(vmSpace, "Log");
    DaoNamespace_AddConstValue(ns, "Log", (DaoValue*)logNamespace);

    DaoNamespace_WrapFunction(logNamespace, dao_LogDebug, "Debug(message: string)");
    return 0;
  }
#ifdef __cplusplus
}
#endif

EZ_STATICLINK_FILE(DaoPlugin, DaoPlugin_Bindings_Implementation_Log);
