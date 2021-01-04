#pragma once

#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Containers/ArrayMap.h>
#include <MonoPlugin/MonoPluginDLL.h>

class EZ_MONOPLUGIN_DLL ezMonoManager
{
  EZ_DECLARE_SINGLETON(ezMonoManager);

public:
  ezMonoManager();
  ~ezMonoManager();
  void Startup(const ezArrayMap<ezString, ezString>& trustedPlatformAssemblies);
  void Shutdown();
  ezMonoAssembly* LoadAssembly(const ezString& path, const ezString& name);

private:
  bool m_bIsInitialized;
  MonoDomain* m_pRootDomain;
  ezArrayMap<ezString, ezMonoAssembly*> m_Assemblies;
};
