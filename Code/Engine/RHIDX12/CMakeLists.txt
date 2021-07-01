ez_cmake_init()

ez_build_filter_renderer()
#ez_requires_d3d()

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(LIBRARY ${PROJECT_NAME})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE DIRECTX_SUPPORT=1)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Foundation
  RHI
  d3d12
  dxgi
  dxguid
  DirectX-Headers
  DirectX-Guids
)
