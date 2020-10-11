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
	set(SOURCE
		${ORBIT_SOURCE}
		../dep/ImGui/imgui.cpp
		../dep/ImGui/imgui_draw.cpp
		../dep/ImGui/imgui_impl_dx12.cpp
		../dep/ImGui/imgui_impl_win32.cpp
		../dep/ImGui/imgui_widgets.cpp
		../dep/ImGui/imgui_demo.cpp
	)
endif()

add_library(${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME} 
	PRIVATE 
		../inc/ 
		../dep/
)
physx_dependency(${PROJECT_NAME})
add_compile_definitions(DIRECTINPUT_VERSION=0x0800 NOMINMAX)
add_definitions(-DUNICODE)
if (WITH_DEAR_IMGUI)
	add_compile_definitions(ORBIT_WITH_IMGUI)
endif()
