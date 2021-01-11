//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Containers/ArrayMap.h>
#include <MonoPlugin/Mono/Declarations.h>
#include <MonoPlugin/Mono/ScriptMeta.h>
#include <MonoPlugin/MonoPluginDLL.h>

/// \brief Loads Mono script assemblies and manages script objects.
class EZ_MONOPLUGIN_DLL ezMonoManager
{
  EZ_DECLARE_SINGLETON(ezMonoManager);

public:
  ezMonoManager();
  ~ezMonoManager();

  void Initialize(const ezArrayMap<ezString, ezString>& trustedPlatformAssemblies);

  /// \brief Unloads all assemblies and shuts down the runtime.
  /// Called automatically on module shut-down.
  void UnloadAll();

  /// \brief Loads a new assembly from the provided path.
  ///
  /// \param[in]	path				Absolute path to the assembly .dll.
  /// \param[in]	name				Unique name for the assembly.
  ezMonoAssembly* LoadAssembly(const ezString& path, const ezString& name);

  /// \brief Attempts to find a previously loaded assembly with the specified name.
  /// Returns null if assembly cannot be found.
  ezMonoAssembly* GetAssembly(const ezString& name) const;

  /// \brief Searches all loaded assemblies for the specified class.
  ezMonoClass* FindClass(const ezString& classNamespace, const ezString& typeName);

  /// \brief Searches all loaded assemblies for the specified class.
  ezMonoClass* FindClass(MonoClass* monoClass);

  /// \brief Returns the main (scripting) Mono domain.
  ///
  /// Mono no longer support multiple domains, return root domain
  MonoDomain* GetDomain() const { return m_pRootDomain; }

  /// \brief Unloads the active domain(in which all script assemblies are loaded) and destroys any
  /// managed objects* associated with it.
  void UnloadScriptDomain();

  /// \brief Registers a new script type.
  /// This should be done before any assembly loading is done. Upon assembly load these
  ///script types will be initialized with necessary information about their managed counterparts.* /
  void RegisterScriptType(ezScriptMeta* metaData, const ezScriptMeta& localMetaData);

private:
  /// \brief Initializes any script types registered with RegisterScriptType() for this assembly.
  /// This sets up any native <-> managed internal calls and other similar code for such types.
  void InitializeScriptTypes(ezMonoAssembly& assembly);

private:
  struct ScriptMetaInfo
  {
    ezScriptMeta* m_MetaData;
    ezScriptMeta m_LocalMetaData;
  };

private:
  ezMap<ezString, ezDynamicArray<ScriptMetaInfo>> m_ScriptMetaData;

  bool m_bIsInitialized;
  MonoDomain* m_pRootDomain;
  //MonoDomain* m_pScriptDomain;
  ezMonoAssembly* m_pCoreLibAssembly;
  ezArrayMap<ezString, ezMonoAssembly*> m_Assemblies;
};
