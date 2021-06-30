#pragma once
#include <RHI/Instance/BaseTypes.h>
#include <dxc/Support/WinIncludes.h>
#include <dxc/Support/dxcapi.use.h>

dxc::DxcDllSupport& GetDxcSupport(ShaderBlobType type);
