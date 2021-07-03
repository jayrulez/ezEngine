#pragma once

#include <Foundation/Types/Delegate.h>
#include <RHI/Renderer/Basics.h>

struct EZ_RHI_DLL ezRHIDeviceFactory
{
  using CreatorFunc = ezDelegate<ezInternal::NewInstance<ezRHIDevice>(ezAllocatorBase*, const ezRHIDeviceCreationDescription&)>;

  static ezInternal::NewInstance<ezRHIDevice> CreateDevice(const char* szRendererName, ezAllocatorBase* pAllocator, const ezRHIDeviceCreationDescription& desc);

  static void GetShaderModelAndCompiler(const char* szRendererName, const char*& szShaderModel, const char*& szShaderCompiler);

  static void RegisterCreatorFunc(const char* szRendererName, const CreatorFunc& func, const char* szShaderModel, const char* szShaderCompiler);
  static void UnregisterCreatorFunc(const char* szRendererName);
};
