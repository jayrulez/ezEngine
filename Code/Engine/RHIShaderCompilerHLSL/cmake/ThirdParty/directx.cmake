option(DXHEADERS_BUILD_TEST "Disable building spirv tests" OFF)
option(DXHEADERS_INSTALL "Disable building spirv tests" OFF)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/DirectX-Headers")
set_target_properties(DirectX-Guids PROPERTIES FOLDER "ThirdParty/directx")