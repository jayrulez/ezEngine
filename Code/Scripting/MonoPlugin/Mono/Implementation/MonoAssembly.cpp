#include <MonoPlugin/Mono/MonoAssembly.h>
#include <MonoPlugin/Mono/MonoClass.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <mono/metadata/assembly.h>

ezMonoAssembly::ezMonoAssembly(const char* path, const ezString& name)
  : m_Name(name)
  , m_Path(path)
  , m_pMonoAssembly(nullptr)
  , m_pMonoImage(nullptr)
  , m_bIsLoaded(false)
  , m_bIsDependency(false)
  , m_bHasCachedClasses(false)
{
}

ezMonoAssembly::ezMonoAssembly(MonoImage* image)
  : m_bIsLoaded(true)
  , m_bIsDependency(true)
  , m_pMonoImage(image)
  , m_bHasCachedClasses(false)
{
  m_pMonoAssembly = mono_image_get_assembly(image);
  if (!m_pMonoAssembly)
  {
    EZ_REPORT_FAILURE("Unable to get assembly from image.");
  }
}

ezMonoAssembly::~ezMonoAssembly()
{
  Unload();
}

ezMonoClass* ezMonoAssembly::GetClass(const ezString& classNamespace, const ezString& name) const
{
  return nullptr;
}

ezMonoClass* ezMonoAssembly::GetClass(MonoClass* monoClass) const
{
  return nullptr;
}

const ezDynamicArray<ezMonoClass*>& ezMonoAssembly::GetClasses() const
{
  // TODO: insert return statement here
}

void ezMonoAssembly::Invoke(const ezString& methodName)
{
}

ezMonoClass* ezMonoAssembly::GetClass(const ezString& classNamespace, const ezString& name, MonoClass* monoClass) const
{
  return nullptr;
}

void ezMonoAssembly::Load()
{
  if (m_bIsLoaded)
    return;

  ezFileReader assemblyStream;
  if (!assemblyStream.Open(m_Path).Succeeded())
  {
    ezLog::Error("Cannot load assembly at path '{0}' because the file doesn't exist.", m_Path);
    return;
  }

  ezUInt32 assemblySize = (ezUInt32)assemblyStream.GetFileSize();
  ezDynamicArray<char> assemblyData;
  assemblyData.SetCountUninitialized(assemblySize);
  assemblyStream.ReadBytes(assemblyData.GetData(), assemblyStream.GetFileSize());

  ezString imageName = m_Path;// todo: get file name from path

  MonoImageOpenStatus status = MONO_IMAGE_OK;
  MonoImage* image = mono_image_open_from_data_with_name(assemblyData.GetData(), assemblySize, true, &status, false, imageName.GetData());
  assemblyData.Clear();

  if (status != MONO_IMAGE_OK || image == nullptr)
  {
    ezLog::Error("Failed loading image data for assembly '{0}'", m_Path);
    return;
  }

  #if EZ_ENABLED(EZ_COMPILE_FOR_DEBUG)
  ezString mdbPath = m_Path;
  #endif

  m_pMonoAssembly = mono_assembly_load_from_full(image, imageName.GetData(), &status, false);
  if (status != MONO_IMAGE_OK || m_pMonoAssembly == nullptr)
  {
    ezLog::Error("Failed loading assembly '{0}'", m_Path);
    return;
  }

  m_pMonoImage = image;
  if (m_pMonoImage == nullptr)
  {
    EZ_REPORT_FAILURE("Cannot get script assembly image.");
    return;
  }

  m_bIsLoaded = true;
  m_bIsDependency = false;
}

void ezMonoAssembly::Unload()
{
  if (!m_bIsLoaded)
    return;

  m_bIsLoaded = false;
}

bool ezMonoAssembly::IsGenericClass(const ezString& name) const
{
  return false;
}



EZ_STATICLINK_FILE(MonoPlugin, MonoPlugin_Mono_Implementation_MonoAssembly);
