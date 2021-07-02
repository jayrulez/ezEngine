#pragma once
#include <cstddef>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <RHI/HighLevelRenderer/RenderDevice.h>
#include <RHI/HighLevelRenderer/BufferLayout.h>

template<ShaderType, typename T> class ShaderHolderImpl {};

template<typename T>
class ShaderHolderImpl<ShaderType::kPixel, T>
{
public:
    struct Api : public T
    {
        using T::T;
    } ps;

    T& GetApi()
    {
        return ps;
    }

    ShaderHolderImpl(RenderDevice& device, ShaderBlobType shaderBlobType)
      : ps(device, shaderBlobType)
    {
    }
};

template<typename T>
class ShaderHolderImpl<ShaderType::kVertex, T>
{
public:
    struct Api : public T
    {
        using T::T;
    } vs;

    T& GetApi()
    {
        return vs;
    }

    ShaderHolderImpl(RenderDevice& device, ShaderBlobType shaderBlobType)
      : vs(device, shaderBlobType)
    {
    }
};

template<typename T>
class ShaderHolderImpl<ShaderType::kGeometry, T>
{
public:
    struct Api : public T
    {
        using T::T;
    } gs;

    T& GetApi()
    {
        return gs;
    }

    ShaderHolderImpl(RenderDevice& device, ShaderBlobType shaderBlobType)
      : gs(device, shaderBlobType)
    {
    }
};

template<typename T>
class ShaderHolderImpl<ShaderType::kAmplification, T>
{
public:
    struct Api : public T
    {
        using T::T;
    } as;

    T& GetApi()
    {
        return as;
    }

    ShaderHolderImpl(RenderDevice& device, ShaderBlobType shaderBlobType)
      : as(device, shaderBlobType)
    {
    }
};

template<typename T>
class ShaderHolderImpl<ShaderType::kMesh, T>
{
public:
    struct Api : public T
    {
        using T::T;
    } ms;

    T& GetApi()
    {
        return ms;
    }

    ShaderHolderImpl(RenderDevice& device, ShaderBlobType shaderBlobType)
      : ms(device, shaderBlobType)
    {
    }
};

template<typename T>
class ShaderHolderImpl<ShaderType::kCompute, T>
{
public:
    struct Api : public T
    {
        using T::T;
    } cs;

    T& GetApi()
    {
        return cs;
    }

    ShaderHolderImpl(RenderDevice& device, ShaderBlobType shaderBlobType)
      : cs(device, shaderBlobType)
    {
    }
};

template<typename T>
class ShaderHolderImpl<ShaderType::kLibrary, T>
{
public:
    struct Api : public T
    {
        using T::T;
    } lib;

    T& GetApi()
    {
        return lib;
    }

    ShaderHolderImpl(RenderDevice& device, ShaderBlobType shaderBlobType)
      : lib(device, shaderBlobType)
    {
    }
};

template<ShaderType T, ShaderType... Ts>
constexpr bool contains()
{
    return ((T == Ts) || ...);
}

template<typename T> class ShaderHolder : public ShaderHolderImpl<T::type, T> { using ShaderHolderImpl<T::type, T>::ShaderHolderImpl; };

template<typename ... Args>
class ProgramHolder : public ShaderHolder<Args>...
{
public:
    ProgramHolder(RenderDevice& device, ShaderBlobType shaderBlobType)
    : ShaderHolder<Args>(device, shaderBlobType)...
        , m_device(device)
    {
        CompileShaders();
    }

    template<typename Setup>
    ProgramHolder(RenderDevice& device, ShaderBlobType shaderBlobType, const Setup& setup)
        : ShaderHolder<Args>(device, shaderBlobType)...
        , m_device(device)
    {
        setup(*this);
        CompileShaders();
    }

    operator std::shared_ptr<Program>& ()
    {
        return m_program;
    }

    void UpdateProgram()
    {
        CompileShaders();
    }

private:
    template <typename T>
    bool ApplyCallback()
    {
        auto& api = static_cast<ShaderHolder<T>&>(*this).GetApi();
        api.CompileShader(m_device);
        return true;
    }

    template<typename ... ShadowsArgs> void DevNull(ShadowsArgs ... args) {}

    template<typename... ShadowsArgs>
    void EnumerateShader()
    {
        DevNull(ApplyCallback<ShadowsArgs>()...);
    }

    void CompileShaders()
    {
        EnumerateShader<Args...>();
        m_program = m_device.CreateProgram({ static_cast<ShaderHolder<Args>&>(*this).GetApi().shader ... });
    }

    RenderDevice& m_device;
    std::shared_ptr<Program> m_program;
};
