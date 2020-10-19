ez_cmake_init()

ez_build_filter_everything()

find_package(Vulkan REQUIRED)

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(LIBRARY ${PROJECT_NAME})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W0 /WX)
endif()

#target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Foundation
  Vulkan::Vulkan
)
