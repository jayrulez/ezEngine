ez_cmake_init()

ez_build_filter_everything()

set(EZ_RHI_D3D11_SUPPORTED FALSE CACHE INTERNAL "")
set(EZ_RHI_D3D12_SUPPORTED FALSE CACHE INTERNAL "")
set(EZ_RHI_VULKAN_SUPPORTED FALSE CACHE INTERNAL "")

if(EZ_CMAKE_PLATFORM_WINDOWS_UWP)
  set(EZ_RHI_D3D11_SUPPORTED TRUE CACHE INTERNAL "D3D11 is supported on UWP platform")
  set(EZ_RHI_D3D12_SUPPORTED TRUE CACHE INTERNAL "D3D12 is supported on UWP platform")
elseif(EZ_CMAKE_PLATFORM_WINDOWS)
  set(EZ_RHI_D3D11_SUPPORTED TRUE CACHE INTERNAL "D3D11 is supported on Win32 platform")
  set(EZ_RHI_D3D12_SUPPORTED TRUE CACHE INTERNAL "D3D12 is supported on Win32 platform")
  set(EZ_RHI_VULKAN_SUPPORTED TRUE CACHE INTERNAL "Vulkan is supported on Win32 platform")
elseif(EZ_CMAKE_PLATFORM_ANDROID)
  set(EZ_RHI_VULKAN_SUPPORTED TRUE CACHE INTERNAL "Vulkan is supported on Android platform")
elseif(EZ_CMAKE_PLATFORM_LINUX)
  set(EZ_RHI_VULKAN_SUPPORTED TRUE CACHE INTERNAL "Vulkan is supported on Linux platform")
elseif(EZ_CMAKE_PLATFORM_OSX)
  set(EZ_RHI_VULKAN_SUPPORTED TRUE CACHE INTERNAL "Vulkan is supported on OSX platform")
#elseif(EZ_CMAKE_PLATFORM_IOS)
#
else()
  message(FATAL_ERROR "RHI does not have a backend that is supported on this platform.")
endif()

option(EZ_RHI_NO_D3D11 "Disable D3D11 backend" OFF)
option(EZ_RHI_NO_D3D12 "Disable D3D11 backend" OFF)
option(EZ_RHI_NO_VULKAN "Disable Vulkan backend" OFF)

if(${EZ_RHI_NO_D3D11})
    set(EZ_RHI_D3D11_SUPPORTED FALSE CACHE INTERNAL "D3D11 backend is forcibly disabled")
endif()

if(${EZ_RHI_NO_D3D12})
    set(EZ_RHI_D3D12_SUPPORTED FALSE CACHE INTERNAL "D3D12 backend is forcibly disabled")
endif()

if(${EZ_RHI_NO_VULKAN})
    set(EZ_RHI_VULKAN_SUPPORTED FALSE CACHE INTERNAL "Vulkan backend is forcibly disabled")
endif()

if(NOT (${EZ_RHI_D3D12_SUPPORTED} OR ${EZ_RHI_D3D12_SUPPORTED} OR ${EZ_RHI_VULKAN_SUPPORTED}))
    message(FATAL_ERROR "No rendering backends are select to build")
endif()

message("D3D11_SUPPORTED: " ${EZ_RHI_D3D11_SUPPORTED})
message("D3D12_SUPPORTED: " ${EZ_RHI_D3D12_SUPPORTED})
message("VULKAN_SUPPORTED: " ${EZ_RHI_VULKAN_SUPPORTED})


# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(LIBRARY ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}
  PUBLIC 
  EZ_RHI_D3D11_SUPPORTED=$<BOOL:${EZ_RHI_D3D11_SUPPORTED}>
  EZ_RHI_D3D12_SUPPORTED=$<BOOL:${EZ_RHI_D3D12_SUPPORTED}>
  EZ_RHI_VULKAN_SUPPORTED=$<BOOL:${EZ_RHI_VULKAN_SUPPORTED}>
)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W0 /WX)
  #target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  #target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)
endif()

if(${EZ_RHI_D3D11_SUPPORTED})
  ez_link_target_dx11(${PROJECT_NAME})
endif()

if(${EZ_RHI_D3D12_SUPPORTED})
  # TODO link dx12
endif()

if(${EZ_RHI_VULKAN_SUPPORTED})
  # TODO link vulkan properly
  # TODO link spirv and spirv-reflect
  find_package(Vulkan REQUIRED)
  target_link_libraries(${PROJECT_NAME}
      PUBLIC
      Vulkan::Vulkan
  )
endif()

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Foundation
)
