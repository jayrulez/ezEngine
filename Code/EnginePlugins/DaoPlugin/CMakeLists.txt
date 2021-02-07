ez_cmake_init()

ez_build_filter_renderer()

ez_requires(EZ_3RDPARTY_DAO_SUPPORT)

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(LIBRARY ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Core
  RendererCore
  GameEngine
  Dao
)

if(MSVC)
  add_definitions(-DUNICODE -D_UNICODE)
	target_compile_options(${PROJECT_NAME} PRIVATE /W0 /WX-)
endif()
