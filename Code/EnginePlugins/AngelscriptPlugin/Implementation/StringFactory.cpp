#include <AngelscriptPlugin/StringFactory.h>

ezStringFactory::ezStringFactory() {}

ezStringFactory::~ezStringFactory()
{
  EZ_ASSERT_ALWAYS(m_StringCache.GetCount() == 0, "The script engine must release each string constant that it has requested");
}

const void* ezStringFactory::GetStringConstant(const char* data, asUINT length)
{
  // The string factory might be modified from multiple
  // threads, so it is necessary to use a mutex.
  asAcquireExclusiveLock();

  ezString str(data);

  ezUInt32 strIndex = m_StringCache.Find(str);
  if (strIndex != ezInvalidIndex)
  {
    m_StringCache[str]++;
  }
  else
  {
    strIndex = m_StringCache.Insert(str, 1);
  }

  asReleaseExclusiveLock();

  return reinterpret_cast<const void*>(&m_StringCache.GetKey(strIndex));
}

ezInt32 ezStringFactory::ReleaseStringConstant(const void* str)
{
  if (!str)
    return asERROR;

  ezInt32 result = asSUCCESS;

  asAcquireExclusiveLock();
  ezString key(*reinterpret_cast<const ezString*>(str));
  ezUInt32 strIndex = m_StringCache.Find(key);

  if (strIndex == ezInvalidIndex)
  {
    result = asERROR;
  }
  else
  {
    m_StringCache[key]--;
    if (m_StringCache[key] == 0)
      m_StringCache.RemoveAtAndCopy(strIndex);
  }

  asReleaseExclusiveLock();

  return result;
}

ezInt32 ezStringFactory::GetRawStringData(const void* str, char* data, asUINT* length) const
{
  if (!str)
    return asERROR;

  const ezString* nativeStr = reinterpret_cast<const ezString*>(str);

  if (length)
    *length = nativeStr->GetElementCount();

  if (data)
  {
    ezMemoryUtils::Copy(data, const_cast<char*>((*nativeStr).GetData()), nativeStr->GetElementCount());
  }

  return asSUCCESS;
}
