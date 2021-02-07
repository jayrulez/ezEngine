#include <HashLinkPluginPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <Foundation/Types/UniquePtr.h>
#include <HashLinkPlugin/HashLinkManager.h>
#include <Foundation/IO/OSFile.h>
#include <Foundation/IO/FileSystem/FileSystem.h>

static ezUniquePtr<ezHashLinkManager> s_HashLinkManager;

static ezString GetAbsolutePath(const ezString& file)
{
  ezStringBuilder path;

  if (!ezPathUtils::IsAbsolutePath(file))
  {
    if (ezFileSystem::ResolveSpecialDirectory(">project", path).Succeeded())
    {
      path.Append("/", file.GetData());
      if (ezFileSystem::ExistsFile(path))
      {
        path.MakeCleanPath();
        return path;
      }
    }

    path = ezOSFile::GetApplicationDirectory();
    path.Append(file.GetData());

    // todo: verify that file exists
  }
  else
  {
    path = file;
  }

  path.MakeCleanPath();
  return path;
}

// clang-format off
EZ_BEGIN_SUBSYSTEM_DECLARATION(HashLink, HashLinkPlugin)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    s_HashLinkManager = EZ_DEFAULT_NEW(ezHashLinkManager);
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    s_HashLinkManager->Shutdown();
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
    s_HashLinkManager->Startup(GetAbsolutePath("NewProject.hl"));
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
  }

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on
