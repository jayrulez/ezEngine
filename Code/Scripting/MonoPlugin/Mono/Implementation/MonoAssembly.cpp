//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include <Foundation/IO/FileSystem/FileReader.h>
#include <MonoPlugin/Mono/MonoAssembly.h>
#include <MonoPlugin/Mono/MonoClass.h>
#include <MonoPlugin/Mono/MonoManager.h>
#include <MonoPlugin/Mono/MonoUtil.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/tokentype.h>


bool ezMonoAssembly::ClassId::operator==(const ClassId& other) const
{
  return Name.IsEqual(other.Name) && Namespace == other.Namespace && GenericInstance == other.GenericInstance;
}

bool ezMonoAssembly::ClassId::operator!=(const ClassId& other) const
{
  return !(*this == other);
}

bool ezMonoAssembly::ClassId::operator<(const ClassId& other) const
{
  return GetHashCode() < other.GetHashCode();
}

ezUInt32 ezMonoAssembly::ClassId::GetHashCode() const
{
  ezUInt32 hash = 0;

  hash = ezHashingUtils::xxHash32String(Namespace, hash);
  hash = ezHashingUtils::xxHash32String(Name, hash);
  if (GenericInstance)
    hash = ezHashingUtils::xxHash32(GenericInstance, sizeof(GenericInstance), hash);

  return hash;
}

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

ezMonoAssembly::~ezMonoAssembly()
{
  Unload();
}

void ezMonoAssembly::Load()
{
  if (m_bIsLoaded)
    Unload();

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
  assemblyStream.Close();

  ezString imageName = ezPathUtils::GetFileName(m_Path);

  MonoImageOpenStatus status = MONO_IMAGE_OK;
  MonoImage* image = mono_image_open_from_data_with_name(assemblyData.GetData(), assemblySize, true, &status, false, imageName.GetData());
  assemblyData.Clear();

  if (status != MONO_IMAGE_OK || image == nullptr)
  {
    ezLog::Error("Failed loading image data for assembly '{0}'", m_Path);
    return;
  }

#if EZ_ENABLED(EZ_COMPILE_FOR_DEBUG)
  ezStringBuilder pdbPath = m_Path;
  pdbPath.ReplaceLast(".dll", ".pdb");

  if (ezOSFile::ExistsFile(pdbPath))
  {
    ezFileReader pdbStream;
    if (pdbStream.Open(m_Path).Succeeded())
    {
      ezUInt32 pdbSize = (ezUInt32)pdbStream.GetFileSize();
      m_DebugData.SetCountUninitialized(pdbSize);
      pdbStream.ReadBytes(m_DebugData.GetData(), pdbStream.GetFileSize());
      mono_debug_open_image_from_memory(image, m_DebugData.GetData(), pdbSize);
      pdbStream.Close();
    }
  }
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

void ezMonoAssembly::LoadFromImage(MonoImage* image)
{
  MonoAssembly* monoAssembly = mono_image_get_assembly(image);
  if (!monoAssembly)
  {
    EZ_REPORT_FAILURE("Unable to get assembly from image.");
    return;
  }

  m_pMonoAssembly = monoAssembly;
  m_pMonoImage = image;

  m_bIsLoaded = true;
  m_bIsDependency = true;
}

void ezMonoAssembly::Unload()
{
  if (!m_bIsLoaded)
    return;

  for (auto& entry : m_ClassesByMonoClass)
  {
    EZ_DEFAULT_DELETE(entry.value);
  }

  m_Classes.Clear();
  m_ClassesByMonoClass.Clear();
  m_CachedClasses.Clear();
  m_bHasCachedClasses = false;

  if (!m_bIsDependency)
  {
    if (m_DebugData.GetCount() > 0)
    {
      mono_debug_close_image(m_pMonoImage);
      m_DebugData.Clear();
    }
    if (m_pMonoImage != nullptr)
    {
      // Make sure to close the image, otherwise when we try to re-load this assembly the Mono will return the cached image
      mono_image_close(m_pMonoImage);
      m_pMonoImage = nullptr;
    }
    m_pMonoAssembly = nullptr;
    m_bIsLoaded = false;
  }
}

void ezMonoAssembly::Invoke(const ezString& methodName)
{
  MonoMethodDesc* methodDesc = mono_method_desc_new(methodName.GetData(), false);

  if (methodDesc != nullptr)
  {
    MonoMethod* entry = mono_method_desc_search_in_image(methodDesc, m_pMonoImage);

    if (entry != nullptr)
    {
      MonoObject* exception = nullptr;
      mono_runtime_invoke(entry, nullptr, nullptr, &exception);

      ezMonoUtil::ThrowIfException(exception);
    }

    mono_method_desc_free(methodDesc);
  }
}

ezMonoClass* ezMonoAssembly::GetClass(const ezString& classNamespace, const ezString& name) const
{
  if (!m_bIsLoaded)
  {
    EZ_REPORT_FAILURE("Trying to use an unloaded assembly.");
    return nullptr;
  }

  ezMonoAssembly::ClassId classId(classNamespace, name);
  ezUInt32 classIndex = m_Classes.Find(classId);

  if (classIndex != ezInvalidIndex)
    return m_Classes.GetValue(classIndex);

  MonoClass* monoClass = mono_class_from_name(m_pMonoImage, classNamespace.GetData(), name.GetData());
  if (monoClass == nullptr)
    return nullptr;

  ezMonoClass* newClass = EZ_DEFAULT_NEW(ezMonoClass, classNamespace, name, monoClass, this);
  m_Classes[classId] = newClass;
  m_ClassesByMonoClass[monoClass] = newClass;

  return newClass;
}

ezMonoClass* ezMonoAssembly::GetClass(MonoClass* monoClass) const
{
  if (!m_bIsLoaded)
  {
    EZ_REPORT_FAILURE("Trying to use an unloaded assembly.");
    return nullptr;
  }

  if (monoClass == nullptr)
    return nullptr;

  ezUInt32 classIndex = m_ClassesByMonoClass.Find(monoClass);
  if (classIndex != ezInvalidIndex)
    return m_ClassesByMonoClass.GetValue(classIndex);

  ezString classNamespace;
  ezString typeName;
  ezMonoUtil::GetClassName(monoClass, classNamespace, typeName);

  // Verify the class is actually part of this assembly
  MonoImage* classImage = mono_class_get_image(monoClass);
  if (classImage != m_pMonoImage)
    return nullptr;

  ezMonoClass* newClass = EZ_DEFAULT_NEW(ezMonoClass, classNamespace, typeName, monoClass, this);
  m_ClassesByMonoClass[monoClass] = newClass;

  ezMonoAssembly::ClassId classId(classNamespace, typeName);
  m_Classes[classId] = newClass;

  return newClass;
}

ezMonoClass* ezMonoAssembly::GetClass(const ezString& classNamespace, const ezString& name, MonoClass* monoClass) const
{
  if (!m_bIsLoaded)
  {
    EZ_REPORT_FAILURE("Trying to use an unloaded assembly.");
    return nullptr;
  }

  if (monoClass == nullptr)
    return nullptr;

  ezUInt32 classIndex = m_ClassesByMonoClass.Find(monoClass);
  if (classIndex != ezInvalidIndex)
    return m_ClassesByMonoClass.GetValue(classIndex);

  ezMonoClass* newClass = EZ_DEFAULT_NEW(ezMonoClass, classNamespace, name, monoClass, this);
  m_ClassesByMonoClass[monoClass] = newClass;

  if (!IsGenericClass(name)) // No point in referencing generic types by name as all instances share it
  {
    ezMonoAssembly::ClassId classId(classNamespace, name);
    m_Classes[classId] = newClass;
  }

  return newClass;
}

const ezDynamicArray<ezMonoClass*>& ezMonoAssembly::GetAllClasses() const
{
  if (m_bHasCachedClasses)
    return m_CachedClasses;

  m_CachedClasses.Clear();
  ezDeque<ezMonoClass*> todo;

  ezMonoAssembly* coreLib = ezMonoManager::GetSingleton()->GetAssembly("System.Private.CoreLib");
  ezMonoClass* compilerGeneratedAttrib = coreLib->GetClass("System.Runtime.CompilerServices", "CompilerGeneratedAttribute");

  const MonoTableInfo* pTableInfo = mono_image_get_table_info(m_pMonoImage, MONO_TABLE_TYPEDEF);
  //ezInt32 numRows = mono_image_get_table_rows(m_pMonoImage, MONO_TABLE_TYPEDEF);
  ezInt32 numRows = mono_table_info_get_rows(pTableInfo);

  for (ezInt32 i = 1; i < numRows; i++) // Skip Module
  {
    MonoClass* monoClass = mono_class_get(m_pMonoImage, MONO_TOKEN_TYPE_DEF | (i + 1));

    ezString classNamespace;
    ezString type;
    ezMonoUtil::GetClassName(monoClass, classNamespace, type);

    ezMonoClass* curClass = GetClass(classNamespace, type);
    if (curClass != nullptr)
    {
      // Skip compiler generates classes
      if (curClass->HasAttribute(compilerGeneratedAttrib))
        continue;

      // Get nested types if it has any
      todo.PushBack(curClass);
      while (!todo.IsEmpty())
      {
        ezMonoClass* curNestedClass = todo.PeekBack();
        todo.PopBack();

        void* iter = nullptr;
        do
        {
          MonoClass* rawNestedClass = mono_class_get_nested_types(curNestedClass->GetMonoClass(), &iter);
          if (rawNestedClass == nullptr)
            break;

          ezString nestedType = ezStringBuilder(curNestedClass->GetTypeName(), "+", mono_class_get_name(rawNestedClass));

          ezMonoClass* nestedClass = GetClass(classNamespace, nestedType, rawNestedClass);
          if (nestedClass != nullptr)
          {
            // Skip compiler generated classes
            if (nestedClass->HasAttribute(compilerGeneratedAttrib))
              continue;

            m_CachedClasses.PushBack(nestedClass);
            todo.PushBack(nestedClass);
          }

        } while (true);
      }

      m_CachedClasses.PushBack(curClass);
    }
  }

  m_bHasCachedClasses = true;

  return m_CachedClasses;
}

bool ezMonoAssembly::IsGenericClass(const ezString& name) const
{
  // By CIL convention generic classes have ` separating their name and
  // number of generic parameters
  return name.FindSubString("`") != nullptr;
}

EZ_STATICLINK_FILE(MonoPlugin, MonoPlugin_Mono_Implementation_MonoAssembly);
