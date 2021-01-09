//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Containers/ArrayMap.h>
#include <MonoPlugin/MonoPluginDLL.h>

class EZ_MONOPLUGIN_DLL ezMonoManager
{
  EZ_DECLARE_SINGLETON(ezMonoManager);

public:
  ezMonoManager(const ezArrayMap<ezString, ezString>& trustedPlatformAssemblies);
  ~ezMonoManager();

  void Shutdown();

  ezMonoAssembly* LoadAssembly(const ezString& path, const ezString& name);
  ezMonoAssembly* GetAssembly(const ezString& name) const;

  /// \brief Searches all loaded assemblies for the specified class.
  ezMonoClass* FindClass(const ezString& classNamespace, const ezString& typeName);

  /// \brief Searches all loaded assemblies for the specified class.
  ezMonoClass* FindClass(MonoClass* monoClass);

  /// \brief Returns the main (scripting) Mono domain.
  ///
  /// Mono no longer support multiple domains, return root domain
  MonoDomain* GetDomain() const { return m_pRootDomain; }

private:
  bool m_bIsInitialized;
  MonoDomain* m_pRootDomain;
  ezMonoAssembly* m_pCoreLibAssembly;
  ezArrayMap<ezString, ezMonoAssembly*> m_Assemblies;
};
