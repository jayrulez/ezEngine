#pragma once
#include <RHI/Basics.h>
#ifdef NULL
#  undef NULL
#endif
#define NULL 0
#include <Support/WinIncludes.h>
#include <Support/dxcapi.use.h>

dxc::DxcDllSupport& GetDxcSupport(ezRHIShaderBlobType type);
