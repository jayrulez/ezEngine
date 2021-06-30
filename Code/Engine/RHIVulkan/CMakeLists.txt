ez_cmake_init()

ez_build_filter_renderer()
#ez_requires_vulkan()

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(LIBRARY ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC)
target_compile_definitions(${PROJECT_NAME} PRIVATE VK_NO_PROTOTYPES)
target_compile_definitions(${PROJECT_NAME} PRIVATE VK_USE_PLATFORM_WIN32_KHR)

ez_link_target_vulkan(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  spirv-cross-core
  spirv-cross-hlsl
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Foundation
  RHI
)
