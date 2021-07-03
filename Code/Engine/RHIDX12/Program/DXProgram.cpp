#include <RHIDX12/Program/DXProgram.h>
#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/View/DXView.h>
#include <RHIDX12/BindingSet/DXBindingSet.h>
#include <deque>
#include <set>
#include <stdexcept>
#include <directx/d3dx12.h>

DXProgram::DXProgram(DXDevice& device, const std::vector<std::shared_ptr<Shader>>& shaders)
    : ProgramBase(shaders)
{
}
