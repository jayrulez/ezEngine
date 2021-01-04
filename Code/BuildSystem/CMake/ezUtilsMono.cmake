######################################
### Mono support
######################################



######################################
### ez_requires_mono()
######################################

macro(ez_requires_mono)

	ez_requires_windows()
	if (EZ_CMAKE_PLATFORM_WINDOWS_UWP)
		return()
	endif()

endmacro()

######################################
### ez_link_target_mono(<target>)
######################################

function(ez_link_target_mono TARGET_NAME)

	ez_requires_mono()

	find_package(EzMono REQUIRED)

	if (EZMONO_FOUND)
	  target_link_libraries(${TARGET_NAME} PRIVATE EzMono::EzMono)
	
	  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:EzMono::EzMono> $<TARGET_FILE_DIR:${TARGET_NAME}>
	  )
	  
	  foreach(CORELIB_FILE ${EZ_MONO_CORELIB_FILES})
	    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
		  COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${CORELIB_FILE} $<TARGET_FILE_DIR:${TARGET_NAME}>
	    )
	  endforeach()
	
	endif()

endfunction()

