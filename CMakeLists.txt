cmake_minimum_required(VERSION "3.12.4")
project(Orbit)

include(CMakeHelper.txt)

#_DISABLE_EXTENDED_ALIGNED_STORAGE

set_option(BUILD_SAMPLES TRUE BOOL "Uncheck this value if you don't want to build the samples.")
set_option(WITH_DEAR_IMGUI TRUE BOOL "Uncheck to build without ImGui")
set_option(PHYSX_ROOT_PATH "" PATH "Path to your NVidia PhysX build (Root folder with physx and physxshared).")
set_option(PHYSX_LIBRARY_PATH "" PATH "Path to the PhysX bin folder to use (The folder with the built debug/checked/release/profile folders inside).")
set_option(EIGEN_ROOT_PATH "" PATH "Leave this empty if Eigen is located in the dep/ folder.")
set_option(IMGUI_ROOT_PATH "" PATH "Leave this empty if ImGui sources and headers are located in the dep/ folder.")

if ("${PHYSX_ROOT_PATH}" STREQUAL "")
	message(FATAL_ERROR "Unable to find NVidia Physx built. Please set the PHYSX_ROOT_PATH variable.")
endif()
if ("${PHYSX_LIBRARY_PATH}" STREQUAL "")
	message(FATAL_ERROR "Unable to find NVidia Physx built. Please set the PHYSX_LIBRARY_PATH variable.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
if (BUILD_SAMPLES)
	add_subdirectory(samples)
endif()
