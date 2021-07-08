#include <RHIShaderCompilerHLSLPCH.h>

#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/OSFile.h>
#include <Foundation/System/EnvironmentVariableUtils.h>
#include <RHIShaderCompilerHLSL/DXCLoader.h>
#include <Support/Global.h>
#include <filesystem>
#include <string>
#include <vector>
#include <wrl.h>

using namespace Microsoft::WRL;

HRESULT Test(dxc::DxcDllSupport& dll_support, ezRHIShaderBlobType target)
{
  std::string test_shader = "[shader(\"pixel\")]void main(){}";
  ComPtr<IDxcLibrary> library;
  IFR(dll_support.CreateInstance(CLSID_DxcLibrary, library.GetAddressOf()));
  ComPtr<IDxcBlobEncoding> source;
  IFR(library->CreateBlobWithEncodingFromPinned(test_shader.data(), (ezUInt32)test_shader.size(), CP_ACP, &source));

  std::vector<LPCWSTR> args;
  if (target == ezRHIShaderBlobType::SPIRV)
  {
    args.emplace_back(L"-spirv");
  }

  ComPtr<IDxcOperationResult> result;
  ComPtr<IDxcCompiler> compiler;
  IFR(dll_support.CreateInstance(CLSID_DxcCompiler, compiler.GetAddressOf()));
  IFR(compiler->Compile(
    source.Get(),
    L"main.hlsl",
    L"",
    L"lib_6_3",
    args.data(),
    (ezUInt32)args.size(),
    nullptr, 0,
    nullptr,
    &result));

  HRESULT hr = {};
  result->GetStatus(&hr);
  return hr;
}

std::unique_ptr<dxc::DxcDllSupport> Load(const std::string& path, ezRHIShaderBlobType target)
{
  auto dxcompiler_path = std::filesystem::u8path(path) / "dxcompiler.dll";
  if (!std::filesystem::exists(dxcompiler_path))
  {
    return {};
  }

  auto dxil_path = std::filesystem::u8path(path) / "dxil.dll";
  if (target == ezRHIShaderBlobType::DXIL && !std::filesystem::exists(dxil_path))
  {
    return {};
  }

  std::vector<wchar_t> prev_dll_dir(GetDllDirectoryW(0, nullptr));
  GetDllDirectoryW(static_cast<DWORD>(prev_dll_dir.size()), prev_dll_dir.data());

  EZ_SCOPE_EXIT(SetDllDirectoryW(prev_dll_dir.data()));

  SetDllDirectoryW(std::filesystem::u8path(path).wstring().c_str());

  auto dll_support = std::make_unique<dxc::DxcDllSupport>();
  if (FAILED(dll_support->InitializeForDll(dxcompiler_path.wstring().c_str(), "DxcCreateInstance")))
  {
    return {};
  }
  if (FAILED(Test(*dll_support, target)))
  {
    return {};
  }

  return dll_support;
}

std::unique_ptr<dxc::DxcDllSupport> GetDxcSupportImpl(ezRHIShaderBlobType target)
{
  ezStringBuilder vkBinDir(ezEnvironmentVariableUtils::GetValueString("VULKAN_SDK"), "/Bin");
  vkBinDir.MakeCleanPath();

  std::vector<std::string> localions = {
    ezOSFile::GetCurrentWorkingDirectory().GetData(),
    DXC_CUSTOM_LOCATION,
    DXC_DEFAULT_LOCATION,
    vkBinDir.GetData()};
  for (const auto& path : localions)
  {
    auto res = Load(path, target);
    if (res)
    {
      return res;
    }
  }
  EZ_ASSERT_NOT_IMPLEMENTED(false);
  return {};
}

dxc::DxcDllSupport& GetDxcSupport(ezRHIShaderBlobType target)
{
  static ezMap<ezRHIShaderBlobType, std::unique_ptr<dxc::DxcDllSupport>> cache;
  auto it = cache.Find(target);
  if (it == end(cache))
  {
    it = cache.Insert(target, GetDxcSupportImpl(target));
  }
  return *it.Value();
}
