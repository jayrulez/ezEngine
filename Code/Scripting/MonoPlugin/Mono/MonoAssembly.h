#pragma once

#include <MonoPlugin/MonoPluginDLL.h>

class EZ_MONOPLUGIN_DLL ezMonoAssembly
{
private:
  friend class ezMonoManager;
  friend class ezMemoryUtils;

  ezMonoAssembly(const char* path, const ezString& name);
  ezMonoAssembly(MonoImage* image);
  ~ezMonoAssembly();
  void Load();
  void Unload();

private:
  const char* m_Path;
  ezString m_Name;
  bool m_bIsLoaded;
  bool m_bIsDependency;

  MonoAssembly* m_pAssembly;
  MonoImage* m_pImage;
};
