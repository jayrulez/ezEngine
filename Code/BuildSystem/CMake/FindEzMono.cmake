# find the folder in which Mono is located

# early out, if this target has been created before
if (TARGET EzMono::EzMono)
	return()
endif()

get_property(EZ_SUBMODULE_PREFIX_PATH GLOBAL PROPERTY EZ_SUBMODULE_PREFIX_PATH)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  find_path(EZ_MONO_DIR "include/mono-2.0/mono/jit/mono-private-unstable.h"
    PATHS
    ${CMAKE_SOURCE_DIR}/${EZ_SUBMODULE_PREFIX_PATH}/Code/ThirdParty/mono/windows.x64.Release
  )
  set(MONO_BIN_PATH "${EZ_MONO_DIR}/bin")
  set(MONO_LIB_PATH "${EZ_MONO_DIR}/lib")
  set(MONO_CORELIB_PATH "${EZ_MONO_DIR}/System.Private.CoreLib")
else()
  find_path(EZ_MONO_DIR "include/mono-2.0/mono/jit/mono-private-unstable.h"
    PATHS
    ${CMAKE_SOURCE_DIR}/${EZ_SUBMODULE_PREFIX_PATH}/Code/ThirdParty/mono/windows.x86.Release
  )
  set(MONO_BIN_PATH "${EZ_MONO_DIR}/bin")
  set(MONO_LIB_PATH "${EZ_MONO_DIR}/lib")
  set(MONO_CORELIB_PATH "${EZ_MONO_DIR}/System.Private.CoreLib")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EzMono DEFAULT_MSG EZ_MONO_DIR)

if (EZMONO_FOUND)
	add_library(EzMono::EzMono SHARED IMPORTED)
	set_target_properties(EzMono::EzMono PROPERTIES IMPORTED_LOCATION "${MONO_BIN_PATH}/monosgen-2.0.dll")
	set_target_properties(EzMono::EzMono PROPERTIES IMPORTED_IMPLIB "${MONO_LIB_PATH}/monosgen-2.0.import.lib")
	set_target_properties(EzMono::EzMono PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${EZ_MONO_DIR}/include/mono-2.0/")
	
	set(EZ_MONO_CORELIB_FILES
	  ${MONO_CORELIB_PATH}/System.Private.CoreLib.deps.json
	  ${MONO_CORELIB_PATH}/System.Private.CoreLib.dll
	  ${MONO_CORELIB_PATH}/System.Private.CoreLib.pdb
	  ${MONO_CORELIB_PATH}/System.Private.CoreLib.xml
	)
endif()

mark_as_advanced(FORCE EZ_MONO_DIR)

unset (MONO_BIN_PATH)
unset (MONO_LIB_PATH)
unset (MONO_CORELIB_PATH)