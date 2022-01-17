source_group(
	src\\implementation\\imgui
	FILES
	${IMGUI_ROOT_PATH}/imgui.cpp
	${IMGUI_ROOT_PATH}/imgui_draw.cpp
	${IMGUI_ROOT_PATH}/imgui_widgets.cpp
	${IMGUI_ROOT_PATH}/imgui_tables.cpp
)

source_group(
	src\\implementation
	FILES
	implementation/Common.cpp
)

source_group(
    src\\implementation\\backends\\DirectX11
    FILES
    implementation/backends/DirectX11/DirectX11_Engine.cpp
	implementation/backends/DirectX11/DirectX11_PipelineState.cpp
	implementation/backends/DirectX11/DirectX11_IndexBuffer.cpp
	implementation/backends/DirectX11/DirectX11_VertexBuffer.cpp
	implementation/backends/DirectX11/DirectX11_Renderer.cpp
	implementation/backends/DirectX11/DirectX11_VertexShader.cpp
	implementation/backends/DirectX11/DirectX11_PixelShader.cpp
	implementation/backends/DirectX11/DirectX11_GeometryShader.cpp
	implementation/backends/DirectX11/DirectX11_DomainShader.cpp
	implementation/backends/DirectX11/DirectX11_HullShader.cpp
	implementation/backends/DirectX11/DirectX11_InputLayout.cpp
	implementation/backends/DirectX11/DirectX11_Texture.cpp
	implementation/backends/DirectX11/DirectX11_BlendState.cpp
	implementation/backends/DirectX11/DirectX11_SamplerState.cpp
	implementation/backends/DirectX11/DirectX11_RasterizerState.cpp
)

source_group(
    src\\implementation\\backends\\Windows
    FILES
    implementation/backends/Windows/Windows_Window.cpp
)

source_group(
	src\\implementation\\misc
	FILES
	implementation/misc/Time.cpp
	implementation/misc/Transform.cpp
	implementation/misc/Logger.cpp
	implementation/misc/DDSTextureLoader.cpp
	implementation/misc/DirectXHelpers.cpp
	implementation/misc/pch.cpp
	implementation/misc/WICTextureLoader.cpp
)

source_group(
	src\\implementation\\engine
	FILES
	implementation/engine/SceneManager.cpp
	implementation/engine/ResourceManager.cpp
	implementation/engine/AllocatorPage.cpp
	implementation/engine/Allocator.cpp
	implementation/engine/GameObject.cpp
	implementation/engine/PhysxEngine.cpp
)

source_group(
	src\\implementation\\engine\\components
	FILES
	implementation/engine/components/KeyboardComponent.cpp
	implementation/engine/components/MouseComponent.cpp
	implementation/engine/components/BatchComponent.cpp
	implementation/engine/components/StaticBatchComponent.cpp
	implementation/engine/components/RigidStaticComponent.cpp
	implementation/engine/components/RigidDynamicComponent.cpp
)

source_group(
	src\\implementation\\rendering
	FILES
	implementation/rendering/Mesh.cpp
	implementation/rendering/Light.cpp
	implementation/rendering/ThirdPersonCamera.cpp
	implementation/rendering/ParticleSystem.cpp
)

source_group(
	src\\interfaces\\rendering
	FILES
	interfaces/rendering/Material.cpp
	interfaces/rendering/PipelineState.cpp
	interfaces/rendering/Camera.cpp
	interfaces/rendering/Shader.cpp
	interfaces/rendering/Renderer.cpp
	interfaces/rendering/BlendState.cpp
)

source_group(
	src\\interfaces\\engine
	FILES
	interfaces/engine/SceneBase.cpp
	interfaces/engine/EngineBase.cpp
)

source_group(
	src\\interfaces\\misc
	FILES
	interfaces/misc/UnLoadable.cpp
)

set(IMGUI_SOURCE
	${IMGUI_ROOT_PATH}/imgui.cpp
	${IMGUI_ROOT_PATH}/imgui_draw.cpp
	${IMGUI_ROOT_PATH}/imgui_widgets.cpp
	${IMGUI_ROOT_PATH}/imgui_tables.cpp
)

if ("${ORBIT_RENDERER}" STREQUAL "DX12")
	source_group(src\\implementation\\imgui\\renderer FILES ${IMGUI_ROOT_PATH}/backends/imgui_impl_dx12.cpp)
	set(IMGUI_SOURCE ${IMGUI_SOURCE} ${IMGUI_ROOT_PATH}/backends/imgui_impl_dx12.cpp)
elseif ("${ORBIT_RENDERER}" STREQUAL "OpenGL")
	source_group(src\\implementation\\imgui\\renderer FILES ${IMGUI_ROOT_PATH}/backends/imgui_impl_glut.cpp)
	set(IMGUI_SOURCE ${IMGUI_SOURCE} ${IMGUI_ROOT_PATH}/backends/imgui_impl_glut.cpp)
elseif ("${ORBIT_RENDERER}" STREQUAL "DX11")
	source_group(src\\implementation\\imgui\\renderer FILES ${IMGUI_ROOT_PATH}/backends/imgui_impl_dx11.cpp)
	set(IMGUI_SOURCE ${IMGUI_SOURCE} ${IMGUI_ROOT_PATH}/backends/imgui_impl_dx11.cpp)
endif()

if ("${ORBIT_OS}" STREQUAL "Windows")
	source_group(src\\implementation\\imgui\\system FILES ${IMGUI_ROOT_PATH}/backends/imgui_impl_win32.cpp)
	set(IMGUI_SOURCE ${IMGUI_SOURCE} ${IMGUI_ROOT_PATH}/backends/imgui_impl_win32.cpp)
elseif("${ORBIT_OS}" STREQUAL "Unix")
	#source_group(src\\implementation\\imgui\\system FILES ${IMGUI_ROOT_PATH}/backends/imgui_impl_unix.cpp)
	#set(IMGUI_SOURCE ${IMGUI_SOURCE} ${IMGUI_ROOT_PATH}/backends/imgui_impl_unix.cpp)
endif()

set(ORBIT_LIB_SOURCE
	implementation/Common.cpp

	implementation/backends/DirectX11/DirectX11_Engine.cpp
	implementation/backends/DirectX11/DirectX11_PipelineState.cpp
	implementation/backends/DirectX11/DirectX11_IndexBuffer.cpp
	implementation/backends/DirectX11/DirectX11_VertexBuffer.cpp
	implementation/backends/DirectX11/DirectX11_Renderer.cpp
	implementation/backends/DirectX11/DirectX11_VertexShader.cpp
	implementation/backends/DirectX11/DirectX11_PixelShader.cpp
	implementation/backends/DirectX11/DirectX11_GeometryShader.cpp
	implementation/backends/DirectX11/DirectX11_DomainShader.cpp
	implementation/backends/DirectX11/DirectX11_HullShader.cpp
	implementation/backends/DirectX11/DirectX11_InputLayout.cpp
	implementation/backends/DirectX11/DirectX11_Texture.cpp
	implementation/backends/DirectX11/DirectX11_BlendState.cpp
	implementation/backends/DirectX11/DirectX11_SamplerState.cpp
	implementation/backends/DirectX11/DirectX11_RasterizerState.cpp
    
    implementation/backends/Windows/Windows_Window.cpp

	implementation/engine/SceneManager.cpp
	implementation/engine/ResourceManager.cpp
	implementation/engine/AllocatorPage.cpp
	implementation/engine/Allocator.cpp
	implementation/engine/GameObject.cpp
	implementation/engine/PhysxEngine.cpp

	implementation/engine/components/KeyboardComponent.cpp
	implementation/engine/components/MouseComponent.cpp
	implementation/engine/components/BatchComponent.cpp
	implementation/engine/components/StaticBatchComponent.cpp
	implementation/engine/components/RigidStaticComponent.cpp
	implementation/engine/components/RigidDynamicComponent.cpp

	implementation/misc/Time.cpp
	implementation/misc/Transform.cpp
	implementation/misc/Logger.cpp
	implementation/misc/DDSTextureLoader.cpp
	implementation/misc/DirectXHelpers.cpp
	implementation/misc/pch.cpp
	implementation/misc/WICTextureLoader.cpp

	implementation/rendering/Light.cpp
	implementation/rendering/ThirdPersonCamera.cpp
	implementation/rendering/ParticleSystem.cpp
	
	interfaces/rendering/Material.cpp
	interfaces/rendering/PipelineState.cpp
	interfaces/rendering/Camera.cpp
	interfaces/rendering/Shader.cpp
	interfaces/rendering/Renderer.cpp
	interfaces/rendering/BlendState.cpp

	interfaces/engine/SceneBase.cpp
	interfaces/engine/EngineBase.cpp

	interfaces/misc/UnLoadable.cpp

	${IMGUI_SOURCE}
)

add_library(${PROJECT_NAME} ${ORBIT_LIB_SOURCE})
target_include_directories(${PROJECT_NAME} 
	PRIVATE 
		PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/inc/>
		PUBLIC $<INSTALL_INTERFACE:${CMAKE_SOURCE_DIR}/inc/> 
		PUBLIC $<BUILD_INTERFACE:${ZLIB_DIR}>
		PUBLIC $<INSTALL_INTERFACE:${ZLIB_DIR}>
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	${PHYSX_ROOT_PATH}/physx/include/
	${PHYSX_ROOT_PATH}/pxshared/include/
)

if (NOT "${EIGEN_ROOT_PATH}" STREQUAL "")
	target_include_directories(${PROJECT_NAME} PUBLIC ${EIGEN_ROOT_PATH})
else()
	message(WARNING "EIGEN_ROOT_PATH not set. Compilation will fail")
endif()

if (NOT "${IMGUI_ROOT_PATH}" STREQUAL "")
	target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_ROOT_PATH})
else()
	message(WARNING "IMGUI_ROOT_PATH not set. Compilation will fail")
endif()

add_compile_definitions(DIRECTINPUT_VERSION=0x0800 PX_PHYSX_STATIC_LIB VC_EXTRALEAN WIN32_LEAN_AND_MEAN NOMINMAX _DISABLE_EXTENDED_ALIGNED_STORAGE UNICODE ${ORBIT_RENDER_ENGINE} ${ORBIT_SYSTEM})

if ("${ORBIT_RENDER_ENGINE}" STREQUAL "ORBIT_DIRECTX_11")
	target_link_libraries(${PROJECT_NAME} PRIVATE "dinput8.lib" "dxguid.lib" "d3d11.lib" "d3dcompiler.lib")
elseif("${ORBIT_RENDER_ENGINE}" STREQUAL "ORBIT_DIRECTX_12")
	target_link_libraries(${PROJECT_NAME} PRIVATE "dinput8.lib" "dxguid.lib" "d3d12.lib" "d3dcompiler.lib")
elseif("${ORBIT_RENDER_ENGINE}" STREQUAL "ORBIT_OPENGL")

endif()