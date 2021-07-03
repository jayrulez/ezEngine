#pragma once
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Resource/Resource.h>
#ifdef NULL
#  undef NULL
#endif
#define NULL 0
#include <Support/WinIncludes.h>
#include <Support/dxcapi.use.h>

dxc::DxcDllSupport& GetDxcSupport(ShaderBlobType type);
