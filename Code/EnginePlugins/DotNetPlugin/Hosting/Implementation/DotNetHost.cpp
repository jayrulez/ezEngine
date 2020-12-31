#include <DotNetPlugin/Hosting/DotNetHost.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/OSFile.h>

// Standard headers
#include <stdint.h>

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
#  include <Foundation/Basics/Platform/Win/IncludeWindows.h>

#  define STR(s) L##s
#  define CH(c) L##c
#  define DIR_SEPARATOR L'\\'

#else
#  include <dlfcn.h>
#  include <limits.h>

#  define STR(s) s
#  define CH(c) c
#  define DIR_SEPARATOR '/'
#  define MAX_PATH PATH_MAX

#endif

hostfxr_initialize_for_runtime_config_fn ezDotNetHost::InitFuncPtr = {};
hostfxr_get_runtime_delegate_fn ezDotNetHost::GetDelegateFuncPtr = {};
hostfxr_close_fn ezDotNetHost::CloseFuncPtr = {};

EZ_IMPLEMENT_SINGLETON(ezDotNetHost);

ezDotNetHost::ezDotNetHost()
  : m_SingletonRegistrar(this)
{
  m_bInitialized = false;
  m_hHostFxr = nullptr;
  m_hHostContext = nullptr;
  m_LoadAssemblyAndGetFuncPtr = nullptr;
}

ezDotNetHost::~ezDotNetHost()
{
}

void ezDotNetHost::Startup()
{
  if (m_bInitialized)
    return;

  ezStringBuilder configPathSb = ezOSFile::GetApplicationDirectory();
  configPathSb.Append("ezEngineDotNet.runtimeconfig.json");

  m_bInitialized = Initialize(configPathSb.GetData());
}

void ezDotNetHost::Shutdown()
{
  if (!m_bInitialized)
    return;

  if (m_hHostContext)
  {
    CloseFuncPtr(m_hHostContext);
    m_hHostContext = nullptr;
  }

  if (m_hHostFxr)
  {
    CloseFuncPtr(m_hHostFxr);
    m_hHostFxr = nullptr;
  }

  m_bInitialized = false;
}

bool ezDotNetHost::Initialize(const char* runtimeConfigPath)
{
  // Step 1: Load HostFxr and get exported functions
  if (!LoadHostFxr())
  {
    EZ_REPORT_FAILURE("Failure: ezDotNetHost::LoadHostFxr()");
    return false;
  }

  // Step 2: Initialize and start .NET Core runtime
  m_LoadAssemblyAndGetFuncPtr = GetDotNetLoadAssembly(runtimeConfigPath);
  if (m_LoadAssemblyAndGetFuncPtr == nullptr)
  {
    EZ_REPORT_FAILURE("Failure: ezDotNetHost::GetDotNetLoadAssembly()");
    return false;
  }

  return true;
}

component_entry_point_fn ezDotNetHost::LoadAssemblyInternal(const char* dll, const char* className, const char* classNamespace, const char* method)
{
  component_entry_point_fn function = nullptr;

  ezStringBuilder dllPathSb = ezOSFile::GetApplicationDirectory();
  dllPathSb.Append(dll);

  ezStringBuilder dotnetTypeSb(className, ", ", classNamespace);

  int result = m_LoadAssemblyAndGetFuncPtr(ezStringWChar(dllPathSb.GetData()).GetData(), ezStringWChar(dotnetTypeSb.GetData()).GetData(), ezStringWChar(method).GetData(), nullptr, nullptr, (void**)&function);

  if (result != 0 || function == nullptr)
  {
    EZ_REPORT_FAILURE("Failure: m_LoadAssemblyAndGetFuncPtr()");
    return nullptr;
  }

  return function;
}

void* ezDotNetHost::LoadNativeLibrary(const char* path)
{
#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  HMODULE handle = ::LoadLibraryW(ezStringWChar(path).GetData());
  EZ_ASSERT_DEBUG(handle != nullptr, "Failed to load library: {}", path);
  return (void*)handle;
#else
  void* handle = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
  EZ_ASSERT_DEBUG(handle != nullptr, "Failed to load library: {}", path);
  return handle;
#endif
}

void ezDotNetHost::CloseNativeLibrary(void* handle)
{
#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  BOOL result = FreeLibrary(static_cast<HMODULE>(handle));
  EZ_ASSERT_DEBUG(result != false, "Failed to free native library.");
#else
  int result = dlclose(handle);
  EZ_ASSERT_DEBUG(result == 0, "Failed to free native library.");
#endif
}

void* ezDotNetHost::GetNativeExport(void* handle, const char* name)
{
#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  void* f = ::GetProcAddress((HMODULE)handle, name);
  EZ_ASSERT_DEBUG(f != nullptr, "Failed to get function pointer: {}", name);
  return f;
#else
  void* f = dlsym(h, name);
  EZ_ASSERT_DEBUG(f != nullptr, "Failed to get function pointer: {}", name);
  return f;
#endif
}

// Using the nethost library, discover the location of hostfxr and get exports
bool ezDotNetHost::LoadHostFxr()
{
  // Pre-allocate a large buffer for the path to hostfxr
  char_t buffer[MAX_PATH];
  size_t buffer_size = sizeof(buffer) / sizeof(char_t);
  int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
  if (rc != 0)
    return false;

  // Load hostfxr and get desired exports
  m_hHostFxr = LoadNativeLibrary(ezString(buffer));
  ezDotNetHost::InitFuncPtr = (hostfxr_initialize_for_runtime_config_fn)GetNativeExport(m_hHostFxr, "hostfxr_initialize_for_runtime_config");
  ezDotNetHost::GetDelegateFuncPtr = (hostfxr_get_runtime_delegate_fn)GetNativeExport(m_hHostFxr, "hostfxr_get_runtime_delegate");
  ezDotNetHost::CloseFuncPtr = (hostfxr_close_fn)GetNativeExport(m_hHostFxr, "hostfxr_close");

  return (InitFuncPtr && GetDelegateFuncPtr && CloseFuncPtr);
}

// Load and initialize .NET Core and get desired function pointer for scenario
load_assembly_and_get_function_pointer_fn ezDotNetHost::GetDotNetLoadAssembly(const char* configPath)
{
  // Load .NET Core
  void* loadAssemblyAndGetFuncPtr = nullptr;
  int rc = InitFuncPtr(ezStringWChar(configPath).GetData(), nullptr, &m_hHostContext);
  if (rc != 0 || m_hHostContext == nullptr)
  {
    ezLog::Error("Init failed: {}", rc); // todo: std::hex << std::showbase << rc
    CloseFuncPtr(m_hHostContext);
    return nullptr;
  }

  // Get the load assembly function pointer
  rc = GetDelegateFuncPtr(m_hHostContext, hdt_load_assembly_and_get_function_pointer, &loadAssemblyAndGetFuncPtr);
  if (rc != 0 || loadAssemblyAndGetFuncPtr == nullptr)
    ezLog::Error("Get delegate failed: {}", rc); // todo: std::hex << std::showbase << rc

  //CloseFuncPtr(m_hHostContext);
  return (load_assembly_and_get_function_pointer_fn)loadAssemblyAndGetFuncPtr;
}

void ezDotNetHost::LoadAssembly(const char* path)
{
  if (!m_bInitialized)
  {
    ezLog::Error("Cannot load assembly as ezDotNetHost is not initialized.");
    return;
  }
}


EZ_STATICLINK_FILE(DotNetPlugin, DotNetPlugin_Hosting_Implementation_DotNetHost);

