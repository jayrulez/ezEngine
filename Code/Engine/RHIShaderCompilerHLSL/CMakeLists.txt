ez_cmake_init()

ez_build_filter_renderer()

add_library(dxc2 INTERFACE)
target_include_directories(dxc2 INTERFACE ${CMAKE_SOURCE_DIR}/Code/ThirdParty/dxc2/include)
target_compile_definitions(dxc2 INTERFACE DXC_CUSTOM_LOCATION="${CMAKE_SOURCE_DIR}/Code/ThirdParty/dxc2/bin")
set(dxc_default_location "C:/Program Files (x86)/Windows Kits/10/Bin/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/x64")
target_compile_definitions(dxc2 INTERFACE DXC_DEFAULT_LOCATION="${dxc_default_location}")
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Code/ThirdParty/dxc2/cmake/modules")

find_package(DiaSDK REQUIRED)
add_library(dia INTERFACE)
target_include_directories(dia INTERFACE "${DIASDK_INCLUDE_DIRS}")

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

set(RSCH_HEADERS Compiler.h DXCLoader.h ShaderReflection.h)
set(RSCH_SOURCES Compiler.cpp DXCLoader.cpp ShaderReflection.cpp)

include_directories()

#if (VULKAN_SUPPORT)
list(APPEND RSCH_HEADERS SPIRVReflection.h)
list(APPEND RSCH_SOURCES SPIRVReflection.cpp)
#endif()

#if (DIRECTX_SUPPORT)
list(APPEND RSCH_HEADERS DXILReflection.h)
list(APPEND RSCH_SOURCES DXILReflection.cpp)
#endif()

set(RSCH_MANUAL_SOURCES ${RSCH_HEADERS} ${RSCH_SOURCES})

ez_create_target(LIBRARY ${PROJECT_NAME} MANUAL_SOURCE_FILES ${RSCH_MANUAL_SOURCES})

#if (VULKAN_SUPPORT)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  spirv-cross-core
  spirv-cross-hlsl
)
#endif()

#if (DIRECTX_SUPPORT)
target_link_libraries(${PROJECT_NAME}
  PUBLIC
  d3d12
  dxgi
  dxguid
  dia
)
#endif()

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Foundation
  RHI
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    d3dcompiler
    dxc2
)
