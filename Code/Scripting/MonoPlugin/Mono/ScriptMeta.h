//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include <MonoPlugin/Mono/Declarations.h>
#include <MonoPlugin/MonoPluginDLL.h>
#include <MonoPlugin/MonoPluginPCH.h>

/// \brief Contains information required for initializing and handling a single script class.
struct EZ_MONOPLUGIN_DLL ezScriptMeta
{
  EZ_DECLARE_POD_TYPE();
  ezScriptMeta();
  ezScriptMeta(const ezString& assembly, const ezString& classNamespace, const ezString& name, ezDelegate<void()> initCallback);

  ezString m_Namespace;
  ezString m_Name;
  ezString m_Assembly;

  ezDelegate<void()> m_InitCallback;
  ezMonoClass* m_ScriptClass;

  ezMonoField* m_NativeInstance;
};
