ez_cmake_init()

ez_requires_mono()

set(EZ_MANAGED_ASSEMBLY_NAME "ezEngine.Managed")
set(EZ_MANAGED_CSPROJ ${EZ_MANAGED_ASSEMBLY_NAME}.csproj)

# Get the name of this folder as the project name
# get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

set(EZ_MANAGED_ASSEMBLY_EXCLUDE_FILES "")
set(EXCLUDE_FILES ${EZ_MANAGED_CSPROJ}.in CMakeLists.txt)
message(${EXCLUDE_FILES})
foreach(EXCLUDE_FILE ${EXCLUDE_FILES})
  message(${EXCLUDE_FILE})
  string(APPEND EZ_MANAGED_ASSEMBLY_EXCLUDE_FILES "    <None Remove=\"${EXCLUDE_FILE}\" />\n")
endforeach()

set(EZ_MANAGED_ASSEMBLY_OUTPUT ${EZ_OUTPUT_DIRECTORY_DLL}/Assemblies)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${EZ_MANAGED_CSPROJ}.in ${CMAKE_CURRENT_SOURCE_DIR}/${EZ_MANAGED_CSPROJ})

if(MSVC)
  include_external_msproject(${EZ_MANAGED_ASSEMBLY_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EZ_MANAGED_CSPROJ})
  set_property(TARGET ${EZ_MANAGED_ASSEMBLY_NAME} PROPERTY FOLDER Scripting)
else()
 # todo
endif()

ez_set_default_target_output_dirs(${EZ_MANAGED_ASSEMBLY_NAME})