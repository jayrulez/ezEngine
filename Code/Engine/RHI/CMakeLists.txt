ez_cmake_init()

ez_build_filter_renderer()

set(EZ_RHI_D3D11_SUPPORTED FALSE CACHE INTERNAL "")
set(EZ_RHI_VULKAN_SUPPORTED FALSE CACHE INTERNAL "")
set(EZ_RHI_METAL_SUPPORTED FALSE CACHE INTERNAL "")
set(EZ_RHI_OPENGL_SUPPORTED FALSE CACHE INTERNAL "")
set(EZ_RHI_OPENGLES_SUPPORTED FALSE CACHE INTERNAL "")

if(EZ_CMAKE_PLATFORM_WINDOWS_UWP)
  set(EZ_RHI_D3D11_SUPPORTED TRUE CACHE INTERNAL "D3D11 is supported on UWP platform")
elseif(EZ_CMAKE_PLATFORM_WINDOWS)
  set(EZ_RHI_D3D11_SUPPORTED TRUE CACHE INTERNAL "D3D11 is supported on Win32 platform")
  set(EZ_RHI_VULKAN_SUPPORTED TRUE CACHE INTERNAL "Vulkan is supported on Win32 platform")
  set(EZ_RHI_OPENGL_SUPPORTED TRUE CACHE INTERNAL "OpenGL is supported on Win32 platform")
elseif(EZ_CMAKE_PLATFORM_ANDROID)
  set(EZ_RHI_VULKAN_SUPPORTED TRUE CACHE INTERNAL "Vulkan is supported on Android platform")
  set(EZ_RHI_OPENGLES_SUPPORTED TRUE CACHE INTERNAL "OpenGL ES is supported on Android platform")
elseif(EZ_CMAKE_PLATFORM_LINUX)
  set(EZ_RHI_VULKAN_SUPPORTED TRUE CACHE INTERNAL "Vulkan is supported on Linux platform")
  set(EZ_RHI_OPENGL_SUPPORTED TRUE CACHE INTERNAL "OpenGL is supported on Linux platform")
elseif(EZ_CMAKE_PLATFORM_OSX)
  set(EZ_RHI_METAL_SUPPORTED TRUE CACHE INTERNAL "Metal is supported on OSX platform")
#elseif(EZ_CMAKE_PLATFORM_IOS)
#
else()
  message(FATAL_ERROR "RHI does not have a backend that is supported on this platform.")
endif()

option(EZ_RHI_NO_D3D11 "Disable D3D11 backend" OFF)
option(EZ_RHI_NO_VULKAN "Disable Vulkan backend" OFF)
option(EZ_RHI_NO_METAL "Disable Metal backend" OFF)
option(EZ_RHI_NO_OPENGL "Disable OpenGL backend" OFF)
option(EZ_RHI_NO_OPENGLES "Disable OpenGL ES backend" OFF)

if(${EZ_RHI_NO_D3D11})
    set(EZ_RHI_D3D11_SUPPORTED FALSE CACHE INTERNAL "D3D11 backend is forcibly disabled")
endif()

if(${EZ_RHI_NO_VULKAN})
    set(EZ_RHI_VULKAN_SUPPORTED FALSE CACHE INTERNAL "Vulkan backend is forcibly disabled")
endif()

if(${EZ_RHI_NO_METAL})
    set(EZ_RHI_METAL_SUPPORTED FALSE CACHE INTERNAL "Metal backend is forcibly disabled")
endif()

if(${EZ_RHI_NO_OPENGL})
    set(EZ_RHI_OPENGL_SUPPORTED FALSE CACHE INTERNAL "OPENGL backend is forcibly disabled")
endif()

if(${EZ_RHI_NO_OPENGLES})
    set(EZ_RHI_OPENGLES_SUPPORTED FALSE CACHE INTERNAL "OpenGL ES backend is forcibly disabled")
endif()

if(NOT (${EZ_RHI_D3D11_SUPPORTED} OR ${EZ_RHI_VULKAN_SUPPORTED} OR ${EZ_RHI_METAL_SUPPORTED} OR ${EZ_RHI_OPENGL_SUPPORTED} OR ${EZ_RHI_OPENGLES_SUPPORTED}))
    message(FATAL_ERROR "No rendering backends are select to build")
endif()

message("D3D11_SUPPORTED: " ${EZ_RHI_D3D11_SUPPORTED})
message("VULKAN_SUPPORTED: " ${EZ_RHI_VULKAN_SUPPORTED})
message("METAL_SUPPORTED: " ${EZ_RHI_METAL_SUPPORTED})
message("GL_SUPPORTED: " ${EZ_RHI_OPENGL_SUPPORTED})
message("GLES_SUPPORTED: " ${EZ_RHI_OPENGLES_SUPPORTED})

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(LIBRARY ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}
  PUBLIC 
  EZ_RHI_D3D11_SUPPORTED=$<BOOL:${EZ_RHI_D3D11_SUPPORTED}>
  EZ_RHI_VULKAN_SUPPORTED=$<BOOL:${EZ_RHI_VULKAN_SUPPORTED}>
  EZ_RHI_METAL_SUPPORTED=$<BOOL:${EZ_RHI_METAL_SUPPORTED}>
  EZ_RHI_OPENGL_SUPPORTED=$<BOOL:${EZ_RHI_OPENGL_SUPPORTED}>
  EZ_RHI_OPENGLGLES_SUPPORTED=$<BOOL:${EZ_RHI_OPENGLES_SUPPORTED}>
)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
  #target_compile_options(${PROJECT_NAME} PRIVATE /W0 /WX-)
  target_compile_options(${PROJECT_NAME} PRIVATE /GR) #temporarily
endif()

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Foundation
)
