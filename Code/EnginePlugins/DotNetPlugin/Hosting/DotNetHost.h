#include <DotNetPlugin/DotNetPluginDLL.h>
#include <Foundation/Configuration/Singleton.h>

// Provided by the AppHost NuGet package and installed as an SDK pack
#include <DotNetPlugin/Hosting/ThirdParty/nethost.h>

// Header files copied from https://github.com/dotnet/core-setup
#include <DotNetPlugin/Hosting/ThirdParty/coreclr_delegates.h>
#include <DotNetPlugin/Hosting/ThirdParty/hostfxr.h>

class EZ_DOTNETPLUGIN_DLL ezDotNetHost
{
  EZ_DECLARE_SINGLETON(ezDotNetHost);

public:

  struct HostConfig
  {
    const char* m_Assembly;
  };

public:
  ezDotNetHost();
  ~ezDotNetHost();

  void Startup();
  void Shutdown();

  void LoadAssembly(const char* path);

private:
  bool Initialize(const char* runtimeConfigPath);

  component_entry_point_fn LoadAssemblyInternal(const char* dll, const char* className, const char* classNamespace, const char* method);
  void* LoadNativeLibrary(const char* path);
  void CloseNativeLibrary(void* handle);
  void* GetNativeExport(void* handle, const char* name);
  bool LoadHostFxr();
  load_assembly_and_get_function_pointer_fn GetDotNetLoadAssembly(const char* configPath);

private:
  // Statics to hold hostfxr exports
  static hostfxr_initialize_for_runtime_config_fn InitFuncPtr;
  static hostfxr_get_runtime_delegate_fn GetDelegateFuncPtr;
  static hostfxr_close_fn CloseFuncPtr;

private:
  bool m_bInitialized;
  hostfxr_handle m_hHostFxr;
  hostfxr_handle m_hHostContext;
  load_assembly_and_get_function_pointer_fn m_LoadAssemblyAndGetFuncPtr;
};
