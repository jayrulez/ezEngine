#pragma once

#include "StaticRTTI.h"

// ****************************************************
// ***** Templates for accessing static RTTI data *****

/// \brief All classes that should be dynamically reflectable, need to be derived from this base class.
class ezReflectedClass
{
public:
  virtual const ezRTTI* GetDynamicRTTI()
  {
    return ezGetStaticRTTI<ezReflectedClass>();
  }
};

EZ_DECLARE_REFLECTABLE_TYPE(EZ_NO_LINKAGE, ezReflectedClass);

/// \brief This needs to be put into the class declaration of EVERY dynamically reflectable class.
#define EZ_ADD_DYNAMIC_REFLECTION(SELF)                               \
  public:                                                             \
    virtual const ezRTTI* GetDynamicRTTI() EZ_OVERRIDE                \
    {                                                                 \
      return ezGetStaticRTTI<SELF>();                                 \
    }                                                                 \
  private:                                                            \

