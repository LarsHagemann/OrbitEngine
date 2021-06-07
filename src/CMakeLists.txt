source_group(zlib
	FILES
	zlib/adler32.c
	zlib/compress.c
	zlib/crc32.c
	zlib/deflate.c
	zlib/gzclose.c
	zlib/gzlib.c
    zlib/gzread.c
    zlib/gzwrite.c
	zlib/infback.c
	zlib/inffast.c
	zlib/inflate.c
	zlib/inftrees.c
	zlib/trees.c
	zlib/uncompr.c
	zlib/zutil.c
)

source_group(Engine
	FILES
	Engine/Engine.cpp
	Engine/Object.cpp
	Engine/Scene.cpp
	Engine/DebugObject.cpp
)

source_group(Engine\\Bindings
	FILES
	Engine/Bindings/DDSTextureLoader.cpp
	Engine/Bindings/WICTextureLoader.cpp
)

source_group(Engine\\Bindings\\DX
	FILES
	Engine/Bindings/DX/DXGIHelper.cpp
	Engine/Bindings/DX/DXInput.cpp
)

source_group(Engine\\Bindings\\DX12
	FILES
	Engine/Bindings/DX12/CommandQueue.cpp
	Engine/Bindings/DX12/DX12Engine.cpp
	Engine/Bindings/DX12/DX12Helper.cpp
	Engine/Bindings/DX12/DX12Renderer.cpp
	Engine/Bindings/DX12/DX12ResourceManager.cpp
	Engine/Bindings/DX12/UploadBuffer.cpp
	Engine/Bindings/DX12/DX12Buffer.cpp
)

source_group(Engine\\Bindings\\Windows
	FILES
	Engine/Bindings/Windows/WindowWin.cpp
)

source_group(Engine\\Components
	FILES
	Engine/Component/Component.cpp
	Engine/Component/KeyboardComponent.cpp
	Engine/Component/MouseComponent.cpp
	Engine/Component/BatchComponent.cpp
)

source_group(Engine\\Misc
	FILES
	Engine/Misc/Logger.cpp
	Engine/Misc/ThreadPool.cpp
	Engine/Misc/Time.cpp
	Engine/Misc/Transform.cpp
	Engine/Misc/ResourceManager.cpp
	Engine/Misc/OrbLoader.cpp
	Engine/Misc/Random.cpp
)

source_group(Engine\\Rendering
	FILES
	Engine/Rendering/Mesh.cpp
	Engine/Rendering/Light.cpp
	Engine/Rendering/Texture.cpp
	Engine/Rendering/Material.cpp
)

source_group(Engine\\Management
	FILES
	Engine/Management/Allocator.cpp
	Engine/Management/AllocatorPage.cpp
)

source_group(Engine\\Rendering\\Cameras
	FILES
	Engine/Rendering/Cameras/BaseCamera.cpp
	Engine/Rendering/Cameras/ThirdPersonCamera.cpp	
)

set(ORBIT_SOURCE
	zlib/adler32.c
	zlib/compress.c
	zlib/crc32.c
	zlib/deflate.c
	zlib/gzclose.c
	zlib/gzlib.c
    zlib/gzread.c
    zlib/gzwrite.c
	zlib/infback.c
	zlib/inffast.c
	zlib/inflate.c
	zlib/inftrees.c
	zlib/trees.c
	zlib/uncompr.c
	zlib/zutil.c

	Engine/Engine.cpp
	Engine/Object.cpp
	Engine/Scene.cpp
	Engine/DebugObject.cpp

	Engine/Bindings/DDSTextureLoader.cpp
	Engine/Bindings/WICTextureLoader.cpp
	Engine/Bindings/DX/DXGIHelper.cpp
	Engine/Bindings/DX/DXInput.cpp
	Engine/Bindings/DX12/CommandQueue.cpp
	Engine/Bindings/DX12/DX12Engine.cpp
	Engine/Bindings/DX12/DX12Helper.cpp
	Engine/Bindings/DX12/DX12Renderer.cpp
	Engine/Bindings/DX12/DX12ResourceManager.cpp
	Engine/Bindings/DX12/UploadBuffer.cpp
	Engine/Bindings/DX12/DX12Buffer.cpp
	Engine/Bindings/Windows/WindowWin.cpp

	Engine/Component/Component.cpp
	Engine/Component/KeyboardComponent.cpp
	Engine/Component/MouseComponent.cpp
	Engine/Component/BatchComponent.cpp

	Engine/Misc/Logger.cpp
	Engine/Misc/ThreadPool.cpp
	Engine/Misc/Time.cpp
	Engine/Misc/Transform.cpp
	Engine/Misc/ResourceManager.cpp
	Engine/Misc/OrbLoader.cpp
	Engine/Misc/Random.cpp

	Engine/Management/Allocator.cpp
	Engine/Management/AllocatorPage.cpp

	Engine/Rendering/Mesh.cpp
	Engine/Rendering/Light.cpp
	Engine/Rendering/Texture.cpp
	Engine/Rendering/Material.cpp
	Engine/Rendering/Cameras/BaseCamera.cpp
	Engine/Rendering/Cameras/ThirdPersonCamera.cpp
)

set(SOURCE ${ORBIT_SOURCE})

if (WITH_DEAR_IMGUI)
	if ("${IMGUI_ROOT_PATH}" STREQUAL "")
		set(IMGUI_PREFIX "../dep/ImGui")
	else()
		set(IMGUI_PREFIX ${IMGUI_ROOT_PATH})
	endif()

	if ("${ORBIT_RENDERER}" STREQUAL "DX12")
		set(TARGET_FRAMEWORK "dx12")
	elseif("${ORBIT_RENDERER}" STREQUAL "OpenGL")
		set(TARGET_FRAMEWORK "opengl3")
	elseif("${ORBIT_RENDERER}" STREQUAL "DX11")
		set(TARGET_FRAMEWORK "dx11")
	else()
		message(FATAL_ERROR "Invalid render engine selected. Choose one of DX12, OpenGL, DX11.")
	endif()

	set(SOURCE
		${ORBIT_SOURCE}
		${IMGUI_PREFIX}/imgui.cpp
		${IMGUI_PREFIX}/imgui_draw.cpp
		${IMGUI_PREFIX}/imgui_tables.cpp
		${IMGUI_PREFIX}/Backends/imgui_impl_${TARGET_FRAMEWORK}.cpp
		${IMGUI_PREFIX}/Backends/imgui_impl_${TARGET_PLATFORM}.cpp
		${IMGUI_PREFIX}/imgui_widgets.cpp
		${IMGUI_PREFIX}/imgui_demo.cpp
	)
	source_group(ImGui
		FILES
		${IMGUI_PREFIX}/imgui.cpp
		${IMGUI_PREFIX}/imgui_draw.cpp
		${IMGUI_PREFIX}/imgui_tables.cpp
		${IMGUI_PREFIX}/Backends/imgui_impl_${TARGET_FRAMEWORK}.cpp
		${IMGUI_PREFIX}/Backends/imgui_impl_${TARGET_PLATFORM}.cpp
		${IMGUI_PREFIX}/imgui_widgets.cpp
		${IMGUI_PREFIX}/imgui_demo.cpp
	)
endif()

add_library(${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME} 
	PRIVATE 
		PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/inc/>
		PUBLIC $<INSTALL_INTERFACE:${CMAKE_SOURCE_DIR}/inc/> 
		PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/dep/>
		PUBLIC $<INSTALL_INTERFACE:${CMAKE_SOURCE_DIR}/dep/> 
)
generate_export_header(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${Orbit_VERSION})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 0)
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_${PROJECT_NAME}_MAJOR_VERSION 0)
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ${PROJECT_NAME}_MAJOR_VERSION)

if (NOT "${EIGEN_ROOT_PATH}" STREQUAL "")
	target_include_directories(${PROJECT_NAME} PUBLIC ${EIGEN_ROOT_PATH})
endif()
if (NOT "${IMGUI_ROOT_PATH}" STREQUAL "")
	target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_ROOT_PATH})
endif()

physx_dependency(${PROJECT_NAME})
add_compile_definitions(DIRECTINPUT_VERSION=0x0800 NOMINMAX _DISABLE_EXTENDED_ALIGNED_STORAGE ${ORBIT_RENDER_ENGINE})
add_definitions(-DUNICODE)
if (WITH_DEAR_IMGUI)
	add_compile_definitions(ORBIT_WITH_IMGUI)
endif()
