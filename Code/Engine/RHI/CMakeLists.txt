#flycube: 26c2c71d7c4eeb046b8c68cb04b93311457ed734
ez_cmake_init()

ez_build_filter_renderer()

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(LIBRARY ${PROJECT_NAME})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Foundation
)
