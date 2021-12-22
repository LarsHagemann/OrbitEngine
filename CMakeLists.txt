cmake_minimum_required(VERSION 3.20)
project(OrbitEngine)

include(CMakeHelper.txt)

set_option(BUILD_SAMPLES TRUE BOOL "Uncheck this value if you don't want to build the samples.")
set_option(EIGEN_ROOT_PATH "" PATH "Set the path to Eigen.")
set_option(PHYSX_ROOT_PATH "" PATH "Set the path to Nvidia Physx")
set_option(PHYSX_LIBRARY_PATH "" PATH "Set the path to the Nvidia Physx libraries that you have build")
set_option(IMGUI_ROOT_PATH "" PATH "Set the path to ImGui.")
set_option(ZLIB_ROOT_PATH "" PATH "Set the path to zlib.")
set_option(ORBIT_OS "Windows" STRING "The OS you want to develop for (Windows or Unix)")
set_option(ORBIT_RENDERER "" STRING "The backend you want to develop for (DX11, DX12 or OpenGL)")

# Enforce C++ >= 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${ORBIT_OS}" STREQUAL "Windows")
	set(ORBIT_SYSTEM "ORBIT_WINDOWS")
elseif("${ORBIT_OS}" STREQUAL "Unix")
	set(ORBIT_SYSTEM "ORBIT_UNIX")
else()
	message(WARNING "ORBIT_OS not set correctly (choose Windows or Unix). Compilation will fail.")
endif()

set(ORBIT_RENDER_ENGINE "ORBIT_DX11")
if ("${ORBIT_RENDERER}" STREQUAL "DX12")
	set(ORBIT_RENDER_ENGINE "ORBIT_DIRECTX_12")
elseif ("${ORBIT_RENDERER}" STREQUAL "OpenGL")
	set(ORBIT_RENDER_ENGINE "ORBIT_OPENGL")
elseif ("${ORBIT_RENDERER}" STREQUAL "DX11")
	set(ORBIT_RENDER_ENGINE "ORBIT_DIRECTX_11")
else()
	message(WARNING "ORBIT_RENDERER not set correctly (choose DX11, DX12 or OpenGL). Compilation will fail.")
endif()

add_subdirectory(src/)
add_subdirectory(converter/src/)

if (${BUILD_SAMPLES})
	add_subdirectory(samples/)
endif()