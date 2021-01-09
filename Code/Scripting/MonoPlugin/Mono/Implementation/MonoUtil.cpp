//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include <MonoPlugin/Mono/MonoUtil.h>

ezStringWChar ezMonoUtil::MonoToWideString(MonoString* str)
{
  if (str == nullptr)
    return ezStringWChar();

  int len = mono_string_length(str);
  mono_unichar2* monoChars = mono_string_chars(str);

  ezStringWChar ret(monoChars);
  return ret;
}

ezString ezMonoUtil::MonoToString(MonoString* str)
{
  ezStringWChar wideString = MonoToWideString(str);

  return ezStringUtf8(wideString).GetData();
}

MonoString* ezMonoUtil::WideStringToMono(const ezStringWChar& str)
{
  if (sizeof(wchar_t) == 2) // Assuming UTF-16
    return mono_string_from_utf16((mono_unichar2*)str.GetData());
  else // Assuming UTF-32
    return mono_string_from_utf32((mono_unichar4*)str.GetData());
}

MonoString* ezMonoUtil::StringToMono(const ezString& str)
{
  return WideStringToMono(ezStringWChar(str));
}

void ezMonoUtil::GetClassName(MonoObject* obj, ezString& classNamespace, ezString& typeName)
{
  if (obj == nullptr)
    return;

  MonoClass* monoClass = mono_object_get_class(obj);
  GetClassName(monoClass, classNamespace, typeName);
}

void ezMonoUtil::GetClassName(MonoClass* monoClass, ezString& classNamespace, ezString& typeName)
{
  MonoClass* nestingClass = mono_class_get_nesting_type(monoClass);

  if (nestingClass == nullptr)
  {
    classNamespace = mono_class_get_namespace(monoClass);
    typeName = mono_class_get_name(monoClass);

    return;
  }
  else
  {
    const char* className = mono_class_get_name(monoClass);

    // Class name is generally never null, except for the case of specialized generic types, which we handle
    // separately
    if (className)
      typeName = ezStringBuilder("+", className);

    do
    {
      MonoClass* nextNestingClass = mono_class_get_nesting_type(nestingClass);
      if (nextNestingClass != nullptr)
      {
        typeName = ezStringBuilder("+", mono_class_get_name(nestingClass), typeName);
        nestingClass = nextNestingClass;
      }
      else
      {
        classNamespace = mono_class_get_namespace(nestingClass);
        typeName = ezStringBuilder(mono_class_get_name(nestingClass), typeName);

        break;
      }
    } while (true);
  }
}

void ezMonoUtil::GetClassName(MonoReflectionType* monoReflType, ezString& classNamespace, ezString& typeName)
{
  MonoType* monoType = mono_reflection_type_get_type(monoReflType);
  ::MonoClass* monoClass = mono_class_from_mono_type(monoType);

  GetClassName(monoClass, classNamespace, typeName);
}

void ezMonoUtil::ThrowIfException(MonoException* exception)
{
  ThrowIfException(reinterpret_cast<MonoObject*>(exception));
}

void ezMonoUtil::ThrowIfException(MonoObject* exception)
{
  if (exception != nullptr)
  {
    MonoClass* exceptionClass = mono_object_get_class(exception);
    MonoProperty* exceptionMsgProp = mono_class_get_property_from_name(exceptionClass, "Message");
    MonoMethod* exceptionMsgGetter = mono_property_get_get_method(exceptionMsgProp);
    MonoString* exceptionMsg = (MonoString*)mono_runtime_invoke(exceptionMsgGetter, exception, nullptr, nullptr);

    MonoProperty* exceptionStackProp = mono_class_get_property_from_name(exceptionClass, "StackTrace");
    MonoMethod* exceptionStackGetter = mono_property_get_get_method(exceptionStackProp);
    MonoString* exceptionStackTrace = (MonoString*)mono_runtime_invoke(exceptionStackGetter, exception, nullptr, nullptr);

    // Note: If you modify this format make sure to also modify Log.ParseExceptionMessage in managed code.
    ezLog::Error("Managed exception: {0}\n{1}", MonoToString(exceptionMsg), MonoToString(exceptionStackTrace));
  }
}
