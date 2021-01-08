#include "..\MonoField.h"

ezMonoClass* ezMonoField::GetType()
{
  return nullptr;
}

void ezMonoField::Get(MonoObject* instance, void* outValue)
{
}

MonoObject* ezMonoField::GetBoxed(MonoObject* instance)
{
  return nullptr;
}

void ezMonoField::Set(MonoObject* instance, void* value)
{
}

bool ezMonoField::HasAttribute(ezMonoClass* monoClass)
{
  return false;
}

MonoObject* ezMonoField::GetAttribute(ezMonoClass* monoClass)
{
  return nullptr;
}

ezMonoMemberVisibility ezMonoField::GetVisibility()
{
  return ezMonoMemberVisibility();
}

bool ezMonoField::IsStatic()
{
  return false;
}
