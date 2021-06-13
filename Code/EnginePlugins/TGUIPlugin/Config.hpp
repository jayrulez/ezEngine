/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_CONFIG_HPP
#define TGUI_CONFIG_HPP

// Config.hpp is generated by CMake, it should not be edited directly.

// Detect the platform, to enable platform-specific code
#if defined(_WIN32)
    #define TGUI_SYSTEM_WINDOWS // Windows
#elif defined(__APPLE__) && defined(__MACH__)
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define TGUI_SYSTEM_IOS // iOS
    #elif TARGET_OS_MAC
        #define TGUI_SYSTEM_MACOS // macOS
    #endif
#elif defined(__unix__)
    #if defined(__ANDROID__)
        #define TGUI_SYSTEM_ANDROID // Android
    #else //if defined(__linux__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__OpenBSD__)
        #define TGUI_SYSTEM_LINUX // Linux or BSD
    #endif
#endif

// TGUI will link in the same way as SFML, unless TGUI_DYNAMIC or TGUI_STATIC is defined
#if !defined(TGUI_DYNAMIC) && !defined(TGUI_STATIC)
    #ifdef SFML_STATIC
        #define TGUI_STATIC
    #endif
#endif

#ifndef TGUI_STATIC
    #ifdef TGUI_SYSTEM_WINDOWS
        // Windows compilers need specific (and different) keywords for export and import
        #ifdef TGUI_EXPORTS
            #define TGUI_API __declspec(dllexport)
        #else
            #define TGUI_API __declspec(dllimport)
        #endif

		// For Visual C++ compilers, we also need to turn off the annoying C4251 warning
        #ifdef _MSC_VER
            #pragma warning(disable: 4251)
        #endif
    #else
        #define TGUI_API __attribute__ ((__visibility__ ("default")))
    #endif
#else
    // Static build doesn't need import/export macros
    #define TGUI_API
    #define TGUI_API
#endif

// Enables code that uses SFML
#define TGUI_BUILD_WITH_SFML 1

// Define that specifies the mininmum c++ support in both the TGUI code and user code.
// This constant can be lower than the actual c++ standard version used to compile with,
// as long as this constant is the same when compiling TGUI and when using the TGUI libs.
#define TGUI_COMPILED_WITH_CPP_VER 14

// Enable the use of std::filesystem if TGUI is built with c++17 with a new enough compiler.
// Although GCC and clang supported it before version 9, this is the first version where no
// additional library has to be linked in order to use std::filesystem. This is also the
// reason why we can't rely on __cpp_lib_filesystem for this.
#if TGUI_COMPILED_WITH_CPP_VER >= 17
    #if defined(__clang_major__) && (__clang_major__ >= 9) \
     || defined(__GNUC__) && (__GNUC___ >= 9) \
     || defined(_MSC_VER) && (_MSC_VER >= 1914)
        #define TGUI_USE_STD_FILESYSTEM
    #endif
#endif

// Include the version header when c++20 is available to use the library feature-testing macros
#if TGUI_COMPILED_WITH_CPP_VER >= 20
    #include <version>
#endif

#if defined(__cpp_constexpr) && (__cpp_constexpr >= 201304L)
    #define TGUI_CONSTEXPR constexpr
#else
    #define TGUI_CONSTEXPR inline
#endif

#if __cplusplus >= 201703L
    #define TGUI_EMPLACE_BACK(object, vector) auto& object = vector.emplace_back();
#else
    #define TGUI_EMPLACE_BACK(object, vector) vector.emplace_back(); auto& object = vector.back();
#endif

#ifndef TGUI_NO_DEPRECATED_WARNINGS
    #define TGUI_DEPRECATED(msg) [[deprecated(msg)]]
#else
    #define TGUI_DEPRECATED(msg)
#endif

#if !defined(NDEBUG) && !defined(TGUI_NO_RUNTIME_WARNINGS)
    #include <iostream>
    #define TGUI_PRINT_WARNING(msg) { std::cerr << "TGUI warning: " << msg << "\n"; }
#else
    #define TGUI_PRINT_WARNING(msg)
#endif

#if !defined(NDEBUG) && !defined(TGUI_DISABLE_ASSERTS)
    #include <iostream>
    #include <cassert>
    #define TGUI_ASSERT(condition, msg) { if (!(condition)) { std::cerr << "TGUI assertion: " << msg << "\n"; assert(condition); } }
#else
    #define TGUI_ASSERT(condition, msg)
#endif

// Using [=] gives a warning in c++20, but using [=,this] may not compile with older c++ versions
#if __cplusplus > 201703L
    #define TGUI_LAMBDA_CAPTURE_EQ_THIS [=,this]
#else
    #define TGUI_LAMBDA_CAPTURE_EQ_THIS [=]
#endif


// Version of the library
#define TGUI_VERSION_MAJOR 0
#define TGUI_VERSION_MINOR 9
#define TGUI_VERSION_PATCH 0

#endif // TGUI_CONFIG_HPP
