#pragma once
#include <RHI/RHIDLL.h>
#include <RHI/Instance/QueryInterface.h>
#include <RHI/BindingSet/BindingSet.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Resource/Resource.h>
#include <RHI/Shader/Shader.h>
#include <memory>

class EZ_RHI_DLL Program : public QueryInterface
{
public:
    virtual ~Program() = default;
    virtual bool HasShader(ShaderType type) const = 0;
    virtual ezSharedPtr<Shader> GetShader(ShaderType type) const = 0;
    virtual const std::vector<ezSharedPtr<Shader>>& GetShaders() const = 0;
    virtual const std::vector<BindKey>& GetBindings() const = 0;
    virtual const std::vector<EntryPoint>& GetEntryPoints() const = 0;
};
