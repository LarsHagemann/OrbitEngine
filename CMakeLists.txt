cmake_minimum_required(VERSION "3.12.4")
project(Orbit)

include(CMakeHelper.txt)

file(TO_CMAKE_PATH "$ENV{TMP}" ENV_TMP)
get_filename_component(ORBIT_APPDATA "../../Roaming/Treelab/OrbitEngine/"
                       REALPATH BASE_DIR "${ENV_TMP}")

set_option(BUILD_SAMPLES TRUE BOOL "Uncheck this value if you don't want to build the samples.")
set_option(WITH_DEAR_IMGUI TRUE BOOL "Uncheck to build without ImGui")
set_option(PHYSX_ROOT_PATH "" PATH "Path to your NVidia PhysX build (Root folder with physx and physxshared).")
set_option(PHYSX_LIBRARY_PATH "" PATH "Path to the PhysX bin folder to use (The folder with the built debug/checked/release/profile folders inside).")
set_option(EIGEN_ROOT_PATH "" PATH "Leave this empty if Eigen is located in the dep/ folder.")
set_option(IMGUI_ROOT_PATH "" PATH "Leave this empty if ImGui sources and headers are located in the dep/ folder.")
set_option(ORBIT_RENDERER "DX12" STRING "Select the renderer you want to use (DX12, DX11 or OpenGL)")
set_option(TARGET_PLATFORM "win32" STRING "Select the platform you want to develop for (win32)")
set_property(CACHE ORBIT_RENDERER PROPERTY STRINGS DX12)
set_property(CACHE TARGET_PLATFORM PROPERTY STRINGS win32)

if ("${PHYSX_ROOT_PATH}" STREQUAL "")
	message(FATAL_ERROR "Unable to find NVidia Physx built. Please set the PHYSX_ROOT_PATH variable.")
endif()
if ("${PHYSX_LIBRARY_PATH}" STREQUAL "")
	message(FATAL_ERROR "Unable to find NVidia Physx built. Please set the PHYSX_LIBRARY_PATH variable.")
endif()

file(MAKE_DIRECTORY ${ORBIT_APPDATA}/__general/)
file(COPY ${CMAKE_SOURCE_DIR}/shader DESTINATION ${ORBIT_APPDATA}/__general/)
# copy shaders into the appdata path

set(ORBIT_RENDER_ENGINE "ORBIT_DX12")
if ("${ORBIT_RENDERER}" STREQUAL "DX12")
	set(ORBIT_RENDER_ENGINE "ORBIT_DX12")
elseif ("${ORBIT_RENDERER}" STREQUAL "OpenGL")
	set(ORBIT_RENDER_ENGINE "ORBIT_OPENGL")
elseif ("${ORBIT_RENDERER}" STREQUAL "DX11")
	set(ORBIT_RENDER_ENGINE "ORBIT_DX11")
else()
	message(FATAL_ERROR "Invalid render engine selected. Choose one of DX12, OpenGL, DX11.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(converter)
if (BUILD_SAMPLES)
	add_subdirectory(samples)
endif()