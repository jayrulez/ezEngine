######################################
### HashLink support
######################################



######################################
### ez_requires_hashlink()
######################################

macro(ez_requires_hashlink)

	ez_requires_windows()
	if (EZ_CMAKE_PLATFORM_WINDOWS_UWP)
		return()
	endif()

endmacro()

######################################
### ez_link_target_hashlink(<target>)
######################################

function(ez_link_target_hashlink TARGET_NAME)

	ez_requires_hashlink()

	find_package(EzHashLink REQUIRED)

	if (EZHASHLINK_FOUND)
	
	  target_link_libraries(${TARGET_NAME} PRIVATE EzHashLink::EzHashLink)
	  target_compile_definitions(${PROJECT_NAME} PUBLIC BUILDSYSTEM_ENABLE_HASHLINK_SUPPORT)
	
	  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:EzHashLink::EzHashLink> $<TARGET_FILE_DIR:${TARGET_NAME}>
	  )
	
	endif()

endfunction()

