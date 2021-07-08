#include <RHIShaderCompilerHLSLPCH.h>

#include <RHIShaderCompilerHLSL/Compiler.h>
#include <RHIShaderCompilerHLSL/DXCLoader.h>

#include <d3dcompiler.h>
#include <deque>
#include <wrl.h>

EZ_DEFINE_AS_POD_TYPE(DxcDefine);

using namespace Microsoft::WRL;

static ezString GetShaderTarget(ezRHIShaderType type, const ezString& model)
{
  switch (type)
  {
    case ezRHIShaderType::Pixel:
      return ezStringBuilder("ps_", model);
    case ezRHIShaderType::Vertex:
      return ezStringBuilder("vs_", model);
    case ezRHIShaderType::Geometry:
      return ezStringBuilder("gs_", model);
    case ezRHIShaderType::Compute:
      return ezStringBuilder("cs_", model);
    case ezRHIShaderType::Amplification:
      return ezStringBuilder("as_", model);
    case ezRHIShaderType::Mesh:
      return ezStringBuilder("ms_", model);
    case ezRHIShaderType::Library:
      return ezStringBuilder("lib_", model);
    default:
      EZ_ASSERT_NOT_IMPLEMENTED;
      return "";
  }
}

class IncludeHandler : public IDxcIncludeHandler
{
public:
  IncludeHandler(ComPtr<IDxcLibrary> library, const std::wstring& base_path)
    : m_library(library)
    , m_base_path(base_path)
  {
  }

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject) override { return (HRESULT)E_NOTIMPL; }
  ULONG STDMETHODCALLTYPE AddRef() override { return (ULONG)E_NOTIMPL; }
  ULONG STDMETHODCALLTYPE Release() override { return (ULONG)E_NOTIMPL; }

  HRESULT STDMETHODCALLTYPE LoadSource(
    _In_ LPCWSTR pFilename,
    _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) override
  {
    std::wstring path = m_base_path + pFilename;
    ComPtr<IDxcBlobEncoding> source;
    HRESULT hr = m_library->CreateBlobFromFile(
      path.c_str(),
      nullptr,
      &source);
    if (SUCCEEDED(hr) && ppIncludeSource)
      *ppIncludeSource = source.Detach();
    return hr;
  }

private:
  ComPtr<IDxcLibrary> m_library;
  const std::wstring& m_base_path;
};

ezDynamicArray<ezUInt8> Compile(const ezString& shaderPath, const ezRHIShaderCreationDescription& desc, ezRHIShaderBlobType blobType)
{
  decltype(auto) dxc_support = GetDxcSupport(blobType);

  std::wstring shader_path = ezStringWChar(shaderPath).GetData();
  std::wstring shader_dir = shader_path.substr(0, shader_path.find_last_of(L"\\/") + 1);

  ComPtr<IDxcLibrary> library;
  dxc_support.CreateInstance(CLSID_DxcLibrary, library.GetAddressOf());
  ComPtr<IDxcBlobEncoding> source;

  EZ_ASSERT_ALWAYS(library->CreateBlobFromFile(
                     shader_path.c_str(),
                     nullptr,
                     &source) == S_OK,
    "");

  std::wstring target = ezStringWChar(GetShaderTarget(desc.m_Type, desc.m_Model)).GetData();
  std::wstring entrypoint = ezStringWChar(desc.m_EntryPoint).GetData();
  ezDynamicArray<std::pair<std::wstring, std::wstring>> definesStore;
  ezDynamicArray<DxcDefine> defines;
  for (const auto& define : desc.m_Defines)
  {
    definesStore.PushBack({ezStringWChar(define.Key()).GetData(), ezStringWChar(define.Value()).GetData()});
    defines.PushBack({definesStore.PeekBack().first.c_str(), definesStore.PeekBack().second.c_str()});
  }

  ezDynamicArray<LPCWSTR> arguments;
  ezDeque<std::wstring> dynamicArguments;
  arguments.PushBack(L"/Zi");
  arguments.PushBack(L"/Qembed_debug");
  ezUInt32 space = 0;
  if (blobType == ezRHIShaderBlobType::SPIRV)
  {
    arguments.PushBack(L"-spirv");
    arguments.PushBack(L"-fspv-target-env=vulkan1.2");
    arguments.PushBack(L"-fspv-extension=KHR");
    arguments.PushBack(L"-fspv-extension=SPV_NV_mesh_shader");
    arguments.PushBack(L"-fspv-extension=SPV_EXT_descriptor_indexing");
    arguments.PushBack(L"-fspv-extension=SPV_EXT_shader_viewport_index_layer");
    arguments.PushBack(L"-fspv-extension=SPV_GOOGLE_hlsl_functionality1");
    arguments.PushBack(L"-fspv-extension=SPV_GOOGLE_user_type");
    arguments.PushBack(L"-fvk-use-dx-layout");
    arguments.PushBack(L"-fspv-reflect");
    space = static_cast<ezUInt32>(desc.m_Type);
  }

  ezStringBuilder spaceString;
  spaceString.AppendFormat("{}", space);

  arguments.PushBack(L"-auto-binding-space");
  dynamicArguments.PushBack(ezStringWChar(spaceString).GetData());
  arguments.PushBack(dynamicArguments.PeekBack().c_str());

  ComPtr<IDxcOperationResult> result;
  IncludeHandler include_handler(library, shader_dir);
  ComPtr<IDxcCompiler> compiler;
  dxc_support.CreateInstance(CLSID_DxcCompiler, compiler.GetAddressOf());
  EZ_ASSERT_ALWAYS(compiler->Compile(
                     source.Get(),
                     L"main.hlsl",
                     entrypoint.c_str(),
                     target.c_str(),
                     arguments.GetData(), arguments.GetCount(),
                     defines.GetData(), defines.GetCount(),
                     &include_handler,
                     &result) == S_OK,
    "");

  HRESULT hr = {};
  result->GetStatus(&hr);
  ezDynamicArray<ezUInt8> blob;
  if (SUCCEEDED(hr))
  {
    ComPtr<IDxcBlob> dxcBlob;
    EZ_ASSERT_ALWAYS(result->GetResult(&dxcBlob) == S_OK, "");

    blob.SetCountUninitialized((ezUInt32)dxcBlob->GetBufferSize());

    ezMemoryUtils::Copy(blob.GetData(), (ezUInt8*)dxcBlob->GetBufferPointer(), blob.GetCount());

    //blob.assign((ezUInt8*)dxcBlob->GetBufferPointer(), (ezUInt8*)dxcBlob->GetBufferPointer() + dxcBlob->GetBufferSize());
  }
  else
  {
    ComPtr<IDxcBlobEncoding> errors;
    result->GetErrorBuffer(&errors);

    ezLog::Error(reinterpret_cast<char*>(errors->GetBufferPointer()));
  }
  return blob;
}
