ez_cmake_init()

ez_build_filter_renderer()

find_package(Dotnet 5.0.100 REQUIRED)

set(CSPROJ ${CMAKE_CURRENT_LIST_DIR}/Managed/ezEngineDotNet.csproj)

set(OPTIONS
  /nologo
  /p:IntermediateOutputPath=${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Dotnet_Build.dir/
  /p:MSBuildProjectExtensionsPath=${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/Dotnet_Build.dir/
  /p:OutDir=${CMAKE_CURRENT_BINARY_DIR}/
  /p:PublishDir=${CMAKE_CURRENT_BINARY_DIR}/publish/
)

execute_process(
  COMMAND ${Dotnet_PROGRAM} msbuild ${CSPROJ} ${OPTIONS}
    /t:Restore
)

execute_process(
  COMMAND ${Dotnet_PROGRAM} msbuild ${CSPROJ} ${OPTIONS}
    /t:GetCMakeOutputAssembly
  OUTPUT_VARIABLE OUTPUT_ASSEMBLY
)
string(STRIP "${OUTPUT_ASSEMBLY}" OUTPUT_ASSEMBLY)

execute_process(
  COMMAND ${Dotnet_PROGRAM} msbuild ${CSPROJ} ${OPTIONS}
    /t:GetCMakeOutputByproducts
  OUTPUT_VARIABLE OUTPUT_BYPRODUCTS
)
string(STRIP "${OUTPUT_BYPRODUCTS}" OUTPUT_BYPRODUCTS)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/publish)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_ASSEMBLY}
  BYPRODUCTS ${OUTPUT_BYPRODUCTS}
  COMMAND ${Dotnet_PROGRAM} msbuild ${CSPROJ} ${OPTIONS}
    /t:Build,Publish
  COMMENT "Building .NET subproject"
)

add_custom_target(Dotnet_Build
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_ASSEMBLY}
)

add_executable(Dotnet IMPORTED GLOBAL)
add_dependencies(Dotnet Dotnet_Build)
set_target_properties(Dotnet
  PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/publish/IF_YOU_SEE_THIS_YOU_TRIED_TO_RUN_THE_DOTNET_TARGET
)

if(EZ_CMAKE_PLATFORM_WINDOWS)
  set(nethost_lib "nethost.dll")
elseif(EZ_CMAKE_PLATFORM_OSX)
  set(nethost_lib "libnethost.dylib")
elseif(EZ_CMAKE_PLATFORM_LINUX)
  set(nethost_lib "libnethost.so")
endif()

add_library(CoreCLR::nethost SHARED IMPORTED GLOBAL)
set_target_properties(CoreCLR::nethost
  PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/publish"
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/publish/${nethost_lib}"
)

if(EZ_CMAKE_PLATFORM_WINDOWS)
  set_target_properties(CoreCLR::nethost
    PROPERTIES
      IMPORTED_IMPLIB "${CMAKE_CURRENT_BINARY_DIR}/publish/nethost.lib"
  )
# TODO: check if this is necessary on OS X too
elseif(EZ_CMAKE_PLATFORM_LINUX)
  set_target_properties(CoreCLR::nethost
    PROPERTIES
      INTERFACE_LINK_LIBRARIES "dl"
  )
endif()

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(LIBRARY ${PROJECT_NAME})

add_dependencies(${PROJECT_NAME} Dotnet)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  CoreCLR::nethost
  Core
  RendererCore
  GameEngine
)

#find_library(NETHOST_LIBRARY NAMES libnethost nethost)
#target_link_libraries(${PROJECT_NAME} PRIVATE ${NETHOST_LIBRARY})
