if (TARGET Player AND TARGET UIPlugin)

    # Make sure this project is built when the Editor is built
    add_dependencies(Player UIPlugin)

endif()
