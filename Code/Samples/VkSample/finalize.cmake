if (TARGET VkSample AND TARGET RendererDX11)
  add_dependencies(VkSample RendererDX11)
endif()

if (TARGET VkSample AND TARGET RendererVk)
  add_dependencies(VkSample RendererVk)
endif()
