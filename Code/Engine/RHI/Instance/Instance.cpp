#include <RHIPCH.h>

#include <RHI/Instance/Instance.h>

struct CreatorFuncInfo
{
  InstanceFactory::CreatorFunc m_Func;
};

static ezMap<ApiType, CreatorFuncInfo> s_CreatorFuncs;

CreatorFuncInfo* GetCreatorFuncInfo(ApiType type)
{
  auto pFuncInfo = s_CreatorFuncs.GetValue(type);
  if (pFuncInfo == nullptr)
  {
    ezStringBuilder sPluginName = "ezRHI";
    if (type == ApiType::kDX12)
      sPluginName.Append("DX12");
    else
      sPluginName.Append("Vulkan");


    EZ_VERIFY(ezPlugin::LoadPlugin(sPluginName).Succeeded(), "Renderer plugin '{}' not found", sPluginName);

    pFuncInfo = s_CreatorFuncs.GetValue(type);
    EZ_ASSERT_DEV(pFuncInfo != nullptr, "Renderer '{}' is not registered", sPluginName.GetData());
  }

  return pFuncInfo;
}

std::shared_ptr<Instance> InstanceFactory::CreateInstance(ApiType type)
{
  if (auto pFuncInfo = GetCreatorFuncInfo(type))
  {
    return pFuncInfo->m_Func();
  }

  return nullptr;
}

void InstanceFactory::RegisterCreatorFunc(ApiType type, const CreatorFunc& func)
{
  CreatorFuncInfo funcInfo;
  funcInfo.m_Func = func;

  if (s_CreatorFuncs.Contains(type))
  {
    EZ_REPORT_FAILURE("Creator func already registered");
    return;
  }

  s_CreatorFuncs.Insert(type, funcInfo);
}

void InstanceFactory::UnregisterCreatorFunc(ApiType type)
{
  EZ_VERIFY(s_CreatorFuncs.Remove(type), "Creator func not registered");
}
