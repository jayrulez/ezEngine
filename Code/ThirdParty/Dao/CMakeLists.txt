ez_cmake_init()

ez_build_filter_runtime()

ez_requires(EZ_3RDPARTY_DAO_SUPPORT)

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

# Define source files
file(GLOB DAO_SOURCES 
  kernel/*.c
  modules/auxlib/*.c
  modules/stream/*.c
  kernel/*.h
  modules/auxlib/*.h
  modules/stream/*.h
)

list(REMOVE_ITEM DAO_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/kernel/daoMain.c)

ez_create_target(STATIC_LIBRARY ${PROJECT_NAME} NO_PCH NO_EZ_PREFIX MANUAL_SOURCE_FILES ${DAO_SOURCES})

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  -DTARGET_PLAT="\\\"${CMAKE_SYSTEM_NAME}\\\""
	#-DDAO_STATIC_BUILD
  -DDAO_HAS_STREAM
	-DDAO_KERNEL
	-DDAO_WITH_NUMARRAY
  PUBLIC
  -DDAO_DIRECT_API
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/kernel"
    "${CMAKE_CURRENT_SOURCE_DIR}/modules/auxlib"
    "${CMAKE_CURRENT_SOURCE_DIR}/modules/stream"
)

# Define dependency libs
if (READLINE_FOUND)
	target_compile_definitions(${PROJECT_NAME}
	  PRIVATE
		-DDAO_USE_READLINE
	)
	target_include_directories(${PROJECT_NAME}
	  PRIVATE
		"${READLINE_INCLUDE_DIRS}"
	)
	target_link_libraries(${PROJECT_NAME}
	  PRIVATE
		${READLINE_LIBRARIES}
	)
endif ()

if (CMAKE_HOST_APPLE)
	target_compile_definitions(${PROJECT_NAME}
	  PRIVATE
		-DUNIX
		-DMACOSX
	)
elseif (CMAKE_HOST_WIN32)
	target_compile_definitions(${PROJECT_NAME}
	  PRIVATE
		-DWIN32
	)
  target_link_libraries(${PROJECT_NAME} PRIVATE Winmm.dll)
elseif (CMAKE_HOST_UNIX)
	if (CMAKE_SYSTEM_NAME STREQUAL Linux)
		target_compile_definitions(${PROJECT_NAME}
		  PRIVATE
			-DUNIX
			-DLINUX
		)
	elseif (CMAKE_SYSTEM_NAME STREQUAL MinGW)
		target_compile_definitions(${PROJECT_NAME}
		  PRIVATE
			-DWIN32
		)
	endif ()
endif ()

# force Unicode over Multi-byte
if(MSVC)
  add_definitions(-DUNICODE -D_UNICODE)
	target_compile_options(${PROJECT_NAME} PRIVATE /W0 /WX-)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC BUILDSYSTEM_ENABLE_DAO_SUPPORT)
