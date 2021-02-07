#include <DaoPluginPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/OSFile.h>
#include <Foundation/Types/UniquePtr.h>
#include <DaoPlugin/Runtime/DaoManager.h>

static ezUniquePtr<ezDaoManager> s_DaoManager;

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
EZ_BEGIN_SUBSYSTEM_DECLARATION(Dao, DaoPlugin)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    s_DaoManager = EZ_DEFAULT_NEW(ezDaoManager);
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    s_DaoManager->Shutdown();
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
    s_DaoManager->Startup();
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
  }

EZ_END_SUBSYSTEM_DECLARATION;
// clang-format on


EZ_STATICLINK_FILE(DaoPlugin, DaoPlugin_Startup);

