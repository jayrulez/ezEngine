ez_cmake_init()

ez_requires_d3d()

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(APPLICATION ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  RendererCore
  ${SCRIPT_PLUGINS}
)

if (EZ_3RDPARTY_HASHLINK_SUPPORT)
  target_link_libraries(${PROJECT_NAME} PUBLIC HashLinkPlugin)
endif()

if (EZ_3RDPARTY_ANGELSCRIPT_SUPPORT)
  target_link_libraries(${PROJECT_NAME} PUBLIC AngelscriptPlugin)
endif()

if (EZ_3RDPARTY_DAO_SUPPORT)
  target_link_libraries(${PROJECT_NAME} PUBLIC DaoPlugin)
endif()

add_dependencies(${PROJECT_NAME}
  InspectorPlugin
)

if (EZ_3RDPARTY_HASHLINK_SUPPORT)
  add_dependencies(${PROJECT_NAME} HashLinkPlugin)
endif()

if (EZ_3RDPARTY_ANGELSCRIPT_SUPPORT)
  add_dependencies(${PROJECT_NAME} AngelscriptPlugin)
endif()

if (EZ_3RDPARTY_DAO_SUPPORT)
  add_dependencies(${PROJECT_NAME} DaoPlugin)
endif()
