cmake_minimum_required(VERSION "3.12.4")
project(Orbit)
set(Orbit_VERSION 0.0.1)

include(GenerateExportHeader)
include(CMakeHelper.txt)

# Change this part so that it works with Linux
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

if (NOT (EXISTS ${ORBIT_APPDATA}/__general))
	file(MAKE_DIRECTORY ${ORBIT_APPDATA})
	message(STATUS "Copying")
	file(COPY ${CMAKE_SOURCE_DIR}/projects/__general DESTINATION ${ORBIT_APPDATA})
	file(COPY ${CMAKE_SOURCE_DIR}/scripts/create_project.bat DESTINATION ${ORBIT_APPDATA})
endif()

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

install(TARGETS
        ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Config
    LIBRARY DESTINATION src
	PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/inc
	INCLUDES DESTINATION ${CMAKE_SOURCE_DIR}/inc
)
set_target_properties(${PROJECT_NAME} PROPERTIES ORBIT_RENDER_ENGINE ${ORBIT_RENDER_ENGINE})
export(TARGETS
        ${PROJECT_NAME}
    NAMESPACE Orbit::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
)
export (PACKAGE ${PROJECT_NAME})