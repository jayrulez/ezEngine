#include <MonoPlugin/Mono/MonoMethod.h>
#include <mono/metadata/class.h>

ezMonoMethod::ezMonoMethod(MonoMethod* method)
{
}

void ezMonoMethod::CacheSignature() const
{
}

ezMonoMethod::~ezMonoMethod()
{
}

MonoObject* ezMonoMethod::Invoke(MonoObject* instance, void** params)
{
  return nullptr;
}

MonoObject* ezMonoMethod::InvokeVirtual(MonoObject* instance, void** params)
{
  return nullptr;
}

void* ezMonoMethod::GetThunk() const
{
  return nullptr;
}

ezString ezMonoMethod::GetName() const
{
  return ezString();
}

ezMonoClass* ezMonoMethod::GetReturnType() const
{
  return nullptr;
}

ezUInt32 ezMonoMethod::GetParameterCount() const
{
  return ezUInt32();
}

ezMonoClass* ezMonoMethod::GetParameterType(ezUInt32 paramIdx) const
{
  return nullptr;
}

bool ezMonoMethod::IsStatic() const
{
  return false;
}

bool ezMonoMethod::HasAttribute(ezMonoClass* monoClass) const
{
  return false;
}

MonoObject* ezMonoMethod::GetAttribute(ezMonoClass* monoClass) const
{
  return nullptr;
}

ezMonoMemberVisibility ezMonoMethod::GetVisibility()
{
  return ezMonoMemberVisibility();
}
