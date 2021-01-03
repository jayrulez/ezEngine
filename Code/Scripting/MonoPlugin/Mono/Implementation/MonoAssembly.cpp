#include <MonoPlugin/Mono/MonoAssembly.h>
#include <mono/metadata/assembly.h>

ezMonoAssembly::ezMonoAssembly(const char* path, const ezString& name)
  : m_Path(path)
  , m_Name(name)
  , m_bIsLoaded(false)
  , m_bIsDependency(false)
  , m_pAssembly(nullptr)
  , m_pImage(nullptr)
{
}

ezMonoAssembly::ezMonoAssembly(MonoImage* image)
  : m_bIsLoaded(true)
  , m_bIsDependency(true)
  , m_pImage(image)
{
  m_pAssembly = mono_image_get_assembly(image);
  if (!m_pAssembly)
  {
    EZ_REPORT_FAILURE("Unable to get assembly from image.");
  }
}

ezMonoAssembly::~ezMonoAssembly()
{
  Unload();
}

void ezMonoAssembly::Load()
{
  m_bIsLoaded = true;
}

void ezMonoAssembly::Unload()
{
  if (!m_bIsLoaded)
    return;

  m_bIsLoaded = false;
}



EZ_STATICLINK_FILE(MonoPlugin, MonoPlugin_Mono_Implementation_MonoAssembly);
