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
}

ezDotNetHost::~ezDotNetHost()
{
}

void ezDotNetHost::Startup()
{
  if (m_bInitialized)
    return;

  AssemblyConfig config{"ezEngineDotNet", "ezEngineDotNet.Main"};

  m_bInitialized = Initialize(config);
}

void ezDotNetHost::Shutdown()
{
  if (!m_bInitialized)
    return;

  m_bInitialized = false;
}

bool ezDotNetHost::Initialize(const AssemblyConfig& config)
{
  // Step 1: Load HostFxr and get exported functions
  if (!LoadHostFxr())
  {
    EZ_REPORT_FAILURE("Failure: ezDotNetHost::LoadHostFxr()");
    return false;
  }

  // Step 2: Initialize and start .NET Core runtime
  ezStringBuilder configPathSb = ezOSFile::GetApplicationDirectory();
  configPathSb.AppendFormat("{}.runtimeconfig.json", config.m_Assembly);

  load_assembly_and_get_function_pointer_fn loadAssemblyAndGetFunctionPointer = nullptr;
  loadAssemblyAndGetFunctionPointer = GetDotNetLoadAssembly(configPathSb.GetData());
  if (loadAssemblyAndGetFunctionPointer == nullptr)
  {
    EZ_REPORT_FAILURE("Failure: ezDotNetHost::GetDotNetLoadAssembly()");
    return false;
  }

  return true;
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
  void* lib = LoadNativeLibrary(ezString(buffer));
  ezDotNetHost::InitFuncPtr = (hostfxr_initialize_for_runtime_config_fn)GetNativeExport(lib, "hostfxr_initialize_for_runtime_config");
  ezDotNetHost::GetDelegateFuncPtr = (hostfxr_get_runtime_delegate_fn)GetNativeExport(lib, "hostfxr_get_runtime_delegate");
  ezDotNetHost::CloseFuncPtr = (hostfxr_close_fn)GetNativeExport(lib, "hostfxr_close");

  return (InitFuncPtr && GetDelegateFuncPtr && CloseFuncPtr);
}

// Load and initialize .NET Core and get desired function pointer for scenario
load_assembly_and_get_function_pointer_fn ezDotNetHost::GetDotNetLoadAssembly(const char* configPath)
{
  // Load .NET Core
  void* load_assembly_and_get_function_pointer = nullptr;
  hostfxr_handle cxt = nullptr;
  int rc = InitFuncPtr(ezStringWChar(configPath).GetData(), nullptr, &cxt);
  if (rc != 0 || cxt == nullptr)
  {
    ezLog::Error("Init failed: {}", rc); // todo: std::hex << std::showbase << rc
    CloseFuncPtr(cxt);
    return nullptr;
  }

  // Get the load assembly function pointer
  rc = GetDelegateFuncPtr(cxt, hdt_load_assembly_and_get_function_pointer, &load_assembly_and_get_function_pointer);
  if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
    ezLog::Error("Get delegate failed: {}", rc); // todo: std::hex << std::showbase << rc

  CloseFuncPtr(cxt);
  return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
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

