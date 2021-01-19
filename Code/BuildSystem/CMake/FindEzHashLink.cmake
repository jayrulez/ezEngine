
# find the folder in which HashLink is located

# early out, if this target has been created before
if (TARGET EzHashLink::EzHashLink)
	return()
endif()

get_property(EZ_SUBMODULE_PREFIX_PATH GLOBAL PROPERTY EZ_SUBMODULE_PREFIX_PATH)

find_path(EZ_HASHLINK_DIR hashlink/hl.h
  PATHS
  ${CMAKE_SOURCE_DIR}/${EZ_SUBMODULE_PREFIX_PATH}/Code/ThirdParty/HashLink
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(HASHLINK_LIB_PATH "${EZ_HASHLINK_DIR}/vc142win64")
else()
  set(HASHLINK_LIB_PATH "${EZ_HASHLINK_DIR}/vc142win32")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EzHashLink DEFAULT_MSG EZ_HASHLINK_DIR)

if (EZHASHLINK_FOUND)

	add_library(EzHashLink::EzHashLink SHARED IMPORTED)
	set_target_properties(EzHashLink::EzHashLink PROPERTIES IMPORTED_LOCATION "${HASHLINK_LIB_PATH}/libhl.dll")
	set_target_properties(EzHashLink::EzHashLink PROPERTIES IMPORTED_IMPLIB "${HASHLINK_LIB_PATH}/libhl.lib")
	set_target_properties(EzHashLink::EzHashLink PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${EZ_HASHLINK_DIR}")

endif()

mark_as_advanced(FORCE EZ_HASHLINK_DIR)

unset (HASHLINK_LIB_PATH)





