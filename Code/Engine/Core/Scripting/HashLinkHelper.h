#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Timestamp.h>

#ifdef BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT
extern "C"
{
#  include <HashLink/src/hl.h>
#  include <HashLink/src/hlc.h>
#  include <HashLink/src/hlmodule.h>
}

class EZ_CORE_DLL ezHashLinkHelper
{
};

#endif
