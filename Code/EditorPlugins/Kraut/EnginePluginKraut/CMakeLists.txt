ez_cmake_init()

ez_requires_editor()

ez_requires(EZ_BUILD_KRAUT)
ez_requires_kraut()

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(LIBRARY ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  EditorEngineProcessFramework
  GameEngine
  KrautPlugin
)

ez_link_target_kraut(${PROJECT_NAME})
