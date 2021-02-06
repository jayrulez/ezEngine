if (TARGET RendererDX11)
  add_dependencies(ScriptingSample RendererDX11)
endif()

if (TARGET RendererVulkan)
  add_dependencies(ScriptingSample RendererVulkan)
endif()
