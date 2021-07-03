#include <RHIPCH.h>

#include <RHI/Renderer/Device/DeviceFactory.h>

struct CreatorFuncInfo
{
  ezRHIDeviceFactory::CreatorFunc m_Func;
  ezString m_sShaderModel;
  ezString m_sShaderCompiler;
};

static ezHashTable<ezString, CreatorFuncInfo> s_CreatorFuncs;

CreatorFuncInfo* GetCreatorFuncInfo(const char* szRendererName)
{
  auto pFuncInfo = s_CreatorFuncs.GetValue(szRendererName);
  if (pFuncInfo == nullptr)
  {
    ezStringBuilder sPluginName = "ezRenderer";
    sPluginName.Append(szRendererName);

    EZ_VERIFY(ezPlugin::LoadPlugin(sPluginName).Succeeded(), "Renderer plugin '{}' not found", sPluginName);

    pFuncInfo = s_CreatorFuncs.GetValue(szRendererName);
    EZ_ASSERT_DEV(pFuncInfo != nullptr, "Renderer '{}' is not registered", szRendererName);
  }

  return pFuncInfo;
}

ezInternal::NewInstance<ezRHIDevice> ezRHIDeviceFactory::CreateDevice(const char* szRendererName, ezAllocatorBase* pAllocator, const ezRHIDeviceCreationDescription& desc)
{
  if (auto pFuncInfo = GetCreatorFuncInfo(szRendererName))
  {
    return pFuncInfo->m_Func(pAllocator, desc);
  }

  return ezInternal::NewInstance<ezRHIDevice>(nullptr, pAllocator);
}

void ezRHIDeviceFactory::GetShaderModelAndCompiler(const char* szRendererName, const char*& szShaderModel, const char*& szShaderCompiler)
{
  if (auto pFuncInfo = GetCreatorFuncInfo(szRendererName))
  {
    szShaderModel = pFuncInfo->m_sShaderModel;
    szShaderCompiler = pFuncInfo->m_sShaderCompiler;
  }
}

void ezRHIDeviceFactory::RegisterCreatorFunc(const char* szRendererName, const CreatorFunc& func, const char* szShaderModel, const char* szShaderCompiler)
{
  CreatorFuncInfo funcInfo;
  funcInfo.m_Func = func;
  funcInfo.m_sShaderModel = szShaderModel;
  funcInfo.m_sShaderCompiler = szShaderCompiler;

  EZ_VERIFY(s_CreatorFuncs.Insert(szRendererName, funcInfo) == false, "Creator func already registered");
}

void ezRHIDeviceFactory::UnregisterCreatorFunc(const char* szRendererName)
{
  EZ_VERIFY(s_CreatorFuncs.Remove(szRendererName), "Creator func not registered");
}
