ez_cmake_init()

ez_build_filter_runtime()

ez_requires(EZ_3RDPARTY_HASHLINK_SUPPORT)

#set(DEFAULT_CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ${CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS})
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(HL_VERSION_MAJOR 1)
set(HL_VERSION_MINOR 12)
set(HL_VERSION_PATCH 0)
set(HL_VERSION ${HL_VERSION_MAJOR}.${HL_VERSION_MINOR}.${HL_VERSION_PATCH})

set (PROJECT_NAME "libhl")

file(GLOB hl_headers
    src/*.h
)

file(GLOB pcre_srcs
    include/pcre/*.c
)
file(GLOB std_srcs
    src/std/*.h
    src/std/array.c
    src/std/buffer.c
    src/std/bytes.c
    src/std/cast.c
    src/std/date.c
    src/std/error.c
    src/std/file.c
    src/std/fun.c
    src/std/maps.c
    src/std/math.c
    src/std/obj.c
    src/std/random.c
    src/std/regexp.c
    src/std/socket.c
    src/std/string.c
    src/std/sys.c
    src/std/track.c
    src/std/types.c
    src/std/ucs2.c
    src/std/thread.c
    src/std/process.c
)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    list(APPEND std_srcs
        include/mdbg/mdbg.c
        include/mdbg/mach_excServer.c
        include/mdbg/mach_excUser.c
    )
endif()

if(ANDROID)
    list(APPEND std_srcs
        src/std/sys_android.c
    )
endif()

if(IOS_PLATFORM)
    list(APPEND std_srcs
        src/std/sys_ios.m
   )
endif()

ez_create_target(LIBRARY ${PROJECT_NAME} NO_EZ_PREFIX SOURCE_FILES
    ${pcre_srcs}
    ${hl_headers}
    src/gc.c
    src/code.c
    src/debugger.c
    src/jit.c
    src/module.c
	  src/profile.c
    ${std_srcs}
)

set(EZ_HASHLINK_IMP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/code.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/debugger.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module.c
	  ${CMAKE_CURRENT_SOURCE_DIR}/src/profile.c
)

#set_target_properties(${PROJECT_NAME}
#    PROPERTIES
#    VERSION ${HL_VERSION}
#    SOVERSION ${HL_VERSION_MAJOR}
#    COMPILE_DEFINITIONS "_USRDLL;LIBHL_EXPORTS"
#)

target_compile_definitions(${PROJECT_NAME}
  PUBLIC
    BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT
    _USRDLL
    LIBHL_EXPORTS
    VERSION=${HL_VERSION}
    SOVERSION=${HL_VERSION_MAJOR}
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/include/pcre"
  PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/src"
    "${CMAKE_CURRENT_LIST_DIR}/src/std"
)

# force Unicode over Multi-byte
if(MSVC)
  add_definitions(-DUNICODE -D_UNICODE)
	target_compile_options(${PROJECT_NAME} PRIVATE /W0 /WX-)
endif()

#include_directories(
#    src
#    include
#    include/pcre
#)

if(EZ_CMAKE_PLATFORM_WINDOWS)
    target_link_libraries(${PROJECT_NAME} ws2_32 user32)
else()
    target_link_libraries(${PROJECT_NAME} m dl pthread)
endif()

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ${DEFAULT_CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS})
