#pragma once
#include <RHI/Program/ProgramBase.h>
#include <RHI/Shader/Shader.h>
#include <set>
#include <vector>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXDevice;

class DXProgram : public ProgramBase
{
public:
    DXProgram(DXDevice& device, const std::vector<std::shared_ptr<Shader>>& shaders);
};
