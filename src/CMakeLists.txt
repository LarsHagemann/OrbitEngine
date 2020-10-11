set(ORBIT_SOURCE
	base_camera.cpp
	batch_component.cpp
	component.cpp
	command_queue.cpp
	engine.cpp
	engine_callback.cpp
	engine_initialization.cpp
	exception.cpp
	helper.cpp
	keyboard_component.cpp
	light.cpp
	material.cpp
	mesh.cpp
	mouse_component.cpp
	object.cpp
	resource_manager.cpp
	scene.cpp
	third_person_camera.cpp
	time.cpp
	transform.cpp
)

set(SOURCE ${ORBIT_SOURCE})

if (WITH_DEAR_IMGUI)
	if ("${IMGUI_ROOT_PATH}" STREQUAL "")
		set(IMGUI_PREFIX "../dep/ImGui")
	else()
		set(IMGUI_PREFIX ${IMGUI_ROOT_PATH})
	endif()
	set(SOURCE
		${ORBIT_SOURCE}
		${IMGUI_PREFIX}/imgui.cpp
		${IMGUI_PREFIX}/imgui_draw.cpp
		${IMGUI_PREFIX}/imgui_impl_dx12.cpp
		${IMGUI_PREFIX}/imgui_impl_win32.cpp
		${IMGUI_PREFIX}/imgui_widgets.cpp
		${IMGUI_PREFIX}/imgui_demo.cpp
	)
endif()

add_library(${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME} 
	PRIVATE 
		../inc/ 
		../dep/
)
if (NOT "${EIGEN_ROOT_PATH}" STREQUAL "")
	target_include_directories(${PROJECT_NAME} PRIVATE ${EIGEN_ROOT_PATH})
endif()

physx_dependency(${PROJECT_NAME})
add_compile_definitions(DIRECTINPUT_VERSION=0x0800 NOMINMAX)
add_definitions(-DUNICODE)
if (WITH_DEAR_IMGUI)
	add_compile_definitions(ORBIT_WITH_IMGUI)
endif()
