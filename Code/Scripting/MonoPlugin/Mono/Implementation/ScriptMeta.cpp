//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include <MonoPlugin/Mono/ScriptMeta.h>

ezScriptMeta::ezScriptMeta()
  : m_ScriptClass(nullptr)
  , m_NativeInstance(nullptr)
{
}

ezScriptMeta::ezScriptMeta(const ezString& assembly, const ezString& classNamespace, const ezString& name, ezDelegate<void()> initCallback)
  : m_Assembly(assembly)
  , m_Namespace(classNamespace)
  , m_Name(name)
  , m_InitCallback(initCallback)
{
}
