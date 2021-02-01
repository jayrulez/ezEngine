if (TARGET RendererDX11)
  add_dependencies(AngelscriptSample RendererDX11)
endif()

if (TARGET RendererVulkan)
  add_dependencies(AngelscriptSample RendererVulkan)
endif()
