#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/Program/Program.h>
#include <set>
#include <map>
#include <vector>

class EZ_RHI_DLL ProgramBase : public Program
{
public:
    ProgramBase(const std::vector<ezSharedPtr<Shader>>& shaders);

    bool HasShader(ShaderType type) const override final;
    ezSharedPtr<Shader> GetShader(ShaderType type) const override final;
    const std::vector<ezSharedPtr<Shader>>& GetShaders() const override final;
    const std::vector<BindKey>& GetBindings() const override final;
    const std::vector<EntryPoint>& GetEntryPoints() const override final;

protected:
    std::map<ShaderType, ezSharedPtr<Shader>> m_shaders_by_type;
    std::vector<ezSharedPtr<Shader>> m_shaders;
    std::vector<BindKey> m_bindings;
    std::vector<EntryPoint> m_entry_points;
};
